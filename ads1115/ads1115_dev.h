#ifndef MY_ADS1115_H
#define MY_ADS1115_H

#include <string>

class ADS1115 {
  int fd;

  void set_chan(uint16_t *conf, const std::string & s);
  const char * get_chan(uint16_t conf);

  void set_range(uint16_t *conf, const std::string & s);
  double get_range(uint16_t conf);

  void set_rate(uint16_t *conf, const std::string & s);
  int get_rate(uint16_t conf);

  const char * get_comp(uint16_t conf);


public:

  // Constructor. Open the device.
  ADS1115(const char *path, uint8_t addr);

  ~ADS1115();

  // Print all information from device registers
  void print_info();

  double tstamp(); // get timestamp

  // Parse parameters and prepare config register for measurements.
  uint16_t make_conf(const std::string & chan,
             const std::string & range, const std::string & rate);

  // Do a single measurement using a prepared config register, return result [V].
  double meas(uint16_t conf);


  // Parse, parameters, prepare config register, do measurements
  double meas(const std::string & chan,
       const std::string & range, const std::string & rate){
    return meas(make_conf(chan, range, rate));}

  // Do multiple (nmeas) measurements, return time, mean value, unsertainty
  void meas_n(const std::string & chan,
       const std::string & range, const std::string & rate,
       const size_t nmeas, double & t, double & m, double & s);


};


#endif