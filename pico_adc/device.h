#ifndef DEVICE_H
#define DEVICE_H

// Base class for ADC devices

#include <pico/HRDL.h>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

// analog channel configuration
struct ChConf{
  bool en;
  bool sngl;
  float rng;
  int32_t min;
  int32_t max;
  std::vector<int16_t> buf;
};

// digital channel configuration
struct DChConf{
  bool en;
  bool sngl;
  float rng;
  std::vector<int16_t> buf;
};

class ADCInt{
private:
  std::map<int16_t, ChConf> chconf;   // analog channel configuration
  std::map<int16_t, DChConf> dchconf; // digital channel configuration
  bool waiting;

protected:
  bool sixtyHz;  // mains frequency
  bool *chconf_en;
  bool *chconf_sngl;
  float *chconf_rng;
  float *chconf_max;
  int16_t chN;

public:

  // High-level commands.
  // return true if #OK should be printed
  bool cmd(const std::vector<std::string> & args);
  bool is_cmd(const std::vector<std::string> & args, const char *name);
  const char * cmd_help() const; // return command help

  /*****************************************************************************/
  // low-level ADC interface commands defined somewhere else
  // set channel:  chan_set("1", true, , true);

  // get avaiable ranges: chan_get_ranges("1");
  virtual std::string chan_get_ranges(const char * chan) = 0;

  // get avaiable time conversion conctants
  virtual std::string chan_get_tconvs() = 0;

  // returns the number of analog channels enabled
  virtual int16_t chan_get_num() = 0;

  // set channel parameters
  virtual void chan_set(int16_t chan, bool enable, bool sngl, float rng) = 0;

  // returns unit id
  virtual const char * get_unit_id() = 0;

  // returns settings error
  virtual const char * get_err() = 0;

  // configures the mains noise rejection setting
  virtual void set_mains() = 0;

  // sets the sampling time interval
  virtual void set_interval(int32_t dt, int16_t conv) = 0;

  // run block mode
  virtual void run_block(int32_t nvals) = 0;

  // is device ready?
  virtual bool is_ready() = 0;

  // get the requested number of samples for each enabled channel
  virtual float * get_values(int32_t nvals ,int16_t *overflow) = 0;

  // get the maximum and minimum ADC count available for the device
  virtual int32_t get_max(int16_t ch) = 0;

};

#endif


