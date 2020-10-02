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

/// Low-level commands for the device interface

class ADC24 : public ADCInt {
  int16_t devh; // device handle
  int16_t devn; // device number (1..20)

  // convert ms to conversion time
  HRDL_CONVERSION_TIME tconvi2convtime(int16_t t){
    switch (t){
      case  60: return HRDL_60MS;
      case 100: return HRDL_100MS;
      case 180: return HRDL_180MS;
      case 340: return HRDL_340MS;
      case 660: return HRDL_660MS;
    }
    throw Err() << "error: unknown conversion time: " << t;
  }

  const char * tconv2str(HRDL_CONVERSION_TIME tconv){
    switch (tconv){
      case  HRDL_60MS: return "60";
      case HRDL_100MS: return "100";
      case HRDL_180MS: return "180";
      case HRDL_340MS: return "340";
      case HRDL_660MS: return "660";
    }
    throw Err() << "error: unknown conversion time: " << tconv;
  }

  HRDL_DIGITAL_IO_CHANNEL int2dch(int ch){
    switch (ch){
      case 1: return HRDL_DIGITAL_IO_CHANNEL_1;
      case 2: return HRDL_DIGITAL_IO_CHANNEL_2;
      case 3: return HRDL_DIGITAL_IO_CHANNEL_3;
      case 4: return HRDL_DIGITAL_IO_CHANNEL_4;
    }
    throw Err() << "error: unknown digital channel: " << ch;
  }

  int dch2int(HRDL_DIGITAL_IO_CHANNEL ch){
    switch (ch){
      case HRDL_DIGITAL_IO_CHANNEL_1: return 1;
      case HRDL_DIGITAL_IO_CHANNEL_2: return 2;
      case HRDL_DIGITAL_IO_CHANNEL_3: return 3;
      case HRDL_DIGITAL_IO_CHANNEL_4: return 4;
    }
    throw Err() << "error: unknown digital channel: " << ch;
  }

  HRDL_INPUTS int2ach(int ch){
    switch (ch){
      case  1: return HRDL_ANALOG_IN_CHANNEL_1;
      case  2: return HRDL_ANALOG_IN_CHANNEL_2;
      case  3: return HRDL_ANALOG_IN_CHANNEL_3;
      case  4: return HRDL_ANALOG_IN_CHANNEL_4;
      case  5: return HRDL_ANALOG_IN_CHANNEL_5;
      case  6: return HRDL_ANALOG_IN_CHANNEL_6;
      case  7: return HRDL_ANALOG_IN_CHANNEL_7;
      case  8: return HRDL_ANALOG_IN_CHANNEL_8;
      case  9: return HRDL_ANALOG_IN_CHANNEL_9;
      case 10: return HRDL_ANALOG_IN_CHANNEL_10;
      case 11: return HRDL_ANALOG_IN_CHANNEL_11;
      case 12: return HRDL_ANALOG_IN_CHANNEL_12;
      case 13: return HRDL_ANALOG_IN_CHANNEL_13;
      case 14: return HRDL_ANALOG_IN_CHANNEL_14;
      case 15: return HRDL_ANALOG_IN_CHANNEL_15;
      case 16: return HRDL_ANALOG_IN_CHANNEL_16;
    }
    throw Err() << "error: unknown analog channel: " << ch;
  }

  int ach2int(HRDL_INPUTS ch){
    switch (ch){
      case  HRDL_ANALOG_IN_CHANNEL_1: return 1;
      case  HRDL_ANALOG_IN_CHANNEL_2: return 2;
      case  HRDL_ANALOG_IN_CHANNEL_3: return 3;
      case  HRDL_ANALOG_IN_CHANNEL_4: return 4;
      case  HRDL_ANALOG_IN_CHANNEL_5: return 5;
      case  HRDL_ANALOG_IN_CHANNEL_6: return 6;
      case  HRDL_ANALOG_IN_CHANNEL_7: return 7;
      case  HRDL_ANALOG_IN_CHANNEL_8: return 8;
      case  HRDL_ANALOG_IN_CHANNEL_9: return 9;
      case HRDL_ANALOG_IN_CHANNEL_10: return 10;
      case HRDL_ANALOG_IN_CHANNEL_11: return 11;
      case HRDL_ANALOG_IN_CHANNEL_12: return 12;
      case HRDL_ANALOG_IN_CHANNEL_13: return 13;
      case HRDL_ANALOG_IN_CHANNEL_14: return 14;
      case HRDL_ANALOG_IN_CHANNEL_15: return 15;
      case HRDL_ANALOG_IN_CHANNEL_16: return 16;
    }
    throw Err() << "error: unknown analog channel: " << ch;
  }

