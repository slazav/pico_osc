#include <math.h>
#include <unistd.h>
#include "pico.h"

int16_t res;

/********************************************************************/
/* open device */
int
dev_open(spars_t *pars){
  if (pars->h > 0) return 1;
  if (pars->dev && strlen(pars->dev)==0) pars->dev=0;
  res = ps3000aOpenUnit(&(pars->h), pars->dev);
  if (pars->h < 1){  printf("error: %s\n", pico_err(res)); return 1; }
  return 0;
}

/********************************************************************/
/* close device */
int
dev_close(spars_t *pars){
  if (pars->h<1) return 1;
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
void pico_command(spars_t *spars, cpars_t *cpars, opars_t *opars){

  /* default values */
  opars->dsize=0;
  opars->data=NULL;
  opars->status="UNKNOWN_COMMAND";
  opars->headers[0].key = NULL;

/* print to a buffer - for building help messages */
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

/* similat to PRINT macro above, but with ps3000aGetUnitInfo command */
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

    /* allocate buffers */
    /*  -->  [bufs1 (int16)] --> normal
     *        |   |
     *        |  [bufs2 (int16)] --> avrg
     *        |
     *       [bufd1 (double)] --> fourier
     *            |
     *           [bufd2 (double)] --> avrg fourier
     */
    int16_t *bufs1=NULL, *bufs2=NULL;
    double  *bufd1=NULL, *bufd2=NULL;
    int avrg = cpars->rec_block_avrg;
    int fft  = cpars->rec_block_fft;
    int nrec = cpars->rec_block_points;
    int size_s = nrec*MAXCH*sizeof(int16_t);
    int size_d = nrec*MAXCH*sizeof(double);

    if (avrg<1) avrg=1;

    /* prepare data buffers */
    bufs1 = malloc(size_s);   /* buffer for the osc output, always exists */
    if (fft==0){
      if (avrg==1){
        opars->dsize = size_s;
        opars->data  = bufs1;
      }
      else{
        bufs2 = malloc(size_s); /* buffer for averaging */
        opars->dsize = size_s;
        opars->data  = bufs2;
      }
    }
    else {/* fft*/
      bufd1 = malloc(size_d*2); /* complex buffer for calculation*/
      bufd2 = malloc(size_d);   /* output buffer */
      opars->dsize = size_d;
      opars->data  = bufd2;
    }
    /*do recording*/
    res = do_rec_block(spars, cpars, opars, bufs1, bufs2, bufd1, bufd2);

    /* free data buffers, excluding the output buffer */
    if (bufs1 && bufs1!=opars->data) { free(bufs1); }
    if (bufs2 && bufs2!=opars->data) { free(bufs2); }
    if (bufd1 && bufd1!=opars->data) { free(bufd1); }
    if (bufd2 && bufd2!=opars->data) { free(bufd2); }

    /* free output buffer in case of error */
    if (res!=0){
      free(opars->data);
      opars->dsize=0;
    }

    return;
  }
}

