#ifndef DEVICE_ADC24_H
#define DEVICE_ADC24_H

#include <pico/HRDL.h>
#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cstring>  // strcasecmp
#include <cmath>
#include "err/err.h"

#include "device.h"
#include "ADC24_const.h"

/// Low-level commands for the device interface

class ADC24 : public ADCInt {
  int16_t devh; // device handle
  int16_t devn; // device number (1..20)

public:

  // return newline-separated list of all connected devices
  // See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
  static std::string dev_list(){
    std::string ret;
    int16_t devices[HRDL_MAX_PICO_UNITS];
    int8_t line[80];

    for (int i = 0; i < HRDL_MAX_UNITS; i++) {
      devices[i] = HRDLOpenUnit();
      if (devices[i] > 0) {
        HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_BATCH_AND_SERIAL);
        ret += std::string((char*)line) + "\n";
      }
      else {
        HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_ERROR);
        if (atoi((char*)line) != HRDL_NOT_FOUND)
          throw Err() << "can't open device " << i << ":" << line;
      }
    }
    // close devices
    for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
      if (devices[i] > 0) HRDLCloseUnit(devices[i]);
    }
    return ret;
  }


  // Constructor and destructor: open/close device, throw errors if any.
  // See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
  // for their strange way of finding correct device...
  ADC24(const char *name, int mainsHz = 50){


    // open all devices, as in dev_list()
    int16_t devices[HRDL_MAX_PICO_UNITS];
    int8_t line[80];
    devh=-1; devn=-1;

    // initialize device array
    for (int i = 0; i < HRDL_MAX_UNITS; i++) devices[i] = 0;

    // open all devices, as in dev_list(), find which one to use
    for (int i = 0; i < HRDL_MAX_UNITS; i++) {
      devices[i] = HRDLOpenUnit();
      if (devices[i] > 0) {
        HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_BATCH_AND_SERIAL);
        if (name==0 || name[0] == 0 || strcasecmp(name, (char*)line)==0){
          devh = devices[i]; devn = i; break;
        }
      }
    }

    // close all devices except one
    for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
      if (devices[i] > 0 && i!=devn) HRDLCloseUnit(devices[i]);
    }

    if (devn == -1){
      if (name==0 || name[0] == 0) throw Err() << "No PicoLog devices found";
      else throw Err() << "PicoLog device not found: " << name;
    }

    // configure the mains noise rejection
    sixtyHz = (mainsHz == 60);
    set_mains();
    chN=HRDL_MAX_ANALOG_CHANNELS;
    chconf_en.resize(chN, 0);
    chconf_sngl.resize(chN, 0);
    chconf_rng.resize(chN, 0);
    chconf_max.resize(chN, 0);
  }

  ~ADC24(){}


  // get avaiable ranges: chan_get_ranges("A");
  std::string chan_get_ranges(const char * chan){
    std::string ret;
    for (int i = 0; i != HRDL_MAX_RANGES; ++i) {
      if (i>0) ret+=" ";
      ret+=range2str((HRDL_RANGE)i);
    }
    return ret;
  }

  // get avaiable time conversion conctants
  std::string chan_get_tconvs() {
    std::string ret;
    for (int i = 0; i != HRDL_MAX_CONVERSION_TIMES; ++i) {
      if (i>0) ret+=" ";
      ret+=tconv2str((HRDL_CONVERSION_TIME)i);
    }
    return ret;
  }

  void chan_set(int16_t chan, bool enable, bool sngl, float rng) {
    if (!(HRDLSetAnalogInChannel(devh,chan,enable,volt2range(rng),sngl))) throw Err()
      << "failed to set channel " << chan;
  }

  int16_t chan_get_num() {
    int16_t n;
    if (HRDLGetNumberOfEnabledChannels(devh, &n)) return n;
    throw Err() << "chan_get_num() failed: invalid handle";
  }

  const char * get_unit_id() {
    int16_t len=20;
    const char *str;
    str = (char*)malloc(sizeof(char)*20);
    HRDLGetUnitInfo(devh,(int8_t *)str,len,HRDL_BATCH_AND_SERIAL);
    return str;
  }

  // returns settings error
  const char * get_err() {
    return NULL;
  };

  // configures the mains noise rejection setting
  void set_mains() {
    if (!HRDLSetMains(devh,sixtyHz)) throw Err() << "failed to set mains";
  }

  // sets the sampling time interval
  void set_interval(int32_t dt, int16_t conv) {
    if (!HRDLSetInterval(devh,dt,tconvi2convtime(conv))) throw Err() << "failed to set time interval";
  };

  // run block mode
  void run_block(int32_t nvals) {
    if (!HRDLRun(devh,nvals,str2m("block")))
      throw Err() << "failed to run block mode";
  }

  // is device ready?
  bool is_ready() { return HRDLReady(devh); }

  // get the requested number of samples for each enabled channel
  std::vector<float> get_values(int32_t nvals, int16_t *overflow) {
    int16_t nch = chan_get_num();
    std::vector<int32_t> dvals(nch*nvals);
    std::vector<float>   vals(nvals);

    // returns actual number of samples
    int32_t resn = HRDLGetValues(devh,dvals.data(),overflow,nvals);
    float rngs[chN], maxcounts[chN];
    int rs_n=0;
    for (int ch = 0; ch<chN; ++ch) {
      if (chconf_en[ch]) {
        rngs[rs_n]=chconf_rng[ch];
        maxcounts[rs_n]=chconf_max[ch];
        rs_n++;
      }
    }
    int32_t v=0;
    for (int n=0; n<resn; ++n) {
      for (int ri=0; ri<rs_n; ++ri ) {
        vals[v]=rngs[ri]*dvals[v]/maxcounts[ri]*0.001; // Volts
        v++;
      }
    }
    return vals;
  }

  // get the maximum and minimum ADC count available for the device
  int32_t get_max(int16_t ch) {
    int32_t min,max;
    if (!HRDLGetMinMaxAdcCounts(devh,&min,&max,ch)) throw Err() << "failed to get max ADC count";
    return max;
  }

  // get device info
  std::string get_info(){
    const int16_t ui_buflen=1024;
    char ui_v[ui_buflen],ui_n[ui_buflen];
    if (!HRDLGetUnitInfo(devh,(int8_t *)ui_v,ui_buflen, HRDL_VARIANT_INFO) ||
        !HRDLGetUnitInfo(devh,(int8_t *)ui_n,ui_buflen, HRDL_BATCH_AND_SERIAL))
      throw Err() << "failed to get unit info";
    return std::string(ui_v)+" "+std::string(ui_n);
  }
};

#endif


