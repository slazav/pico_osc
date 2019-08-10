#ifndef PICOADC_INT_H
#define PICOADC_INT_H

// ADC interface

#include <pico/HRDL.h>
#include <string>
#include <vector>
#include <stdint.h>

class ADCInt{
private:
  int16_t h; // device handle

public:

  // constructor and destructor: open/close device, throw errors if any
  ADCInt();
  ~ADCInt();

  // High-level commands.
  // return true if #OK should be printed
  bool cmd(const std::vector<std::string> & args);
  bool is_cmd(const std::vector<std::string> & args, const char *name);
  const char * cmd_help() const; // return command help

  /*****************************************************************************/
  // low-level ADC interface commands defined somewhere else
};


#endif


