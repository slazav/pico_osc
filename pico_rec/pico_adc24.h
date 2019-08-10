#ifndef PICO_ADC24_H
#define PICO_ADC24_H

#include <pico/HRDL.h>
#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cstring>  // strcasecmp
#include <cmath>
#include "err/err.h"
#include "pico_err.h"
#include "picoadc_int.h"



class ADC24 : public ADCInt {

public:
  ADC24(){}
  ~ADC24(){}

};

#endif


