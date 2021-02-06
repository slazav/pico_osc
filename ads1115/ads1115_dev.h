#ifndef MY_ADS1115_H
#define MY_ADS1115_H

#include <string>

class ADS1115 {
  int fd;

  int encode_chan(const std::string & s);
  const char * decode_chan(int v);

  int encode_range(const std::string & s);
  double range_to_num(const std::string & s);
  const char * decode_range(int v);

  int encode_rate(const std::string & s);
  double rate_to_num(const std::string & s);
  const char * decode_rate(int v);

  const char * decode_comp(int v);


public:

  // Constructor. Open the device.
  ADS1115(const char *path, uint8_t addr);

  ~ADS1115();

  // Print all information from device registers
  void print_info();

  // Do a single measurement, return result [V]
  double meas(const std::string & chan,
       const std::string & range, const std::string & rate);
};


#endif