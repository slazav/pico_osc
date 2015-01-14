#include <ps3000aApi.h>
int16_t res;


/********************************************************************/
/* open device */
void
dev_open(pico_pars_t *pars){
  if (pars->h > 0) return;
  res = ps3000aOpenUnit(&(pars->h), pars->dev);
  if (pars->h < 1){ print_err(res); exit(1); }
}

/********************************************************************/
/* close device */
void
dev_close(pico_pars_t *pars){
  if (pars->h<1) return;
  res = ps3000aCloseUnit(pars->h);
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* open log file */
void
cmd_log(pico_pars_t *pars){
  if (pars->file==NULL) pars->file="rec.log";
  pars->log = fopen(pars->file, "w");
  if (!pars->log)
    fprintf(stderr, "Error: can't open file: %s\n", pars->file);
}

/********************************************************************/
/* close log file if needed */
void
log_close(pico_pars_t *pars){
  if (pars->log) fclose(pars->log);
}

/********************************************************************/
/* look for pico devices and print the list */
void
cmd_find(pico_pars_t *pars){
  int16_t cnt, len;
  const int buflen=2048;
  char buf[buflen];
  len=buflen;
  res=ps3000aEnumerateUnits(&cnt, buf, &len);
  if (res!=PICO_OK){ print_err(res);  exit(1); }
  printf("devices found: %d", cnt);
  if (cnt>0) printf(", serials: %s\n", buf);
  else printf("\n");
}

/********************************************************************/
/* ping device */
void
cmd_ping(pico_pars_t *pars){
  dev_open(pars);
  res=ps3000aPingUnit(pars->h);
  printf("ping: ");
  print_err(res);
}


/********************************************************************/
/* print device info */
void
cmd_info(pico_pars_t *pars){
  int16_t len;
  const int buflen=2048;
  char buf[buflen];

  printf("device information:\n");

  dev_open(pars);
  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_VARIANT_INFO);
  if (res!=PICO_OK) print_err(res);
  else printf("  device type:      %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_BATCH_AND_SERIAL);
  if (res!=PICO_OK) print_err(res);
  else printf("  batch and serial: %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_CAL_DATE);
  if (res!=PICO_OK) print_err(res);
  else printf("  calendar date:    %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_DRIVER_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  driver version:   %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_USB_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  usb version:      %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_HARDWARE_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  hardware version: %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_KERNEL_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  kernel driver:    %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_DIGITAL_HARDWARE_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  hardware version of the digital section: %s\n", buf);

  res=ps3000aGetUnitInfo(pars->h, buf, buflen, &len, PICO_ANALOGUE_HARDWARE_VERSION);
  if (res!=PICO_OK) print_err(res);
  else printf("  hardware version of the analog section:  %s\n", buf);
}


/********************************************************************/
/* set up a channel */
void
cmd_chan(pico_pars_t *pars){
  dev_open(pars);
  res = ps3000aSetChannel(pars->h,
     pars->channel,
     pars->enable,
     pars->coupling,
     pars->range,
     pars->offset);
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* set up a trigger */
void
cmd_trig(pico_pars_t *pars){
  dev_open(pars);
  res = ps3000aSetSimpleTrigger(pars->h,
     pars->enable,
     pars->src,
     pars->thresh,
     pars->dir,
     pars->delay,
     pars->autotrig);
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* set up a generator */
void
cmd_gen(pico_pars_t *pars){
  dev_open(pars);
  res = ps3000aSetSigGenBuiltInV2(pars->h,
    pars->gen_offs,
    pars->pk2pk,
    pars->wave,
    pars->startf,
    pars->stopf,
    pars->incr,
    pars->dwell,
    pars->sweep,
    PS3000A_ES_OFF,
    pars->cycles,
    pars->sweeps,
    pars->trig_gen_dir,
    pars->trig_gen_src,
    pars->trig_gen_thr
  );
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* trigger the generator */
void
cmd_trig_gen(pico_pars_t *pars){
  dev_open(pars);
  res=ps3000aSigGenSoftwareControl(pars->h, PS3000A_SIGGEN_GATE_HIGH);
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* wait */
void
cmd_wait(pico_pars_t *pars){
  usleep((int)(pars->time*1e6));
}

/********************************************************************/
/* get possible voltage ranges for a channel */
void
cmd_get_range(pico_pars_t *pars){

  int32_t len, i;
  const int32_t buflen=2048;
  int32_t buf[buflen];

  dev_open(pars);
  len=buflen;
  res=ps3000aGetChannelInformation(pars->h, PS3000A_CI_RANGES, 0,  buf, &len, pars->channel);
  if (res!=PICO_OK) print_err(res);
  else {
    printf("Ranges available for channel %d:", pars->channel);
    for (i=0; i<len; i++)  printf(" %s", range2str(buf[i]));
    printf(" [Vpp]\n");
  }
}


/********************************************************************/
#define MAXCH 2
/* data to the callback */
struct cb_in_t{
  int16_t *osc_buf[MAXCH];
  int16_t *prg_buf[MAXCH];
};
/* data from the callback */
struct cb_out_t{
  int16_t ready;
  int32_t count;
  int32_t start;
  int16_t overflow;
  int32_t trig_at;
  int16_t trig;
  int16_t autostop;
} cb_out;

/* callback used in the streaming mode */
void stream_func(int16_t h, int32_t count, uint32_t start, int16_t overflow,
          uint32_t trig_at, int16_t trig, int16_t autostop, void *par){
  struct cb_in_t *dat = (struct cb_in_t *) par;
  int ch;
  if ((par==NULL) || (count==0)) return;
  cb_out.ready    = 1;
  cb_out.count    = count;
  cb_out.start    = start;
  cb_out.overflow = overflow;
  cb_out.trig_at  = trig_at;
  cb_out.trig     = trig;
  cb_out.autostop = autostop;
  /*  printf("data: N=%d start=%d overflow=%d trig=%d trig_at=%d autostop=%d\n",
     count, start, overflow, trig, trig_at, autostop); */
  for (ch=0; ch<MAXCH; ch++){
    if (dat->osc_buf[ch] && dat->prg_buf[ch]){
      memcpy(dat->prg_buf[ch]+start,
             dat->osc_buf[ch]+start, count*sizeof(int16_t));
    }
  }
}

/* free buffers */
void
free_bufs(struct cb_in_t *buffers){
  int ch;
  for (ch=0; ch<MAXCH; ch++){
    if (buffers->osc_buf[ch]) free(buffers->osc_buf[ch]);
    if (buffers->prg_buf[ch]) free(buffers->prg_buf[ch]);
  }
}

/* record data to a file */
void
cmd_rec(pico_pars_t *pars){
  uint32_t interval, total, maxsamp, presamp;
  const int32_t buflen=2<<16;
  int i,ch;
  FILE *fo;

  struct cb_in_t buffers;

  dev_open(pars);
  /* set data buffers for each channel */
  for (ch=0; ch<MAXCH; ch++){
    buffers.osc_buf[ch] = (int16_t*)malloc(buflen*sizeof(int16_t));
    buffers.prg_buf[ch] = (int16_t*)malloc(buflen*sizeof(int16_t));
    res=ps3000aSetDataBuffer(pars->h, (PS3000A_CHANNEL)ch,
      buffers.osc_buf[ch], buflen, 0, PS3000A_RATIO_MODE_NONE);
    if (res!=PICO_OK) {print_err(res); free_bufs(&buffers); return;}
  }

  /* open a file */
  if (pars->file==NULL) pars->file="rec.dat";
  fo = fopen(pars->file, "w");
  if (fo==NULL) {
    printf("error: can not open file: %s\n", pars->file);
     free_bufs(&buffers); return;
  }

  /* start streaming mode */
  interval = (int)(1e9/pars->rate); /* ns */
  maxsamp  = (int)(pars->time * pars->rate);
  presamp  = (int)(pars->pretrig * pars->rate);
  res=ps3000aRunStreaming(pars->h, &interval, PS3000A_NS,
    presamp, 0, 0, 1, PS3000A_RATIO_MODE_NONE, buflen);

  /* on output interval containss real set interval - log it */
  if (pars->log) fprintf(pars->log, "rec::interval: %d [ns]\n", interval);

  if (res!=PICO_OK) { print_err(res); free_bufs(&buffers); fclose(fo); return; }

  fprintf(stderr, "start streaming mode, time: %f s,"
      " rate: %f Hz, interval: %.3e s, samples: %d + %d\n", pars->time, pars->rate, interval/1e9, presamp, maxsamp);

  total = 0;
  while (1){
    usleep(100);
    cb_out.ready=0;

    res=ps3000aGetStreamingLatestValues(pars->h, &stream_func, &buffers);
    if (cb_out.ready && cb_out.count == 0) break;
    if (cb_out.autostop) break;

    if (cb_out.ready && cb_out.count > 0){
      if (cb_out.trig){
        printf("Trigger at index %d\n", cb_out.trig_at+total);
        /* log the trigger position */
        if (pars->log) fprintf(pars->log, "rec::trig_at: %d [sample]\n", cb_out.trig_at+total);
        fprintf(fo, "\n");
      }
      for (i=0; i<cb_out.count; i++){
        if (total+i > maxsamp+presamp) break;
        for (ch=0; ch<MAXCH;ch++){
          if (buffers.prg_buf[ch])
            fprintf(fo, " %5d", buffers.prg_buf[ch][i+cb_out.start]);
        }
        fprintf(fo, "\n");
      }
      total += cb_out.count;
    }
    if (total > maxsamp+presamp) break;
    if (total > presamp && pars->trig_gen){ cmd_trig_gen(pars); pars->trig_gen=0;}
  }
  /* log the total number of samples */
  if (pars->log) fprintf(pars->log, "rec::total: %d [sample]\n", total);

 
  /* close file */
  fclose(fo);

  /* stop streaming mode */
  ps3000aStop(pars->h);
  if (res!=PICO_OK) print_err(res);
}

/********************************************************************/
/* interactive mode */

  /* convert a string into argc/argv 
     problems:
       - doesn't parse empty quotes '' or ""
       - needs fixed array for argv with size maxargs
  */
void
str2argv(char *line, int *argc, char *argv[], int maxargs){
  int i, n, q1=0, q2=0;
  /* put zeros between arguments */
  for (i=0; line[i]; i++){
    if (line[i]=='\"' && q2==0) { q1=(q1+1)%2; line[i]='\0'; }
    if (line[i]=='\'' && q1==0) { q2=(q2+1)%2; line[i]='\0'; }
    if ((line[i]==' ' || line[i]=='\t' || line[i]=='\n') && q1==0 && q2==0) line[i]='\0';
  }
  n=i;
  /* fill argc/argv */
  *argc=0;
  for(i=0;i<n;i++){
    if (line[i] && (i==0 || line[i-1]=='\0')){
      //fprintf(stderr, "<<%s>>\n", line+i);
      argv[(*argc)++] = line+i;
      if (*argc >=maxargs) break;
    }
  }
  argv[*argc]=0;
}

void
cmd_int(pico_pars_t *glob_pars){
  char *line;
  int argc;
  char *argv[512];
  pico_pars_t pars;

  printf("Interactive mode. Type 'list' for command list\n"
         " or '<command> -h' for command options\n");
  while(1){
    line=NULL;
    printf("> ");
    if (getline(&line,&argc,stdin)==-1 || !line || feof(stdin)) break;
    str2argv(line, &argc, argv, 512);
    /* commands - same as in main */
    while (argc>0) {
      char *cmd = argv[0];
      char **a = argv;
      pars = *glob_pars;
      get_pars(&argc, &a, cmd, &pars);
      run_cmd(cmd, &pars);
      glob_pars->h=pars.h; /* return device handle to global pars - to use later */
      glob_pars->log=pars.log;  /* return lof file handle to global pars - to use later */
    }
    if (line) free(line);
  }
}

/********************************************************************/
/* print command list */
void
cmd_list(pico_pars_t *glob_pars){
  int i;
  printf("\nCommands:\n");
  for (i=1; cmds[i].cmd; i++){ /* i=0 -- pico, not needed */
    printf("  %10s -- %s\n", cmds[i].cmd, cmds[i].text);
  }
}
/********************************************************************/
