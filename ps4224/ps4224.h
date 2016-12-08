#ifndef PS4224_H
#define PS4224_H

#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cmath>
#include "err.h"
#include "buf.h"
#include "convs.h"
#include "ps_int.h"

class PS4224 : public PSInterface {
  int16_t h; // device handle

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
  PS4224(const char *name = NULL){
    int16_t res;
    if (name && strlen(name)) res = ps4000OpenUnitEx(&h, (int8_t *)name);
    else  res = ps4000OpenUnit(&h);
    if (res!=PICO_OK) throw Err() << "OpenUnit error: " << pico_err(res);
    res = ps4000Stop(h);
    if (res!=PICO_OK) throw Err() << "Stop error: " << pico_err(res);
  }

  // destructor: close device
  ~PS4224(){
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
    if (res!=PICO_OK) throw Err() << "RunBlock error:" << pico_err(res);
    *dt = tbase2dt(tb);
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


