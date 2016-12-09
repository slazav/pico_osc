#ifndef PICO_4224_H
#define PICO_4224_H

#include <pico/ps4000Api.h>
#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cstring>  // strcasecmp
#include <cmath>
#include "err.h"
#include "buf.h"
#include "pico_err.h"
#include "pico_int.h"

class Pico4224 : public PicoInt {
  int16_t h; // device handle

  /**********************************************************/

  // convert coupling AC,DC
  int16_t str2coupl(const char *str){
    if (strcasecmp(str, "AC")==0) return 0;
    if (strcasecmp(str, "DC")==0) return 1;
    throw Err() << "error: unknown coupling (should be AC or DC): " << str;
  }
  const char * coupl2str(int16_t n){
    return n? "DC":"AC";
  }

  // convert channel (A,B,C,D,EXT)
  PS4000_CHANNEL str2chan(const char *str){
    if (strcasecmp(str, "A")==0) return PS4000_CHANNEL_A;
    if (strcasecmp(str, "B")==0) return PS4000_CHANNEL_B;
    if (strcasecmp(str, "C")==0) return PS4000_CHANNEL_C;
    if (strcasecmp(str, "D")==0) return PS4000_CHANNEL_D;
    if (strcasecmp(str, "EXT")==0) return PS4000_EXTERNAL;
      throw Err() << "error: unknown channel (should be A,B,C,D or EXT): " << str;
  }
  const char * chan2str(int16_t n){
    if (n == PS4000_CHANNEL_A) return "A";
    if (n == PS4000_CHANNEL_B) return "B";
    if (n == PS4000_CHANNEL_C) return "C";
    if (n == PS4000_CHANNEL_D) return "D";
    if (n == PS4000_EXTERNAL) return "EXT";
    throw Err() << "error: unknown channel: " << n;
  }

  // convert voltage range
  PS4000_RANGE volt2range(float v){
    int range = (int)(1000*v);
    switch (range){
      case    10: return PS4000_10MV;
      case    20: return PS4000_20MV;
      case    50: return PS4000_50MV;
      case   100: return PS4000_100MV;
      case   200: return PS4000_200MV;
      case   500: return PS4000_500MV;
      case  1000: return PS4000_1V;
      case  2000: return PS4000_2V;
      case  5000: return PS4000_5V;
      case 10000: return PS4000_10V;
      case 20000: return PS4000_20V;
      case 50000: return PS4000_50V;
      case 100000: return PS4000_100V;
    }
    throw Err() << "error: unknown input range: " << v;
  }
  const char * range2str(PS4000_RANGE n){
      switch (n){
      case PS4000_10MV:  return "0.01";
      case PS4000_20MV:  return "0.02";
      case PS4000_50MV:  return "0.05";
      case PS4000_100MV: return "0.1";
      case PS4000_200MV: return "0.2";
      case PS4000_500MV: return "0.5";
      case PS4000_1V:    return "1";
      case PS4000_2V:    return "2";
      case PS4000_5V:    return "5";
      case PS4000_10V:   return "10";
      case PS4000_20V:   return "20";
      case PS4000_50V:   return "50";
      case PS4000_100V:  return "100";
    }
    throw Err() << "error: unknown input range: " << n;
  }

  // convert trigger direction
  THRESHOLD_DIRECTION str2dir(const char *str){
    if (strcasecmp(str, "above")==0)   return ABOVE;
    if (strcasecmp(str, "below")==0)   return BELOW;
    if (strcasecmp(str, "rising")==0)  return RISING;
    if (strcasecmp(str, "falling")==0) return FALLING;
    if (strcasecmp(str, "rising_or_falling")==0) return RISING_OR_FALLING;
    throw Err() << "error: unknown trigger dir: " << str;
  }
  const char * dir2str(THRESHOLD_DIRECTION n){
    switch (n){
      case ABOVE:   return "ABOVE";
      case BELOW:   return "BELOW";
      case RISING:  return "RISING";
      case FALLING: return "FALLING";
      case RISING_OR_FALLING: return "RISING_OR_FALLING";
    }
    throw Err() << "error: unknown trigger dir: " << n;
  }

