#include <iostream>
#include <iomanip>
#include <unistd.h>

#include "ads1115_dev.h"
#include "i2c.h"
#include "err/err.h"

// device registers
#define REG_CONV 0
#define REG_CONF 1
#define REG_LTHR 2
#define REG_HTHR 3

/*********************************************************************/

void
ADS1115::set_chan(uint16_t *conf, const std::string & s){
  *conf &= ~(7<<12);
  if (s == "AB") {*conf|= 0<<12; return;}
  if (s == "AD") {*conf|= 1<<12; return;}
  if (s == "BD") {*conf|= 2<<12; return;}
  if (s == "CD") {*conf|= 3<<12; return;}
  if (s == "A")  {*conf|= 4<<12; return;}
  if (s == "B")  {*conf|= 5<<12; return;}
  if (s == "C")  {*conf|= 6<<12; return;}
  if (s == "D")  {*conf|= 7<<12; return;}
  throw Err() << "unknown channel setting: " << s;
}

const char *
ADS1115::get_chan(uint16_t conf){
  int v = (conf>>12)&7;
  switch(v){
    case 0: return "AB";
    case 1: return "AD";
    case 2: return "BD";
    case 3: return "CD";
    case 4: return "A";
    case 5: return "B";
    case 6: return "C";
    case 7: return "D";
  }
  throw Err() << "unknown channel setting: " << v;
}

void
ADS1115::set_range(uint16_t *conf, const std::string & s){
  *conf &= ~(7<<9);
  if (s == "6.144") {*conf|= 0<<9; return;}
  if (s == "4.096") {*conf|= 1<<9; return;}
  if (s == "2.048") {*conf|= 2<<9; return;}
  if (s == "1.024") {*conf|= 3<<9; return;}
  if (s == "0.512") {*conf|= 4<<9; return;}
  if (s == "0.256") {*conf|= 5<<9; return;}
  throw Err() << "unknown range setting: " << s;
}

double
ADS1115::range_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
ADS1115::get_range(uint16_t conf){
  int v = (conf>>9)&7;
  switch(v){
    case 0: return "6.144";
    case 1: return "4.096";
    case 2: return "2.048";
    case 3: return "1.024";
    case 4: return "0.512";
    case 5:
    case 6:
    case 7: return "0.256";
  }
  throw Err() << "unknown range setting: " << v;
}

void
ADS1115::set_rate(uint16_t *conf, const std::string & s){
  *conf &= ~(7<<5);
  if (s == "8")   {*conf|= 0<<5; return;}
  if (s == "16")  {*conf|= 1<<5; return;}
  if (s == "32")  {*conf|= 2<<5; return;}
  if (s == "64")  {*conf|= 3<<5; return;}
  if (s == "128") {*conf|= 4<<5; return;}
  if (s == "250") {*conf|= 5<<5; return;}
  if (s == "475") {*conf|= 6<<5; return;}
  if (s == "860") {*conf|= 7<<5; return;}
  throw Err() << "unknown rate setting: " << s;
}

double
ADS1115::rate_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
ADS1115::get_rate(uint16_t conf){
  int v = (conf>>5)&7;
  switch(v){
    case 0: return "8";
    case 1: return "16";
    case 2: return "32";
    case 3: return "64";
    case 4: return "128";
    case 5: return "250";
    case 6: return "475";
    case 7: return "860";
  }
  throw Err() << "unknown rate setting: " << v;
}

const char *
ADS1115::get_comp(uint16_t conf){
  int v = conf&3;
  switch(v){
    case 0: return "after one";
    case 1: return "after two";
    case 2: return "after four";
    case 3: return "off";
  }
  throw Err() << "unknown comparator setting: " << v;
}

// Constructor. Open the device.
ADS1115::ADS1115(const char *path, uint8_t addr) {
  fd = i2c_open(path, addr);
}

ADS1115::~ADS1115() {
  i2c_close(fd);
}


void
ADS1115::print_info(){
  // conversion register
  uint16_t data = i2c_read_word(fd, REG_CONV);
  std::cout << "conversion register: 0x" << std::hex << data << "\n";

  // low threshold
  data = i2c_read_word(fd, REG_LTHR);
  std::cout << "lo_thresh register: 0x" << std::hex << data << "\n";

  // high threshold
  data = i2c_read_word(fd, REG_HTHR);
  std::cout << "hi_thresh register: 0x" << std::hex << data << "\n";

  // Config register
  data = i2c_read_word(fd, REG_CONF);
  if (data<0) throw Err() << "can't read Config register";
  std::cout << "config register:    0x" << std::hex << data << "\n";
  std::cout << "status:     " << ((data>>15)? "ready":"busy") << "\n";
  std::cout << "chan:       " << get_chan(data) << "\n";
  std::cout << "range:      " << get_range(data) << " V\n";
  std::cout << "mode:       " << ((data>>8)? "single":"cont") << "\n";
  std::cout << "rate:       1/" << get_rate(data) << " s\n";
  std::cout << "comp.type:  " << ((data>>4) ? "window":"trad") << "\n";
  std::cout << "comp.pol:   " << ((data>>3) ? "act.high":"act.low") << "\n";
  std::cout << "comp.latch: " << ((data>>2) ? "on":"off") << "\n";
  std::cout << "comp.queue: " << get_comp(data) << "\n";
}


double
ADS1115::meas(const std::string & chan,
             const std::string & range, const std::string & rate){

  uint16_t conf=0x8583; // default value

  set_chan(&conf, chan);
  set_range(&conf, range);
  set_rate(&conf, rate);
  conf |= 1 << 15; // start conversion

  int delay = 1000000/rate_to_num(rate);
  double scale = range_to_num(range) / 32768.0;

  // write conf register
  i2c_write_word(fd, REG_CONF, conf);

  // wait for conversion
  usleep(delay);
  while (1) {
    uint16_t data = i2c_read_word(fd, 1);
    if (data>>15) break;
    usleep(100);
  }

  // read data
  int16_t data = i2c_read_word(fd, REG_CONV);
  return data * scale;
}
