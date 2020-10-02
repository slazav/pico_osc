#ifndef DEVICE_H
#define DEVICE_H

// Base class for ADC devices

#include <string>
#include <vector>
#include <stdint.h>

// analog channel configuration
struct ChConf_t{
  bool en;
  bool sngl;
  float rng;
  int32_t min;
  int32_t max;
  std::vector<int16_t> buf;
  ChConf_t(): en(false), sngl(false), rng(0), min(0), max(0) {}
};

// digital channel configuration
struct DChConf_t{
  bool en;
  bool sngl;
  float rng;
  std::vector<int16_t> buf;
  DChConf_t(): en(0), sngl(0), rng(0) {}
};

class ADC24{

protected:
  std::vector<ChConf_t> chconf;
  int16_t chN;
  int16_t devh; // device handle
  int16_t devn; // device number (1..20)

public:

  // High-level commands.
  void cmd(const std::vector<std::string> & args);

  bool is_cmd(const std::vector<std::string> & args, const char *name);

  const char * cmd_help() const; // return command help


  ADC24(const char *name);

  ~ADC24();

  static std::string dev_list();

  /*****************************************************************************/
  // low-level ADC interface commands defined somewhere else
  // set channel:  chan_set("1", true, , true);

  // get avaiable ranges: chan_get_ranges("1");
  std::string chan_get_ranges(const char * chan);

  // get avaiable time conversion conctants
  std::string chan_get_tconvs();

  // returns the number of analog channels enabled
  int16_t chan_get_num();

  // set channel parameters
  void chan_set(int16_t chan, bool enable, bool sngl, float rng);

  // returns unit id
  const char * get_unit_id();

  // returns settings error
  const char * get_err();

  // configures the mains noise rejection setting
  void set_mains(const bool m60Hz);

  // sets the sampling time interval
  void set_interval(int32_t dt, int16_t conv);

  // run block mode
  void run_block(int32_t nvals);

  // is device ready?
  bool is_ready();

  // get the requested number of samples for each enabled channel
  std::vector<float> get_values(int32_t nvals ,int16_t *overflow);

  // get the maximum and minimum ADC count available for the device
  int32_t get_max(int16_t ch);

  // get device info
  std::string get_info();

  // measure a single channel
  float get_single_value(
      const int16_t ch, const float rng, const int16_t convt,
      const bool single, bool & ovfl);


};

#endif