  // convert time units
  PS4000_TIME_UNITS str2tunits(const char *str){
    if (strcasecmp(str, "fs")==0)    return PS4000_FS;
    if (strcasecmp(str, "ps")==0)    return PS4000_PS;
    if (strcasecmp(str, "ns")==0)    return PS4000_NS;
    if (strcasecmp(str, "us")==0)    return PS4000_US;
    if (strcasecmp(str, "ms")==0)    return PS4000_MS;
    if (strcasecmp(str, "s")==0)    return PS4000_S;
    throw Err() << "error: unknown time units: " << str;
  }
  const char * tunits2str(PS4000_TIME_UNITS n){
    switch (n){
      case PS4000_FS: return "fs";
      case PS4000_PS: return "ps";
      case PS4000_NS: return "ns";
      case PS4000_US: return "us";
      case PS4000_MS: return "ms";
      case PS4000_S:  return "s";
    }
    throw Err() << "error: unknown time units: " << n;
  }
  double time2dbl(uint32_t t, PS4000_TIME_UNITS tu){
    switch (tu){
      case PS4000_FS: return (double)t * 1e-15;
      case PS4000_PS: return (double)t * 1e-12;
      case PS4000_NS: return (double)t * 1e-9;
      case PS4000_US: return (double)t * 1e-6;
      case PS4000_MS: return (double)t * 1e-3;
      case PS4000_S:  return (double)t;
    }
    throw Err() << "error: unknown time units: %i\n" << tu;
  }
  // for streaming mode: convert double time into uint32_t + TIME_UNITS
  uint32_t dbl2time(double t, PS4000_TIME_UNITS *tu){
//    if (t<1e-6) { *tu=PS4000_FS; return round(t*1e15); }
//    if (t<1e-3) { *tu=PS4000_PS; return round(t*1e12); }
    if (t<1)    { *tu=PS4000_NS; return round(t*1e9); }
    if (t<1e3)  { *tu=PS4000_US; return round(t*1e6); }
    if (t<1e6)  { *tu=PS4000_MS; return round(t*1e3); }
    *tu=PS4000_S;  return round(t);
  }

  /**********************************************************/
  public:

  // detect all pico devices and print result to stdout.
  std::string static dev_list(){
    const int buflen=2048;
    int16_t cnt, len;
    char buf[buflen];
    len=buflen;
    int16_t res=ps4000EnumerateUnits(&cnt, (int8_t *)buf, &len);
    if (res!=PICO_OK) throw Err() << "EnumerateUnits error: " << pico_err(res);
    if (cnt>0) return std::string(buf);
    else return std::string();
  }

  // constructor: open pico device
  Pico4224(const char *name = NULL){
    int16_t res;
    if (name && strlen(name)) res = ps4000OpenUnitEx(&h, (int8_t *)name);
    else  res = ps4000OpenUnit(&h);
    if (res!=PICO_OK) throw Err() << "OpenUnit error: " << pico_err(res);
    res = ps4000Stop(h);
    if (res!=PICO_OK) throw Err() << "Stop error: " << pico_err(res);
  }

  // destructor: close device
  ~Pico4224(){
    int16_t res = ps4000CloseUnit(h);
    if (res!=PICO_OK) throw Err() << "CloseUnit error: " << pico_err(res);
  }


  // get avaiable ranges: chan_get_ranges("A");
  std::string chan_get_ranges(const char * chan){
    const int maxlen=100;
    int r[maxlen], len, i;
    int16_t res = ps4000GetChannelInformation(h,
        CI_RANGES, 0, r, &len, str2chan(chan));
    if (res!=PICO_OK) throw Err() << "GetChannelInformation error: " << pico_err(res);
    std::string ret;
    for (i=0; i<len; i++){
      if (i>0) ret+=" ";
      ret+=range2str((PS4000_RANGE)r[i]);
    }
    return ret;
  }

  // set channel:  chan_set("A", "DC", 0.05);
  void chan_set(const char * chan, const char * coupl, float rng){
    int16_t res = ps4000SetChannel(h, str2chan(chan), 1,
                                   str2coupl(coupl), volt2range(rng));
    if (res!=PICO_OK) throw Err() << "SetChannel error: " << pico_err(res);
  }

  // disable channel: chan_disable("B");
  void chan_disable(const char * chan){
    int16_t res = ps4000SetChannel(h, str2chan(chan), 0, 0, PS4000_1V);
    if (res!=PICO_OK) throw Err() << "SetChannel error: " << pico_err(res);
  }

