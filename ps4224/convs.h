#ifndef CONVS_H
#define CONVS_H

#include <pico/ps4000Api.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAXCH 2

/**********************************************************/
/* commands (see commands.c) */

//int dev_open(spars_t *pars);  /* open device */
//int dev_close(spars_t *pars); /* close device */
//int dev_list();                    /* list available devices */
//void pico_command(spars_t *spars, cpars_t *cpars, opars_t *opars);

/**********************************************************/
/* conversions (see convs.c) */

int16_t str2bool(const char *str);
int16_t str2coupl(const char *str);
PS4000_CHANNEL      str2chan(const char *str);
PS4000_RANGE        volt2range(float v);
THRESHOLD_DIRECTION str2dir(const char *str);
int16_t str2wave(const char *str);
int16_t str2sweep(const char *str);
int32_t str2gen_volt(const char *str);
int16_t str2gen_trigsrc(const char *str);
int16_t str2gen_trigdir(const char *str);
PS4000_TIME_UNITS str2tunits(const char *str);

const char * pico_err(int n);
const char * coupl2str(int16_t n);
const char * chan2str(PS4000_CHANNEL n);
const char * range2str(PS4000_RANGE n);
const char * dir2str(THRESHOLD_DIRECTION n);
const char * wave2str(int16_t n);
const char * sweep2str(int16_t n);
const char * gen_trigsrc2str(int16_t n);
const char * gen_trigdir2str(int16_t n);
const char * tunits2str(PS4000_TIME_UNITS n);

double time2dbl(int64_t t, PS4000_TIME_UNITS tu);



#endif
