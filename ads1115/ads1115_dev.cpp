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

int
ADS1115::encode_chan(const std::string & s){
  if (s == "AB") return 0;
  if (s == "AD") return 1;
  if (s == "BD") return 2;
  if (s == "CD") return 3;
  if (s == "A") return 4;
  if (s == "B") return 5;
  if (s == "C") return 6;
  if (s == "D") return 7;
  throw Err() << "unknown channel setting: " << s;
}

const char *
ADS1115::decode_chan(int v){
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

int
ADS1115::encode_range(const std::string & s){
  if (s == "6.144") return 0;
  if (s == "4.096") return 1;
  if (s == "2.048") return 2;
  if (s == "1.024") return 3;
  if (s == "0.512") return 4;
  if (s == "0.256") return 5;
  throw Err() << "unknown range setting: " << s;
}

double
ADS1115::range_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
ADS1115::decode_range(int v){
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

int
ADS1115::encode_rate(const std::string & s){
  if (s == "8") return 0;
  if (s == "16") return 1;
  if (s == "32") return 2;
  if (s == "64") return 3;
  if (s == "128") return 4;
  if (s == "250") return 5;
  if (s == "475") return 6;
  if (s == "860") return 7;
  throw Err() << "unknown rate setting: " << s;
}

double
ADS1115::rate_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
ADS1115::decode_rate(int v){
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
ADS1115::decode_comp(int v){
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
  std::cout << "chan:       " << decode_chan((data>>12)&7) << "\n";
  std::cout << "range:      " << decode_range((data>>9)&7) << " V\n";
  std::cout << "mode:       " << ((data>>8)? "single":"cont") << "\n";
  std::cout << "rate:       1/" << decode_rate((data>>5)&7) << " s\n";
  std::cout << "comp.type:  " << ((data>>4) ? "window":"trad") << "\n";
  std::cout << "comp.pol:   " << ((data>>3) ? "act.high":"act.low") << "\n";
  std::cout << "comp.latch: " << ((data>>2) ? "on":"off") << "\n";
  std::cout << "comp.queue: " << decode_comp(data&3) << "\n";
}


double
ADS1115::meas(const std::string & chan,
             const std::string & range, const std::string & rate){

  // default value with cleared mplex (bits 12-14), gain (9-11), rate (5-7)
  uint16_t conf=0b0000000100000011;

  conf |= 1 << 15; // start conversion
  conf |= encode_chan(chan) << 12;
  conf |= encode_range(range) << 9;
  conf |= encode_rate(rate) << 5;

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
