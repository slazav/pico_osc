#ifndef PICO_INT_H
#define PICO_INT_H

// device-independent PicoScope interface

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#define MAXCH 4

typedef void (StreamingReadyCB)(int16_t, int32_t, uint32_t, int16_t, uint32_t, int16_t, int16_t, void*);

// channel configuration
struct ChConf{
  bool  en;
  float rng;
  std::string cpl;
  std::vector<int16_t> buf;
};

// trigger configuration
struct TrConf{
  std::string src, dir;
  float   lvl;
  int32_t del;
};

// oscilloscope interface
class PicoInt{
  private:
    std::map<char, ChConf> chconf; // channel configuration
    std::vector<TrConf>    trconf; // trigger configuration

  public:

  // high-level commands
  void cmd(const std::vector<std::string> & args);
  const char * cmd_help() const; // return command help

  /*****************************************************************************/
  // low-level oscilloscope interface commands defined somewhere else

  // get avaiable ranges: chan_get_ranges("A");
  virtual std::string chan_get_ranges(const char * chan) = 0;

  // set channel:  chan_set("A", true, "DC", 0.05);
  virtual void chan_set(const char * chan, bool enable, const char * coupl, float rng) = 0;

  // set trigger: trig_set("A", 125, "RISING", 0, 0);
  // if src == "", "OFF", "NONE" then disable trigger
  virtual void trig_set(const char * src, int16_t th, const char * dir,
                        uint32_t del, int16_t auto_del)=0;

  // convert pico timebase to time step (in seconds): tbase2dt(tbase);
  virtual float tbase2dt(uint32_t tbase) = 0;

  // convert time step (seconds) into timebase: dt2tbase(dt);
  virtual uint32_t dt2tbase(float dt) = 0;

  // show data buffer to oscilloscope: set_buff("A",buf);
  virtual void set_buf(const char * chan, int16_t *buf, uint32_t bufsize) = 0;

  // run block mode, return actual time step: run_block(nrec,npre,dt);
  virtual void run_block(uint32_t npre, uint32_t npost, float *dt) = 0;

  // is device ready?
  virtual bool is_ready() = 0;

  // get data, return number of points and overflow flag
  virtual void get_block(uint32_t start, uint32_t *n, int16_t *overflow) = 0;

  // run streaming mode, return actual time step: run_stream(dt);
  virtual void run_stream(uint32_t npre, uint32_t npost, float *dt, uint32_t bufsize) = 0;

  // get number of collected values in streaming mode
  virtual uint32_t get_stream_n() = 0;

  // get stream values
  virtual void get_stream(StreamingReadyCB cb, void *par) =0 ;

  // get trigger position
  virtual double get_trig() = 0;

  // stop oscilloscope
  virtual void stop() = 0;

  // get max and min values
  virtual int16_t get_max_val() const = 0;
  virtual int16_t get_min_val() const = 0;

};


#endif