  // set trigger: trig_set("A", 125, "RISING", 0, 0);
  void trig_set(const char * src, int16_t th, const char * dir, uint32_t del, int16_t auto_del){
    int16_t res = ps4000SetSimpleTrigger(h, 1, str2chan(src), th, str2dir(dir), del, auto_del);
    if (res!=PICO_OK)  throw Err() << "SetSimpleTrigger error: " << pico_err(res);
  }

  // disable trigger: trig_disable();
  void trig_disable(){
    int16_t res = ps4000SetSimpleTrigger(h, 0, PS4000_CHANNEL_A, 0, RISING, 0, 0);
    if (res!=PICO_OK)  throw Err() << "SetSimpleTrigger error: " << pico_err(res);
  }

  // convert pico timebase to time step (in seconds): tbase2dt(tbase);
  float tbase2dt(uint32_t tbase){
    float dt;
    int16_t res = ps4000GetTimebase2(h, tbase, 1, &dt, 0, NULL, 0);
    if (res!=PICO_OK) throw Err() << " GetTimebase2 error: " << pico_err(res);
    return dt*1e-9; /* ns -> s*/
  }

  // convert time step (seconds) into timebase: dt2tbase(dt);
  uint32_t dt2tbase(float dt){
    // this works only for 4224, 4224, 4432, 4424
    if (dt <= 12e-9) return 0;
    if (dt <= 50e-9) return round(log(8e7*dt));
    if (dt <= (pow(2,30)-2)/2e7) return floor(2e7*dt+1);
    return  (1<<30)-1;
  }

  // show data buffer to oscilloscope: set_buff("A",buf);
  void set_buf(const char * chan, Buf<int16_t> buf){
    int16_t res = ps4000SetDataBuffer(h, str2chan(chan), buf.data, buf.size);
    if (res!=PICO_OK) throw Err() << "SetDataBuffer error: " << pico_err(res);
  }

  // run block mode, return actual time step: run_block(nrec,npre,dt);
  void run_block(uint32_t nrec, uint32_t npre, float *dt){
    uint32_t tb = dt2tbase(*dt);
    uint16_t os = 1; // I can't see any effect
    int16_t res = ps4000RunBlock(h,
          npre, nrec-npre, tb, os, NULL, 0, NULL, NULL);
    if (res!=PICO_OK) throw Err() << "RunBlock error: " << pico_err(res);
    *dt = tbase2dt(tb);
  }

  // run streaming mode, return actual time step: run_stream(dt);
  void run_stream(float *dt, uint32_t bufsize){
    // convert dt to integer time and time units
    PS4000_TIME_UNITS tu;
    uint32_t ti = dbl2time(*dt, &tu);
    int16_t res = ps4000RunStreaming(h, &ti, tu, 0,0,0,1, bufsize);
    if (res!=PICO_OK) throw Err() << "RunStreaming error: " << pico_err(res);
    *dt = time2dbl(ti,tu);
  }

  // get stream values
  void get_stream(ps4000StreamingReady cb, void *par){
    int16_t res = ps4000GetStreamingLatestValues(h, cb, par);
    if (res!=PICO_OK) throw Err() << "GetStreamingLatestValues error: " << pico_err(res);
  }

  // is device ready?
  bool is_ready(){
    int16_t stat;
    int16_t res = ps4000IsReady(h, &stat);
    if (res!=PICO_OK) throw Err() << "IsReady error: " << pico_err(res);
    return (bool)stat;
  }

  // get data, return number of points and overflow flag
  void get_data(uint32_t start, uint32_t *n, int16_t *overflow){
    int16_t res = ps4000GetValues(h, start, n, 1, RATIO_MODE_NONE, 0, overflow);
    if (res!=PICO_OK) throw Err() << "GetValues error: " <<  pico_err(res);
  }

  // get trigger position
  double get_trig(){
    int64_t ttime;
    PS4000_TIME_UNITS tunits;
    int16_t res = ps4000GetTriggerTimeOffset64(h, &ttime, &tunits, 0);
    if (res!=PICO_OK) { ttime=0; tunits=PS4000_S; }
    return time2dbl(ttime, tunits);
  }

  // stop oscilloscope
  void stop(){
    int16_t res = ps4000Stop(h);
    if (res!=PICO_OK) throw Err() << "Stop error: " << pico_err(res);
  }

  // get max and min values
  int16_t get_max_val() const { return PS4000_MAX_VALUE; }
  int16_t get_min_val() const { return PS4000_MIN_VALUE; }

};


#endif


