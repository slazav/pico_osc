/* option handling */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

/**********************************************************/
/* structure with all parameters */
typedef struct{
  char   *dev;
  int16_t h;     /* device handle (0 if not opened yet) */
  int16_t help;
  FILE   *log;
    /* chan */
  int16_t channel;
  int16_t enable;
  int16_t coupling;
  int16_t range;
  float   offset;
    /* trig */
  int16_t src;      /* trigger source*/
  int16_t thresh;   /* trigger threshold */
  int16_t dir;      /* trigger direction */
  uint32_t delay;   /* trigger delay, samples */
  int16_t autotrig; /* autotrigger delay, ms*/
    /* gen */
  int32_t  gen_offs;
  uint32_t pk2pk;
  uint16_t wave;
  double   startf,stopf,incr,dwell;
  uint16_t sweep;
  uint32_t cycles,sweeps;
  uint16_t trig_gen_src, trig_gen_dir, trig_gen_thr;
    /* rec */
  double time, rate, pretrig;
  char *file;
  int trig_gen;
} pico_pars_t;

/**********************************************************/
void
par_ini(pico_pars_t *pars){
  pars->dev      = NULL;
  pars->h        = 0;
  pars->help     = 0;
  pars->log      = NULL;
    /* chan */
  pars->channel  = PS3000A_CHANNEL_A;
  pars->enable   = 1;
  pars->coupling = PS3000A_AC;
  pars->range    = PS3000A_50V;
  pars->offset   = 0.0;
    /* trig */
  pars->src      = PS3000A_EXTERNAL;
  pars->thresh   = 0;
  pars->dir      = PS3000A_RISING;
  pars->delay    = 0;
  pars->autotrig = 0;
    /* gen */
  pars->gen_offs = 0;
  pars->pk2pk    = 1000;
  pars->wave     = PS3000A_SINE;
  pars->startf   = 1000;
  pars->stopf    = 1000;
  pars->incr     = 0;
  pars->dwell    = 0;
  pars->sweep    = PS3000A_UP;
  pars->cycles   = 0;
  pars->sweeps   = 0;
  pars->trig_gen_src = PS3000A_SIGGEN_NONE;
  pars->trig_gen_dir = PS3000A_SIGGEN_RISING;
  pars->trig_gen_thr = 0;
    /* rec */
  pars->time     = 1.0;
  pars->rate     = 1000;
  pars->pretrig  = 0;
  pars->file     = NULL;
  pars->trig_gen = 0;
}

/**********************************************************/
void
par_set(pico_pars_t *pars, const char *name, char *val){
       if (strcmp(name, "help")==0)     pars->help     = 1;
  else if (strcmp(name, "dev")==0)      pars->dev      = val;
    /* chan */
  else if (strcmp(name, "channel")==0)  pars->channel  = str2chan(val);
  else if (strcmp(name, "coupling")==0) pars->coupling = str2coupling(val);
  else if (strcmp(name, "enable")==0)   pars->enable   = 1;
  else if (strcmp(name, "disable")==0)  pars->enable   = 0;
  else if (strcmp(name, "range")==0)    pars->range    = str2range(val);
  else if (strcmp(name, "offset")==0)   pars->offset   = atof(val);
    /* trig */
  else if (strcmp(name, "src")==0)      pars->src      = str2chan(val);
  else if (strcmp(name, "thresh")==0)   pars->thresh   = atoi(val);
  else if (strcmp(name, "dir")==0)      pars->dir      = str2dir(val);
  else if (strcmp(name, "delay")==0)    pars->delay    = atoi(val);
  else if (strcmp(name, "autotrig")==0) pars->autotrig = atoi(val);
    /* gen */
  else if (strcmp(name, "gen_offs") ==0)     pars->gen_offs = atoi(val);
  else if (strcmp(name, "pk2pk") ==0)        pars->pk2pk    = (int)(atof(val)*1e6);
  else if (strcmp(name, "wave") ==0)         pars->wave     = str2wave(val);
  else if (strcmp(name, "startf") ==0)       pars->startf   = atof(val);
  else if (strcmp(name, "stopf") ==0)        pars->stopf    = atof(val);
  else if (strcmp(name, "incr") ==0)         pars->incr     = atof(val);
  else if (strcmp(name, "dwell") ==0)        pars->dwell    = atof(val);
  else if (strcmp(name, "sweep") ==0)        pars->sweep    = str2sweep(val);
  else if (strcmp(name, "cycles") ==0)       pars->cycles   = atoi(val);
  else if (strcmp(name, "sweeps") ==0)       pars->sweeps   = atoi(val);
  else if (strcmp(name, "trig_gen_src") ==0) pars->trig_gen_src = str2siggen_src(val);
  else if (strcmp(name, "trig_gen_dir") ==0) pars->trig_gen_dir = str2siggen_dir(val);
  else if (strcmp(name, "trig_gen_thr") ==0) pars->trig_gen_thr = atoi(val);
    /* rec */
  else if (strcmp(name, "time") ==0)     pars->time    = atof(val);
  else if (strcmp(name, "rate") ==0)     pars->rate    = atof(val);
  else if (strcmp(name, "pretrig") ==0)  pars->pretrig = atof(val);
  else if (strcmp(name, "file") ==0)     pars->file    = val;
  else if (strcmp(name, "trig_gen") ==0) pars->trig_gen = 1;
  else printf("unknown option: %s\n", name);
}

