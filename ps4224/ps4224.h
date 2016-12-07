#ifndef PS4224_H
#define PS4224_H

#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cmath>
#include "err.h"
#include "buf.h"
#include "convs.h"

class PS4224{
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
    int64_t res = ps4000Stop(h);
    if (res!=PICO_OK) throw Err() << "Stop error: " << pico_err(res);
  }

  /********************************************************************/
  // input parameters for high-level record function
  struct InPars{
    bool  use_a, use_b;
    float range_a, range_b;       // V
    std::string coupl_a, coupl_b; // "DC", "AC"
    std::string trig_src;         // "", "A", "B"
    float trig_level;             // V
    std::string trig_dir;         // "RISING", "FALLING"
    float dt;      // time step, s
    uint32_t nrec; // total number of samples
    uint32_t npre; // pretrigger samples

    // defaults
    InPars(){
      use_a = use_b = true;
      range_a = range_b = 1;
      coupl_a = coupl_b = "DC";
      trig_src = ""; // none
      trig_level = 0;
      trig_dir = "RISING";
      dt    = 1e-3;
      nrec  = 1024;
      npre  = 0;
    }
  };

  // output parameters for high-level record function
  struct OutPars{
    Buf<int16_t> bufa, bufb;    // data buffers
    float t0, dt;     // tstart, tstep
    float sc_a, sc_b; // data scale
    bool overflow_a, overflow_b;
    // defaults
    OutPars(){
      t0=dt=0;
      sc_a=sc_b=1;
      overflow_a=overflow_b=false;
    }
  };

  // high-level record function
  OutPars record(const InPars & pi){
    OutPars po;

    if (pi.use_a){
      chan_set("A", pi.coupl_a.c_str(), pi.range_a);
      po.bufa = Buf<int16_t>(pi.nrec);
      set_buf("A", po.bufa);
    }
    else chan_disable("A");

    if (pi.use_b){
      chan_set("B", pi.coupl_b.c_str(), pi.range_b);
      po.bufb = Buf<int16_t>(pi.nrec);
      set_buf("B", po.bufb);
    }
    else chan_disable("B");

    if (pi.trig_src!=""){
      int32_t trig_level=0;
      if (pi.trig_src=="A")
        trig_level = pi.trig_level/pi.range_a*PS4000_MAX_VALUE;
      if (pi.trig_src=="B")
        trig_level = pi.trig_level/pi.range_b*PS4000_MAX_VALUE;
      trig_set(pi.trig_src.c_str(), trig_level, pi.trig_dir.c_str(),0,0);
    }
    else trig_disable();

    po.dt = pi.dt;
    run_block(pi.nrec, pi.npre, &po.dt);
    usleep(pi.nrec*po.dt*1e6);
    while (!is_ready()) usleep(pi.nrec*po.dt*1e6/100);

    int16_t o;
    uint32_t n = pi.nrec;
    get_data(0, &n, &o);
    po.overflow_a = (bool)(o&1);
    po.overflow_b = (bool)(o&2);

    po.t0 = get_trig() - pi.npre*po.dt;
    po.sc_a = pi.range_a/PS4000_MAX_VALUE;
    po.sc_b = pi.range_a/PS4000_MAX_VALUE;

    return po;
  }
};


#endif


