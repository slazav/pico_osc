#ifndef PICO_H
#define PICO_H

#include <ps3000aApi.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**********************************************************/
/* server parameters */
typedef struct{
  int     port;  /* tcp port for connections */
  char   *dev;   /* pico device serial number */
  int16_t h;     /* device handle (0 if not opened yet) */
  int16_t verb;  /* print commands to stdout */
} pico_spars_t;

/* key - value pair to build headers */
#define MAXHVAL 128
typedef struct{
  const char *key;
  char val[MAXHVAL];
} str_pair_t;

/* output parameters */
#define MAXHEADS 32
typedef struct{
  int        dsize;    /* data size, bytes   */
  void       *data;    /* data for http body */
  const char *status;  /* status */
  str_pair_t headers[MAXHEADS]; /* (null,*)-terminated array of pairs */
} pico_opars_t;

/**********************************************************/
/* command parameters */
typedef struct{
  const char * command; /* command name */
#undef CPAR
#define CPAR(comm, type, par, short, cnv, def, descr) type comm ## _ ## par;
#include "pars.def"
} pico_cpars_t;

/**********************************************************/
/* commands (see commands.c) */

int dev_open(pico_spars_t *pars);  /* open device */
int dev_close(pico_spars_t *pars); /* close device */
int dev_list();                    /* list available devices */

void pico_command(pico_spars_t *spars, pico_cpars_t *cpars, pico_opars_t *opars);

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

double time2dbl(int64_t t, PS3000A_TIME_UNITS tu);

#endif