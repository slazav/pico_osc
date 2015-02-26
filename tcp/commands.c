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

  /* ping command */
  if (strcmp(cpars->command, "ping")==0){
    res = ps3000aPingUnit(spars->h);
    opars->status = pico_err(res);
    return;
  }

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
    if (spars->verb){
      printf("set_gen:\n");
      printf("  offset:   %d\n", cpars->set_gen_offset);
      printf("  volt:     %d\n", cpars->set_gen_volt);
      printf("  wave:     %d\n", cpars->set_gen_wave);
      printf("  f1:       %f\n", cpars->set_gen_f1);
      printf("  f2:       %f\n", cpars->set_gen_f2);
      printf("  incr:     %f\n", cpars->set_gen_incr);
      printf("  dwell:    %f\n", cpars->set_gen_dwell);
      printf("  sweep:    %d\n", cpars->set_gen_sweep);
      printf("  cycles:   %d\n", cpars->set_gen_cycles);
      printf("  sweeps:   %d\n", cpars->set_gen_sweeps);
      printf("  trig_dir: %d\n", cpars->set_gen_trig_dir);
      printf("  trig_src: %d\n", cpars->set_gen_trig_src);
      printf("  trig_thr: %d\n", cpars->set_gen_trig_thr);
    }
    opars->status = pico_err(res);
    return;
  }

  /* trig_gen command */
  if (strcmp(cpars->command, "trig_gen")==0){
    res=ps3000aSigGenSoftwareControl(spars->h, PS3000A_SIGGEN_GATE_HIGH);
    opars->status = pico_err(res);
    return;
  }


  /* rec_block command */
  if (strcmp(cpars->command, "rec_block")==0){
    int npre;
    int nrec;
    uint32_t tbase;
    int16_t stat = 0, overload = 0, ch;
    int32_t num, t_est; /* ms */
    int64_t ttime;
    PS3000A_TIME_UNITS tunits;
    double ttimed, dt = abs(1e9/cpars->rec_block_rate);

    /* Non-trivial conversion from rate to pico timebase */
    {
      float    dt1,dt2;
      cpars->rec_block_rate;
      if (dt <= 16e-9){
        /* what is timebase 0? */
        res = ps3000aGetTimebase2(spars->h,
                  0, 1, &dt1, 0, NULL, 0);
        tbase = round(log(dt/dt1)/log(2.0));
      }
      else {
        /* what is timebase 4 and 5*/
        res = ps3000aGetTimebase2(spars->h,
                  4, 1, &dt1, 0, NULL, 0);
        res = ps3000aGetTimebase2(spars->h,
                  5, 1, &dt2, 0, NULL, 0);
        tbase = round((dt-dt1)/(dt2-dt1))+4;
      }
      /* final calculation */
      res = ps3000aGetTimebase2(spars->h,
              tbase, 1, &dt1, 0, NULL, 0);
      dt=dt1*1e-9; /*dt1, ns*/
      npre = cpars->rec_block_pretrig/dt;
      nrec = cpars->rec_block_time/dt;
    }

    /* run the oscilloscope */
    res = ps3000aRunBlock(spars->h,
      npre, nrec, tbase, 0, &t_est, 0, NULL, NULL);
    if (res) {
      opars->status = pico_err(res);
      return;
    }

    /* wait and trigger generator if needed */
    usleep(10000);
    usleep(cpars->rec_block_pretrig * 1e6);
    if (cpars->rec_block_triggen)
      res=ps3000aSigGenSoftwareControl(spars->h, PS3000A_SIGGEN_GATE_HIGH);
    usleep(cpars->rec_block_time * 1e6);

    while (stat == 0){
      usleep(cpars->rec_block_time * 1e6/10);
      res = ps3000aIsReady(spars->h, &stat);
      if (res) {
        opars->status = pico_err(res);
        return;
      }
    }

    /* prepare data buffers */
    num = (npre + nrec);
    opars->dsize=num*MAXCH*sizeof(int16_t);
    opars->data=malloc(opars->dsize); 
    if (opars->data==NULL){
      opars->status = "MALLOC_ERROR";
      opars->data=NULL;
      opars->dsize=0;
      return;
    }

    /* show these buffers to the oscilloscope */
    for (ch=0; ch<MAXCH; ch++){
      res=ps3000aSetDataBuffer(spars->h, (PS3000A_CHANNEL)ch,
        opars->data + ch*num*sizeof(int16_t), num*sizeof(int16_t),
        0, PS3000A_RATIO_MODE_NONE);
      if (res!=PICO_OK) {
        free(opars->data);
        opars->data=NULL;
        opars->dsize=0;
        opars->status = pico_err(res);
        return;
      }
    }

    /* get values */
    res = ps3000aGetValues(spars->h, 0, &num,
       1, PS3000A_RATIO_MODE_NONE, 0, &overload);
    if (res!=PICO_OK) {
      free(opars->data);
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
      free(opars->data);
      opars->data=NULL;
      opars->dsize=0;
      opars->status = pico_err(res);
      return;
    }

    /* fill headers */
    opars->status = pico_err(0);
    opars->headers[0].key = "Overload";
    snprintf(opars->headers[0].val, MAXHVAL, "%d", overload!=0);

    opars->headers[1].key = "TrigTime";
    snprintf(opars->headers[1].val, MAXHVAL, "%e", ttimed);
    opars->headers[2].key = "TrigSamp";
    snprintf(opars->headers[2].val, MAXHVAL, "%d", npre);
    opars->headers[3].key = "DT";
    snprintf(opars->headers[3].val, MAXHVAL, "%e", dt);
    opars->headers[4].key = "Samples";
    snprintf(opars->headers[4].val, MAXHVAL, "%d", num);

    opars->headers[5].key = NULL;
    return;
  }
}

