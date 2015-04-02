#include <math.h>
#include <unistd.h>
#include "pico.h"

/* Non-trivial conversion from rate to pico timebase
 * input:  device handler, desired dt (s)
 * output: actual dt (s), pico timebase, result
 */
uint32_t
rec_timebase(uint16_t h, double * dt, uint32_t * tbase){
  float    dt1,dt2;
  int16_t  res;
  uint32_t tb;
  if (*dt <= 16e-9){
    /* what is timebase 0? */
    res = ps3000aGetTimebase2(h,
              0, 1, &dt1, 0, NULL, 0);
    if (res!=PICO_OK) return res;

    tb = round(log(*dt*1e9/dt1)/log(2.0));
  }
  else {
    /* what is timebase 4 and 5*/
    res = ps3000aGetTimebase2(h,
              4, 1, &dt1, 0, NULL, 0);
    if (res!=PICO_OK) return res;

    res = ps3000aGetTimebase2(h,
              5, 1, &dt2, 0, NULL, 0);
    if (res!=PICO_OK) return res;

    tb = round((*dt*1e9-dt1)/(dt2-dt1))+4;
  }
  /* final calculation */
  res = ps3000aGetTimebase2(h,
          tb, 1, &dt1, 0, NULL, 0);
  if (res!=PICO_OK) return res;

  *dt = dt1*1e-9; /* ns -> s*/
  *tbase = tb;
  return PICO_OK;
}


/* Do block recording.
 *   buffers should be prerared;
 *   returns 0 on success, -1 on error
 */
int
do_rec_block(spars_t *spars, cpars_t *cpars, opars_t *opars,
      int16_t *bufs1, int16_t *bufs2, double *bufd1, double *bufd2){

  int    avrg = cpars->rec_block_avrg;
  int    fft  = cpars->rec_block_fft;
  double pretrig = cpars->rec_block_pretrig/100.0;
  int    nrec = cpars->rec_block_points;
  int    npre = ceil(nrec * pretrig);
  double time = cpars->rec_block_time;
  double rate = nrec/time;
  double dt   = 1.0/rate;
  int16_t res;

  uint32_t tbase;
  int16_t stat = 0, overload = 0, ch;
  int32_t t_est; /* ms */
  int64_t ttime;
  PS3000A_TIME_UNITS tunits;
  double ttimed;
  str_pair_t *hh;
  int n;

  /* parameters */
  if (avrg<1) avrg=1;
  if (npre<0) npre=0;
  if (npre>nrec) npre=nrec;

  /* calculate timebase and actual dt */
  res = rec_timebase(spars->h, &dt, &tbase);
  if (res!=PICO_OK){
    opars->status = pico_err(res);
    return -1;
  }

  /* show the buffer to the oscilloscope */
  for (ch=0; ch<MAXCH; ch++){
    res=ps3000aSetDataBuffer(spars->h, (PS3000A_CHANNEL)ch,
          (void *)bufs1 + ch*nrec*sizeof(int16_t), nrec*sizeof(int16_t),
          0, PS3000A_RATIO_MODE_NONE);
    if (res!=PICO_OK) {
      if (spars->verb)
        printf("Error: rec_block: SetDataBuffer: %s\n", pico_err(res));
      opars->status = pico_err(res);
      return -1;
    }
  }

  /* get and average data*/
  for (n=0; n<avrg; n++){

    /* run the oscilloscope */
    res = ps3000aRunBlock(spars->h,
        npre, nrec-npre, tbase, 0, &t_est, 0, NULL, NULL);
    if (res) {
      if (spars->verb)
        printf("Error: rec_block: RunBlock: %s\n", pico_err(res));
      opars->status = pico_err(res);
      return -1;
    }

    /* wait and trigger generator if needed */
    if (cpars->rec_block_triggen){
      usleep(10000);
      usleep(dt*npre);
      res=ps3000aSigGenSoftwareControl(spars->h, PS3000A_SIGGEN_GATE_HIGH);
      if (res) {
        if (spars->verb)
          printf("Error: rec_block: SigGenSoftwareControl: %s\n", pico_err(res));
      }
    }

    /* wait for data */
    usleep(dt*(nrec-npre));
    for (stat=0; stat==0;){
      usleep(dt*(nrec-npre) * 1e6/10);
      res = ps3000aIsReady(spars->h, &stat);
      if (res) {
        if (spars->verb)
          printf("Error: rec_block: IsReady: %s\n", pico_err(res));
        opars->status = pico_err(res);
        return -1;
      }
    }

    /* get values */
    res = ps3000aGetValues(spars->h, 0, &nrec,
            1, PS3000A_RATIO_MODE_NONE, 0, &overload);
    if (res!=PICO_OK) {
      if (spars->verb)
        printf("Error: rec_block: GetValues: %s\n", pico_err(res));
      opars->status = pico_err(res);
      return -1;
    }

    /* get trigger position */
    res = ps3000aGetTriggerTimeOffset64(spars->h,
            &ttime, &tunits, 0);
    if (res!=PICO_OK) { ttime=0; tunits=0; }
    ttimed = time2dbl(ttime, tunits);

    /* stop oscilloscope */
    res = ps3000aStop(spars->h);
    if (res!=PICO_OK) {
      if (spars->verb)
        printf("Error: rec_block: Stop: %s\n", pico_err(res));
      opars->status = pico_err(res);
      return -1;
    }

    /* do averaging */
    if (avrg>1 && fft==0){
      int i;
      for (i=0; i<nrec*MAXCH; i++){
        int16_t vn = bufs2[i];
        int16_t v1 = bufs1[i];
        bufs2[i] = ((long long int)vn*n + v1)/(n+1);
      }
    }

    /* do fft */
    if (fft){
      int i;
      for (i=0; i<nrec*MAXCH; i++){
        bufd1[i] = (double)(bufs1[i]);
        if (avrg>1){
          bufd2[i] = (bufd2[i]*n + bufd1[i])/(n+1);
        }
      }
    }

  }

  /* fill headers */
  opars->status = pico_err(0);
  print_kv(opars->headers, "Overload", "%d", overload!=0);
  print_kv(opars->headers, "TrigTime", "%e", ttimed);
  print_kv(opars->headers, "TrigSamp", "%d", npre);
  print_kv(opars->headers, "DT",       "%e", dt);
  print_kv(opars->headers, "Samples",  "%d", nrec);

  return 0;
}

