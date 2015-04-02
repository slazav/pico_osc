#include <math.h>
#include <unistd.h>
#include "pico.h"

int16_t res;
#define MAXCH 2

/********************************************************************/
/* open device */
int
dev_open(pico_spars_t *pars){
  if (pars->h > 0) return 1;
  if (pars->dev && strlen(pars->dev)==0) pars->dev=0;
  res = ps3000aOpenUnit(&(pars->h), pars->dev);
  if (pars->h < 1){  printf("error: %s\n", pico_err(res)); return 1; }
  return 0;
}

/********************************************************************/
/* close device */
int
dev_close(pico_spars_t *pars){
  if (pars->h<1) return;
  res = ps3000aCloseUnit(pars->h);
  if (res!=PICO_OK) { printf("error: %s\n", pico_err(res)); return 1;}
  return 0;
}

/********************************************************************/
/* look for pico devices and print the list */
int
dev_list(){
  int16_t cnt, len;
  const int buflen=2048;
  char buf[buflen];
  len=buflen;
  res=ps3000aEnumerateUnits(&cnt, buf, &len);
  if (res!=PICO_OK){ printf("error: %s\n", pico_err(res));  return 1;}
  printf("devices found: %d\n", cnt);
  if (cnt>0) printf("  %s\n", buf);
  return 0;
}

/* add info into str_pair_t array */
#define print_kv(ARR, KEY, ...)\
  { str_pair_t *hh;\
    for (hh=ARR; hh->key && hh-ARR < MAXHEADS-2; hh++); \
    hh->key=KEY; snprintf(hh->val, MAXHVAL, __VA_ARGS__); \
    hh++; hh->key=NULL; }

