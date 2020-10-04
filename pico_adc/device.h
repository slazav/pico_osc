#ifndef ADC24_H
#define ADC24_H

#define VERSION "1.0"

#include <pico/HRDL.h>
#include <string>
#include <vector>
#include <stdint.h>

// analog channel configuration
struct ChConf_t{
  int16_t en;
  int16_t sngl;
  HRDL_RANGE rng;
  int32_t min, max;
  ChConf_t(): en(false), sngl(false), rng(HRDL_2500_MV),min(-1),max(1) {}
};

class ADC24{

protected:
  std::vector<ChConf_t> chconf; // channel configuration for block mode
  int16_t devh; // device handle
  int16_t devn; // device number (1..20)
  bool time_conf; // are timings configured (block mode stucks if not!)

public:

  /**********************************************************/

  // return list of available devices
  static std::string dev_list();

  // constructor: find and open device with the name
  ADC24(const std::string & name);

  // destructor: close device
  ~ADC24();

  /**********************************************************/

  // General device/program settings

  // Return unit information for a handler (used in list method
  // and constuctor with different handlers).
  static std::string get_unit_info(const int16_t h, const uint16_t info);

  // Get error information and return message
  std::string get_error();

  // Configure the mains noise rejection setting (60 or 50 Hz).
  void set_mains(const bool m60Hz);

  // Print device information.
  void print_info();

  // Get available range constants.
  std::string get_ranges();

  // Get available time conversion constants.
  std::string get_tconvs();

  // Measure a single value with full channel setup.
  // (no need for other setup functions)
  // returns overflow flag.
  double get_single(const int ch, const bool single,
      const std::string & rng, const std::string & convt);

  // Set four digital lines as outputs, set values
  // according with the bitmask
  void set_dig_out(const int bitmask);

  // Block read mode (set channels, set timings, read data block).
  // Using this mode by multiple users (through device server)
  // will require device locking between setting parameters
  // and making the measurement.

  // Set channel parameters.
  void set_channel(int chan, bool enable,
                bool single, const std::string & rng);

  // Disable all channels
  void disable_all();

  // Print channel settings.
  void print_channel(int chan);

  // Set timing parameters.
  void set_timing(int32_t dt, const std::string & convt);

  // Returns the number of channels enabled.
  int chan_get_num();

  // Returns list of enabled channels.
  std::vector<int> chan_get_list();

  // Get block of data.
  std::vector<double> get_block(int32_t nvals);

};

/*************************/

#include <sstream>
#include <iomanip>
#include "err/err.h"
// convert values to strings and back
template<typename T>
T str_to_type(const std::string & s){
  std::istringstream ss(s);
  T val;
  ss >> std::showbase >> val;
  if (ss.fail() || !ss.eof())
    throw Err() << "can't parse value: \"" << s << "\"";
  return val;
}
template<typename T>
std::string type_to_str(const T & t){
  std::ostringstream ss;
  ss << t;
  return ss.str();
}

#endif