/**********************************************************/
/* My own structure for all options - with description */

struct {
  const char *cmd;
  const char *name;     /* as in struct option */
  int         val;
  int         has_arg;
  char       *descr;
}
opts[] = {
 /* command     option    short Npars description */
  {NULL,        "help",     'h', 0, "print a help message"}, /* all commands! */
  {"pico",      "dev",      'd', 1, "select device by serial number (default - use the first device)"},

  {"chan",      "channel",  'c', 1, "select channel number: A,B (default - A)"},
  {"chan",      "coupling", 'i', 1, "select input coupling, AC or DC (default - AC)"},
  {"chan",      "enable",   'e', 0, "enable channel (default)"},
  {"chan",      "disable",  'd', 0, "disable channel"},
  {"chan",      "range",    'r', 1, "set sensitivity, Vpp (default 100)"},
  {"chan",      "offset",   'o', 1, "set offset, V (default 0)"},

  {"get_range", "channel",  'c', 1, "select channel number (default - 0)"},

  {"trig",      "enable",   'e', 0, "enable trigger (default)"},
  {"trig",      "disable",  'd', 0, "disable trigger"},
  {"trig",      "src",      's', 1, "source channel: A,B,EXT (default EXT)"},
  {"trig",      "thresh",   't', 1, "threshold (default 0)"},
  {"trig",      "dir",      'D', 1, "direction: above,below,rising,falling,rising_or_falling (default rising)"},
  {"trig",      "del",      'w', 1, "delay, samples (default 0)"},
  {"trig",      "autotrig", 'a', 1, "autotrigger after this time, ms (0 - no autotrigger, default)"},

  {"gen",       "gen_offs", 'o', 1, "offset (default 0)"},
  {"gen",       "pk2pk",    'u', 1, "pk2pk voltage, V (default 0.001)"},
  {"gen",       "wave",     'w', 1, "wave type: sine,squere,triangle (default sine)"},
  {"gen",       "startf",   'f', 1, "start frequency, Hz (default 1000)"},
  {"gen",       "stopf",    'g', 1, "stop frequency, Hz (default 1000)"},
  {"gen",       "incr",     'i', 1, "increment, Hz (default 0)"},
  {"gen",       "dwell",    'd', 1, "dwell time (default 0)"},
  {"gen",       "sweep",    's', 1, "sweep type: up,down,updown,downup (default up)"},
  {"gen",       "cycles",   'C', 1, "cycles (default 0)"},
  {"gen",       "sweeps",   'S', 1, "sweeps (default 0 - no sweeps)"},
  {"gen",       "trig_gen_src", 'T', 1, "generator trigger source: none,scope,ext,soft (default none)"},
  {"gen",       "trig_gen_dir", 'D', 1, "generator trigger dir: rising,falling,high,low (default rising)"},
  {"gen",       "trig_gen_thr", 't', 1, "generator trigger threshold (default 0)"},

  {"rec",       "time",     't', 1, "record time,s (default 1)"},
  {"rec",       "rate",     'r', 1, "record rate, Hz (default 1000)"},
  {"rec",       "pretrig",  'p', 1, "record pretrig time, s (default 0)"},
  {"rec",       "file",     'f', 1, "record filename (default: rec.dat)"},
  {"rec",       "trig_gen", 'T', 0, "trigger the generator (default off)"},

  {"wait",      "time",     't', 1, "waiting time,s (default 1)"},

  {"log",       "file",     'f', 1, "log file name (default: rec.log)"},

  {NULL, NULL, 0,0, NULL}
};

typedef void (cmd_t)(pico_pars_t*);

cmd_t cmd_find, cmd_ping, cmd_info, cmd_chan, cmd_get_range,
      cmd_trig, cmd_gen, cmd_rec, cmd_trig_gen, cmd_wait, cmd_log,
      cmd_int, cmd_list;

