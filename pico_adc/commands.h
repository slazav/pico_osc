#ifndef COMMANDS_H
#define COMMANDS_H

#include "device.h"
#include <string>
#include <vector>

// define all SPP commands
void cmd(ADC24 &dev, const std::vector<std::string> & args);

#endif