/********************************************************************/
/* run a command */
void pico_command(pico_spars_t *spars, pico_cpars_t *cpars,
                  pico_opars_t *opars){

  /* default values */
  opars->dsize=0;
  opars->data=NULL;
  opars->status="UNKNOWN_COMMAND";
  opars->headers[0].key = NULL;

/* print to a buffer */
#define PRINT(...)\
    opars->dsize += snprintf(opars->data + opars->dsize,\
                             bufsize - opars->dsize, __VA_ARGS__);

  /*********************************************************/
  /* help command */
  if (strcmp(cpars->command, "help")==0){
    int16_t bufsize = 2048;
    opars->data=malloc(bufsize); opars->dsize=0;
    if (opars->data==NULL){opars->status = "MALLOC_ERROR"; return;}
    PRINT("Commands:\n");
    PRINT("  help -- print help message\n");
    PRINT("  ping -- ping the device\n");
    PRINT("  info -- get device information\n");
    PRINT("  set_chan -- set channel parameters\n");
    PRINT("  set_trig -- set trigger parameters\n");
    PRINT("  set_gen  -- set generator parameters\n");
    PRINT("  trig_gen -- trigger the generator\n");
    PRINT("Ask /command?help to see command parameters.\n\n");
    opars->status = pico_err(0);
    return;
  }

  /*********************************************************/
  /* help argument for a command */
  if (cpars->_help){
    int16_t bufsize = 2048;
    opars->data=malloc(bufsize); opars->dsize=0;
    if (opars->data==NULL){opars->status = "MALLOC_ERROR"; return;}
    PRINT("%s parameters:\n", cpars->command);
#undef CPAR
#define CPAR(comm, type, par, short, cnv, def, descr)\
    if (strlen(#comm)==0 || strcmp(#comm, cpars->command)==0)\
      PRINT("  %s -- %s (default: %s)\n", #par, descr, #def);
#include "pars.def"
    PRINT("\n");
    opars->status = pico_err(0);
    return;
  }

  /*********************************************************/
  /* ping command */
  if (strcmp(cpars->command, "ping")==0){
    res = ps3000aPingUnit(spars->h);
    opars->status = pico_err(res);
    return;
  }

  /*********************************************************/
  /* info command */
  if (strcmp(cpars->command, "info")==0){
    int16_t bufsize = 2048, len;
    char buf[bufsize];

    opars->data=malloc(bufsize); opars->dsize=0;
    if (opars->data==NULL){
      opars->status = "MALLOC_ERROR";
      return;
    }

#define ADD_INFO(inf, title)\
    res=ps3000aGetUnitInfo(spars->h, buf, bufsize, &len, inf);\
    if (res!=PICO_OK) {opars->status = pico_err(res); return;}\
    else opars->dsize += snprintf(opars->data + opars->dsize,\
       bufsize - opars->dsize, "%s %s\n", title, buf);
    ADD_INFO( PICO_VARIANT_INFO,              "device type:      ");
    ADD_INFO( PICO_BATCH_AND_SERIAL,          "batch and serial: ");
    ADD_INFO( PICO_CAL_DATE,                  "calendar date:    ");
    ADD_INFO( PICO_DRIVER_VERSION,            "driver version:   ");
    ADD_INFO( PICO_USB_VERSION,               "usb version:      ");
    ADD_INFO( PICO_KERNEL_VERSION,            "kernel driver:    ");
    ADD_INFO( PICO_HARDWARE_VERSION,          "hardware version: ");
    ADD_INFO( PICO_DIGITAL_HARDWARE_VERSION,  "digital hardware version: ");
    ADD_INFO( PICO_ANALOGUE_HARDWARE_VERSION, "analog hardware version:  ");
    opars->status = pico_err(0);
    return;
  }

  /*********************************************************/
  /* set_chan command */
  if (strcmp(cpars->command, "set_chan")==0){
    res = ps3000aSetChannel(spars->h,
      cpars->set_chan_channel,
      cpars->set_chan_enable,
      cpars->set_chan_coupling,
      cpars->set_chan_range,
      cpars->set_chan_offset
    );
    opars->status = pico_err(res);
    return;
  }

  /*********************************************************/
  /* set_trig command */
  if (strcmp(cpars->command, "set_trig")==0){
    res = ps3000aSetSimpleTrigger(spars->h,
      cpars->set_trig_enable,
      cpars->set_trig_src,
      cpars->set_trig_thr,
      cpars->set_trig_dir,
      cpars->set_trig_del,
      cpars->set_trig_autotrig
    );
    opars->status = pico_err(res);
    return;
  }

  /*********************************************************/
  /* set_gen command */
  if (strcmp(cpars->command, "set_gen")==0){
    res = ps3000aSetSigGenBuiltInV2(spars->h,
      cpars->set_gen_offset,
      cpars->set_gen_volt,
      cpars->set_gen_wave,
      cpars->set_gen_f1,
      cpars->set_gen_f2,
      cpars->set_gen_incr,
      cpars->set_gen_dwell,
      cpars->set_gen_sweep,
      PS3000A_ES_OFF,
      cpars->set_gen_cycles,
      cpars->set_gen_sweeps,
      cpars->set_gen_trig_dir,
      cpars->set_gen_trig_src,
      cpars->set_gen_trig_thr
    );
    opars->status = pico_err(res);
    return;
  }

  /*********************************************************/
  /* trig_gen command */
  if (strcmp(cpars->command, "trig_gen")==0){
    res=ps3000aSigGenSoftwareControl(spars->h, PS3000A_SIGGEN_GATE_HIGH);
    opars->status = pico_err(res);
    return;
  }


  /*********************************************************/
  /* rec_block command */
  if (strcmp(cpars->command, "rec_block")==0){

    int    avrg = cpars->rec_block_avrg;
    double pretrig = cpars->rec_block_pretrig/100.0;
    int    nrec = cpars->rec_block_points;
    int    npre = ceil(nrec * pretrig);
    double time = cpars->rec_block_time;
    double rate = nrec/time;
    double dt   = 1.0/rate;

    uint32_t tbase;
    int16_t stat = 0, overload = 0, ch;
    int32_t t_est; /* ms */
    int64_t ttime;
    PS3000A_TIME_UNITS tunits;
    double ttimed;
    str_pair_t *hh;
    void * buffer;
    int n;

    /* parameters */
    if (avrg<1) avrg=1;
    if (npre<0) npre=0;
    if (npre>nrec) npre=nrec;

    /* calculate timebase and actual dt */
    res = rec_timebase(spars->h, &dt, &tbase);
    if (res!=PICO_OK){
      opars->status = pico_err(res);
      return;
    }

    /* prepare data buffers */
    opars->dsize=nrec*MAXCH*sizeof(int16_t);
    /* separate buffer is needed only for averaging */
    buffer=malloc(opars->dsize);
    if (avrg>1)
      opars->data=malloc(opars->dsize);
    else
      opars->data=buffer;
    if (opars->data==NULL || buffer==NULL){
      opars->status = "MALLOC_ERROR";
      opars->data=NULL;
      opars->dsize=0;
      return;
    }

    /* show the buffer to the oscilloscope */
    for (ch=0; ch<MAXCH; ch++){
      res=ps3000aSetDataBuffer(spars->h, (PS3000A_CHANNEL)ch,
        buffer + ch*nrec*sizeof(int16_t), nrec*sizeof(int16_t),
        0, PS3000A_RATIO_MODE_NONE);
      if (res!=PICO_OK) {
        if (spars->verb)
          printf("Error: rec_block: SetDataBuffer: %s\n",
                 pico_err(res));
        free(opars->data);
        if (avrg>1) free(buffer);
        opars->data=NULL;
        opars->dsize=0;
        opars->status = pico_err(res);
        return;
      }
    }


    /* get and average data*/

    for (n=0; n<avrg; n++){
      /* run the oscilloscope */
      res = ps3000aRunBlock(spars->h,
        npre, nrec-npre, tbase, 0, &t_est, 0, NULL, NULL);
      if (res) {
        if (spars->verb)
          printf("Error: rec_block: RunBlock: %s\n",
                 pico_err(res));
        free(opars->data);
        if (avrg>1) free(buffer);
        opars->data=NULL;
        opars->status = pico_err(res);
        return;
      }

      /* wait and trigger generator if needed */
      if (cpars->rec_block_triggen){
        usleep(10000);
        usleep(dt*npre);
        res=ps3000aSigGenSoftwareControl(spars->h, PS3000A_SIGGEN_GATE_HIGH);
        if (res) {
          if (spars->verb)
            printf("Error: rec_block: SigGenSoftwareControl: %s\n",
                   pico_err(res));
        }
      }

      /* wait for data */
      usleep(dt*(nrec-npre));
      for (stat=0; stat==0;){
        usleep(dt*(nrec-npre) * 1e6/10);
        res = ps3000aIsReady(spars->h, &stat);
        if (res) {
          if (spars->verb)
            printf("Error: rec_block: IsReady: %s\n",
                   pico_err(res));
          free(opars->data);
          if (avrg>1) free(buffer);
          opars->data=NULL;
          opars->status = pico_err(res);
          return;
        }
      }


      /* get values */
      res = ps3000aGetValues(spars->h, 0, &nrec,
         1, PS3000A_RATIO_MODE_NONE, 0, &overload);
      if (res!=PICO_OK) {
        if (spars->verb)
          printf("Error: rec_block: GetValues: %s\n",
                 pico_err(res));
        free(opars->data);
        if (avrg>1) free(buffer);
        opars->data=NULL;
        opars->dsize=0;
        opars->status = pico_err(res);
        return;
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
          printf("Error: rec_block: Stop: %s\n",
                 pico_err(res));
        free(opars->data);
        if (avrg>1) free(buffer);
        opars->data=NULL;
        opars->dsize=0;
        opars->status = pico_err(res);
        return;
      }

      /* do averaging */
      if (avrg>1){
        int i;
        for (i=0; i<nrec*MAXCH; i++){
          int16_t vn = ((int16_t *)opars->data)[i];
          int16_t v1 = ((int16_t *)buffer)[i];
          ((int16_t *)opars->data)[i] =
            ((long long int)vn*n + v1)/(n+1);
        }
      }
    }

    if (avrg>1) free(buffer);

    /* fill headers */
    opars->status = pico_err(0);
    print_kv(opars->headers, "Overload", "%d", overload!=0);
    print_kv(opars->headers, "TrigTime", "%e", ttimed);
    print_kv(opars->headers, "TrigSamp", "%d", npre);
    print_kv(opars->headers, "DT",       "%e", dt);
    print_kv(opars->headers, "Samples",  "%d", nrec);

    return;
  }
}

