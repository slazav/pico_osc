#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <cmath>
#include <sys/time.h> // gettimeofday

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
ADS1115::get_range(uint16_t conf){
  int v = (conf>>9)&7;
  switch(v){
    case 0: return 6.144;
    case 1: return 4.096;
    case 2: return 2.048;
    case 3: return 1.024;
    case 4: return 0.512;
    case 5:
    case 6:
    case 7: return 0.256;
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

int
ADS1115::get_rate(uint16_t conf){
  int v = (conf>>5)&7;
  switch(v){
    case 0: return 8;
    case 1: return 16;
    case 2: return 32;
    case 3: return 64;
    case 4: return 128;
    case 5: return 250;
    case 6: return 475;
    case 7: return 860;
  }  throw Err() << "unknown rate setting: " << v;
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
  std::cout << "range:      " << std::fixed << std::setw(3) << get_range(data) << " V\n";
  std::cout << "mode:       " << ((data>>8)? "single":"cont") << "\n";
  std::cout << "rate:       1/" << get_rate(data) << " s\n";
  std::cout << "comp.type:  " << ((data>>4) ? "window":"trad") << "\n";
  std::cout << "comp.pol:   " << ((data>>3) ? "act.high":"act.low") << "\n";
  std::cout << "comp.latch: " << ((data>>2) ? "on":"off") << "\n";
  std::cout << "comp.queue: " << get_comp(data) << "\n";
}

double
ADS1115::tstamp(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 1e-6*tv.tv_usec;
}

double
ADS1115::meas(uint16_t conf) {
  // write conf register
  i2c_write_word(fd, REG_CONF, conf);

  // wait for conversion
  usleep(1e6/get_rate(conf));
  while (1) {
    uint16_t data = i2c_read_word(fd, REG_CONF);
    if (data>>15) break;
    usleep(100);
  }

  // read data
  return i2c_read_word(fd, REG_CONV)/32768.0 * get_range(conf);
}

uint16_t
ADS1115::make_conf(const std::string & chan,
          const std::string & range, const std::string & rate){
  uint16_t conf=0x8583; // default value
  set_chan(&conf, chan);
  set_range(&conf, range);
  set_rate(&conf, rate);
  conf |= 1 << 15; // start conversion
  return conf;
}

void
ADS1115::meas_n(const std::string & chan,
       const std::string & range, const std::string & rate,
       const size_t nmeas, double & t, double & m, double & s){

  uint16_t conf = make_conf(chan, range, rate);

  std::vector<double> vs;
  double t1 = tstamp();
  for (int j=0; j<nmeas; j++) vs.push_back(meas(conf));
  double t2 = tstamp();

  m = s = 0;
  for (const auto vx:vs) m += vx;
  m /= vs.size();
  for (const auto vx:vs) s += (vx - m) * (vx - m);
  s = sqrt(s / (vs.size()-1)) / sqrt(vs.size());
  t = t1/2.0 + t2/2.0;

  // limit uncertainty to ADC resolution
  s = std::max(s, get_range(conf)/32768.0);
}
