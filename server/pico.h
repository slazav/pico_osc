#ifndef PICO_H
#define PICO_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef ps3000a
  #include <ps3000aApi.h>
  #define psOpenUnit               ps3000aOpenUnit
  #define psCloseUnit              ps3000aCloseUnit
  #define psEnumerateUnits         ps3000aEnumerateUnits
  #define psPingUnit               ps3000aPingUnit
  #define psGetUnitInfo            ps3000aGetUnitInfo
  #define psSetChannel             ps3000aSetChannel
  #define psSetSimpleTrigger       ps3000aSetSimpleTrigger
  #define psSetSigGenBuiltInV2     ps3000aSetSigGenBuiltInV2
  #define psSigGenSoftwareControl  ps3000aSigGenSoftwareControl
  #define psGetTimebase2           ps3000aGetTimebase2
  #define psSetDataBuffer          ps3000aSetDataBuffer
  #define psRunBlock               ps3000aRunBlock
  #define psSigGenSoftwareControl  ps3000aSigGenSoftwareControl
  #define psIsReady                ps3000aIsReady
  #define psGetValues              ps3000aGetValues
  #define psGetTriggerTimeOffset64 ps3000aGetTriggerTimeOffset64
  #define psStop                   ps3000aStop
  #define PS_TIME_UNITS            PS3000A_TIME_UNITS
  #define PS_ES_OFF                PS3000A_ES_OFF
  #define PS_AC                    PS3000A_AC
  #define PS_DC                    PS3000A_DC
  #define PS_SIGGEN_GATE_HIGH      PS3000A_SIGGEN_GATE_HIGH
#endif
#ifdef ps4000
  #include <ps4000Api.h>
  #define psOpenUnit               ps4000OpenUnit
  #define psCloseUnit              ps4000CloseUnit
  #define psEnumerateUnits         ps4000EnumerateUnits
  #define psPingUnit               ps4000PingUnit
  #define psGetUnitInfo            ps4000GetUnitInfo
  #define psSetChannel             ps4000SetChannel
  #define psSetSimpleTrigger       ps4000SetSimpleTrigger
  #define psSetSigGenBuiltInV2     ps4000SetSigGenBuiltInV2
  #define psSigGenSoftwareControl  ps4000SigGenSoftwareControl
  #define psGetTimebase2           ps4000GetTimebase2
  #define psSetDataBuffer          ps4000SetDataBuffer
  #define psRunBlock               ps4000RunBlock
  #define psSigGenSoftwareControl  ps4000SigGenSoftwareControl
  #define psIsReady                ps4000IsReady
  #define psGetValues              ps4000GetValues
  #define psGetTriggerTimeOffset64 ps4000GetTriggerTimeOffset64
  #define psStop                   ps4000Stop
  #define PS_TIME_UNITS            PS4000_TIME_UNITS
  #define PS_ES_OFF                PS4000_ES_OFF
  #define PS_AC                    PS4000_AC
  #define PS_DC                    PS4000_DC
  #define PS_SIGGEN_GATE_HIGH      PS4000_SIGGEN_GATE_HIGH
#endif



#define MAXCH 2

/**********************************************************/
/* server parameters */
typedef struct{
  int     port;  /* tcp port for connections */
  char   *dev;   /* pico device serial number */
  int16_t h;     /* device handle (0 if not opened yet) */
  int16_t verb;  /* print commands to stdout */
} spars_t;

/* key - value pair to build headers */
#define MAXHVAL 128
typedef struct{
  const char *key;
  char val[MAXHVAL];
} str_pair_t;

/* add info into str_pair_t array */
#define print_kv(ARR, KEY, ...)\
  { str_pair_t *hh;\
    for (hh=ARR; hh->key && hh-ARR < MAXHEADS-2; hh++); \
    hh->key=KEY; snprintf(hh->val, MAXHVAL, __VA_ARGS__); \
    hh++; hh->key=NULL; }

/* output parameters */
#define MAXHEADS 32
typedef struct{
  int        dsize;    /* data size, bytes   */
  void       *data;    /* data for http body */
  const char *status;  /* status */
  str_pair_t headers[MAXHEADS]; /* (null,*)-terminated array of pairs */
} opars_t;

/**********************************************************/
/* command parameters */
typedef struct{
  const char * command; /* command name */
#undef CPAR
#define CPAR(comm, type, par, short, cnv, def, descr) type comm ## _ ## par;
#include "pars.def"
} cpars_t;

/**********************************************************/
/* commands (see commands.c) */

int dev_open(spars_t *pars);  /* open device */
int dev_close(spars_t *pars); /* close device */
int dev_list();                    /* list available devices */

void pico_command(spars_t *spars, cpars_t *cpars, opars_t *opars);

/**********************************************************/
/* conversions (see convs.c) */

int16_t str2bool(const char *str);
int16_t str2coupling(const char *str);
int16_t str2chan(const char *str);
int16_t str2range(const char *str);
int16_t str2dir(const char *str);
int16_t str2wave(const char *str);
int16_t str2sweep(const char *str);
int32_t str2gen_volt(const char *str);
int16_t str2gen_trigsrc(const char *str);
int16_t str2gen_trigdir(const char *str);
int16_t str2tunits(const char *str);

const char * pico_err(int n);
const char * coupling2str(int16_t n);
const char * chan2str(int16_t n);
const char * range2str(int16_t n);
const char * dir2str(int16_t n);
const char * wave2str(int16_t n);
const char * sweep2str(int16_t n);
const char * gen_trigsrc2str(int16_t n);
const char * gen_trigdir2str(int16_t n);
const char * tunits2str(int16_t n);

double time2dbl(int64_t t, PS_TIME_UNITS tu);

/* see rec.c */
int do_rec_block(spars_t *spars, cpars_t *cpars, opars_t *opars,
      int16_t *bufs1, int16_t *bufs2, double *bufd1, double *bufd2);

#endif