struct {
  const char *cmd;
  cmd_t *func;
  const char *text;
}
cmds[] = {
  /* cmd        description */
  {"pico",NULL, "a program for manipulation picoscope devices\n"
                "Usage: pico [global options] cmd [cmd options] ...\n"},
  {"find",      &cmd_find,      "find all pico devices"},
  {"ping",      &cmd_ping,      "ping the device"},
  {"info",      &cmd_info,      "print device information"},
  {"chan",      &cmd_chan,      "set channel input coupling, sensetivity, offset"},
  {"get_range", &cmd_get_range, "get available sensitivity settings for the channel"},
  {"trig",      &cmd_trig,      "set trigger"},
  {"gen",       &cmd_gen,       "set generator"},
  {"rec",       &cmd_rec,       "record a signal"},
  {"trig_gen",  &cmd_trig_gen,  "trigger the generator"},
  {"wait",      &cmd_wait,      "wait for some time"},
  {"log",       &cmd_log,       "open log file"},
  {"int",       &cmd_int,       "interactive mode"},
  {"list",      &cmd_list,      "print command list"},
  {NULL,NULL,NULL}
};

/**********************************************************/
/* run the command or print a help message with all options */
void
run_cmd(const char *cmd, pico_pars_t *pars){
  int i;

  if (pars->help){
    /* print help string */
    for (i=0; cmds[i].cmd; i++){
      if (strcmp(cmds[i].cmd, cmd)==0){
        printf("%s -- %s\n", cmds[i].cmd, cmds[i].text);
        break;
      }
    }
    /* print options */
    printf("Options:\n");
    for (i=0; opts[i].name; i++){
      if (!opts[i].cmd || strcmp(opts[i].cmd, cmd)==0)
        printf("  -%c, --%s %s -- %s\n",
          opts[i].val, opts[i].name, opts[i].has_arg?"<arg>":"", opts[i].descr);
    }
    /* print commands */
    if (strcmp(cmd, "pico")==0) cmd_list(NULL);
    printf("\n");
    return;
  }

  /* run command */
  for (i=0; cmds[i].cmd; i++){
    if (strcmp(cmds[i].cmd, cmd)==0){
      if (cmds[i].func) cmds[i].func(pars);
      return;
    }
  }
  printf("error: unknown command: %s\n", cmd);
}

/**********************************************************/
/* read command line arguments and fill pico_pars structure */
void
get_pars(int *argc, char ***argv, const char *cmd, pico_pars_t *pars){

  struct option *longopts;
  char *optstring;
  int index=0, optlen=0;

  if (!opts) {
    printf("get_pars error: NULL opts array\n");
    *argc--;
    *argv++;
    return;
  }
  /* get option array length */
  while(opts[optlen].name){ optlen++; }

  /* allocate memory for getopt_long data*/
  longopts = (struct option *)malloc(sizeof(struct option)*optlen);
  if (!longopts){
    printf("get_pars error: can't malloc longopts\n");
    exit(1);
  }
  optstring = (char *)malloc(3*optlen+2); /* +o::\n */
  if (!optstring){
    printf("get_pars error: can't malloc optstring\n");
    free(longopts);
    exit(1);
  }

  /* Prepare data for getopt_long(3). */
  {
    int i0 = 0; /* option index */
    int i = 0; /* longopt index */
    int j = 0; /* optstring index */
    optstring[j++] = '+'; /* stop at the first non-option argument */
    while (opts[i0].name){
      if (opts[i0].cmd && strcmp(opts[i0].cmd, cmd)!=0){ i0++; continue;}
      if (opts[i0].val != 0){ optstring[j++]=opts[i0].val;
        if (opts[i0].has_arg>0)  optstring[j++]=':';
        if (opts[i0].has_arg>1)  optstring[j++]=':'; /* : or :: */
      }
      longopts[i].name    = opts[i0].name;
      longopts[i].has_arg = opts[i0].has_arg;
      longopts[i].flag    = NULL;
      longopts[i].val     = opts[i0].val;
      i++; i0++;
    }
    longopts[i].name = NULL;
  }

  /* parse  options */
  while(1){ /* options */
    int c;
    index = 0;
    c = getopt_long(*argc, *argv, optstring, longopts, &index);
    if (c == -1) break; /* end*/
    if ((c == '?') || (c == ':'))  continue; /* error msg is printed by getopt_long*/

    if (c!=0){ /* short option -- we must manually set index */
      int i = 0;
      while (longopts[i].name){
        if (longopts[i].val == c) index = i;
        i++;
      }
    }
    if (!longopts[index].name){
      fprintf(stderr, "Error: bad option");
      continue;
    }
    par_set(pars, longopts[index].name, optarg);
  }
  free(longopts);
  free(optstring);

  *argc-=optind;
  *argv+=optind;
  optind=0;
}
