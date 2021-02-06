#ifndef MY_ADS1115_H
#define MY_ADS1115_H

#include <string>

class ADS1115 {
  int fd;

  void set_chan(uint16_t *conf, const std::string & s);
  const char * get_chan(uint16_t conf);

  void set_range(uint16_t *conf, const std::string & s);
  double range_to_num(const std::string & s);
  const char * get_range(uint16_t conf);

  void set_rate(uint16_t *conf, const std::string & s);
  double rate_to_num(const std::string & s);
  const char * get_rate(uint16_t conf);

  const char * get_comp(uint16_t conf);


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