#include <math.h>
#include <unistd.h>
#include "pico.h"

/* Non-trivial conversion from rate to pico timebase
 * input:  device handler, desired dt (s)
 * output: actual dt (s), pico timebase, result
 */
uint32_t rec_timebase(uint16_t h, double * dt, uint32_t * tbase){
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