  HRDL_INPUTS str2ach(const char *str){
    if (strcasecmp(str,"01")==0) return HRDL_ANALOG_IN_CHANNEL_1;
    if (strcasecmp(str,"02")==0) return HRDL_ANALOG_IN_CHANNEL_2;
    if (strcasecmp(str,"03")==0) return HRDL_ANALOG_IN_CHANNEL_3;
    if (strcasecmp(str,"04")==0) return HRDL_ANALOG_IN_CHANNEL_4;
    if (strcasecmp(str,"05")==0) return HRDL_ANALOG_IN_CHANNEL_5;
    if (strcasecmp(str,"06")==0) return HRDL_ANALOG_IN_CHANNEL_6;
    if (strcasecmp(str,"07")==0) return HRDL_ANALOG_IN_CHANNEL_7;
    if (strcasecmp(str,"08")==0) return HRDL_ANALOG_IN_CHANNEL_8;
    if (strcasecmp(str,"09")==0) return HRDL_ANALOG_IN_CHANNEL_9;
    if (strcasecmp(str,"10")==0) return HRDL_ANALOG_IN_CHANNEL_10;
    if (strcasecmp(str,"11")==0) return HRDL_ANALOG_IN_CHANNEL_11;
    if (strcasecmp(str,"12")==0) return HRDL_ANALOG_IN_CHANNEL_12;
    if (strcasecmp(str,"13")==0) return HRDL_ANALOG_IN_CHANNEL_13;
    if (strcasecmp(str,"14")==0) return HRDL_ANALOG_IN_CHANNEL_14;
    if (strcasecmp(str,"15")==0) return HRDL_ANALOG_IN_CHANNEL_15;
    if (strcasecmp(str,"16")==0) return HRDL_ANALOG_IN_CHANNEL_16;
    throw Err() << "error: unknown analog channel: " << str;
  }

  const char * ach2str(HRDL_INPUTS ch){
    switch (ch){
      case  HRDL_ANALOG_IN_CHANNEL_1: return "01";
      case  HRDL_ANALOG_IN_CHANNEL_2: return "02";
      case  HRDL_ANALOG_IN_CHANNEL_3: return "03";
      case  HRDL_ANALOG_IN_CHANNEL_4: return "04";
      case  HRDL_ANALOG_IN_CHANNEL_5: return "05";
      case  HRDL_ANALOG_IN_CHANNEL_6: return "06";
      case  HRDL_ANALOG_IN_CHANNEL_7: return "07";
      case  HRDL_ANALOG_IN_CHANNEL_8: return "08";
      case  HRDL_ANALOG_IN_CHANNEL_9: return "09";
      case HRDL_ANALOG_IN_CHANNEL_10: return "10";
      case HRDL_ANALOG_IN_CHANNEL_11: return "11";
      case HRDL_ANALOG_IN_CHANNEL_12: return "12";
      case HRDL_ANALOG_IN_CHANNEL_13: return "13";
      case HRDL_ANALOG_IN_CHANNEL_14: return "14";
      case HRDL_ANALOG_IN_CHANNEL_15: return "15";
      case HRDL_ANALOG_IN_CHANNEL_16: return "16";
    }
    throw Err() << "error: unknown analog channel: " << ch;
  }

  // convert str to block method
  HRDL_BLOCK_METHOD str2m(const char *str){
    if (strcasecmp(str,"block")==0)  return HRDL_BM_BLOCK;
    if (strcasecmp(str,"window")==0) return HRDL_BM_WINDOW;
    if (strcasecmp(str,"stream")==0) return HRDL_BM_STREAM;
    throw Err() << "error: unknown block method: " << str;
  }

  // convert voltage range
  HRDL_RANGE volt2range(float v){
    int range = (int)(10000*v);
    switch (range){
      case 25000000: return HRDL_2500_MV; // 2500
      case 12500000: return HRDL_1250_MV; // 1250
      case  6250000: return HRDL_625_MV;  // 625
      case  3125000: return HRDL_313_MV;  // 312.5
      case  1562500: return HRDL_156_MV;  // 156.25
      case   781250: return HRDL_78_MV;   // 78.125
      case   390625: return HRDL_39_MV;   // 39.0625
    }
    throw Err() << "error: unknown input range: " << v;
  }

  const char * range2str(HRDL_RANGE r){
     switch (r){
      case HRDL_2500_MV: return "2500";
      case HRDL_1250_MV: return "1250";
      case  HRDL_625_MV: return "625";
      case  HRDL_313_MV: return "312.5";
      case  HRDL_156_MV: return "156.25";
      case   HRDL_78_MV: return "78.125";
      case   HRDL_39_MV: return "39.0625";
    }
    throw Err() << "error: unknown input range: " << r;
  }

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
    chconf_en = (bool*)malloc(sizeof(bool)*chN);
    chconf_sngl = (bool*)malloc(sizeof(bool)*chN);
    chconf_rng = (float*)malloc(sizeof(float)*chN);
    chconf_max = (float*)malloc(sizeof(float)*chN);
    for (int i=0; i<chN; ++i) {
      chconf_en[i]=0;
      chconf_sngl[i]=0;
      chconf_rng[i]=0;
      chconf_max[i]=0;
    }
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
    if (!HRDLRun(devh,nvals,str2m("block"))) throw Err() << "failed to run block mode";
  }

  // is device ready?
  bool is_ready() { return HRDLReady(devh); }

  // get the requested number of samples for each enabled channel
  float * get_values(int32_t nvals, int16_t *overflow) {
    int16_t nch = chan_get_num();
    int32_t *dvals = (int32_t*)malloc(sizeof(int32_t)*nch*nvals);
    float   *vals = (float*)malloc(sizeof(float)*nch*nvals);

    int32_t resn = HRDLGetValues(devh,dvals,overflow,nvals); // returns actual number of samples
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

  // measure a single channel
  float get_single_value(
      const int16_t ch,
      const float rng,
      const int16_t convt,
      const bool single,
      bool & ovfl) override{

    if (ch<0 || ch>15) throw Err() << "channel number 0..15 expected";
    int32_t val;
    int16_t ovfl_mask;

    auto res = HRDLGetSingleValue(devh, ch,
      volt2range(rng),
      tconvi2convtime(convt),
      single, &ovfl_mask, &val);
    ovfl = ovfl & (1<<ch);
    return rng*val*0.001/get_max(ch); // Volts
  }

};

#endif


