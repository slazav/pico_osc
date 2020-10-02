#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include <iostream>
#include <cmath>
#include <bitset>
#include <iomanip>    // setfill
#include <sys/time.h> // gettimeofday
#include "err/err.h"

#include <pico/HRDL.h>
#include "device.h"
#include "ADC24_const.h"

#define VERSION "1.0"

using namespace std;

bool
ADC24::is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0;
}

const char *
ADC24::cmd_help() const {
  return
  "help -- show command list\n"
  "ranges -- show possible ranges\n"
  "chan_set <ch> <en> <sngl> <rng> -- set channel parameters\n"
  "   ch   -- select channel: 1 to 16\n"
  "   en   -- enable channel: 1,0\n"
  "   sngl -- single ended or double: 1,0\n"
  "   rng  -- input range, mV (see ranges command)\n"
  "chan_get <ch>  -- get channel parameters\n"
  "   ch -- select channel: 01,02,07 etc.\n"
  "   Returns a line with four words: <ch> <en> <sngl> <rng> for each channel.\n"
  "   If the channel is disabled returns channel name with 'disabled' word.\n"
  "chan_get_n  -- get number of enabled analog channels\n"
  "set_t <dt> <tconv> -- set time intervals\n"
  "   dt    -- time step in milliseconds within which all conversions \n"
  "            must take place before the next set of conversions starts.\n"
  "   tconv -- the amount of time in milliseconds given to one \n"
  "            channel's conversion (see tconvs command)\n"
  "tconvs -- show possible conversion time constants in milliseconds\n"
  "info -- write device info\n"
  "*idn? -- write id string: \"pico_adc " VERSION "\"\n"
  "get_time -- print current time (unix seconds with ms precision)\n"
  "get_info -- print unit info\n";
}

bool
ADC24::cmd(const vector<string> & args){
  if (args.size()<1) return false;

  // print time
  if (is_cmd(args, "get_time")){
    if (args.size()!=1) throw Err() << "Usage: get_time";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec << "\n";
    return true;
  }

  // print id
  if (is_cmd(args, "*idn?")){
    if (args.size()!=1) throw Err() << "Usage: *idn?";
    cout << "pico_adc " VERSION "\n";
    return true;
  }

  // print help
  if (is_cmd(args, "help")){
    if (args.size()!=1) throw Err() << "Usage: help";
    cout << cmd_help();
    return true;
  }

  // print device info
  if (is_cmd(args, "get_info")){
    if (args.size()!=1) throw Err() << "Usage: get_info";
    cout << "pico_adc " << get_info() << "\n";
    return true;
  }

  // show range settings
  if (is_cmd(args, "ranges")){
    if (args.size()!=1) throw Err() << "Usage: ranges";
    cout << chan_get_ranges(args[1].c_str()) << "\n";
    return true;
  }

  // show time conversion settings
  if (is_cmd(args, "set_t")){
    if (args.size()!=3) throw Err() << "Usage: set_t <dt> <tconv>";
    set_interval(atoi(args[1].c_str()),atoi(args[2].c_str()));
    return true;
  }

  // show time conversion settings
  if (is_cmd(args, "tconvs")){
    if (args.size()!=1) throw Err() << "Usage: ranges";
    cout << chan_get_tconvs() << "\n";
    return true;
  }

  // print number of active channels
  if (is_cmd(args, "chan_get_n")){
    if (args.size()!=1) throw Err() << "Usage: get_chan_n";
    cout << chan_get_num() << "\n";
    return true;
  }

  // set channel parameters
  if (is_cmd(args, "chan_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: chan_set <ch> <en> <sngl> <rng>";
    int8_t len = args[1].length();
    if (len%2) throw Err() << "bad channel option";
    for (int i=0; i<args[1].length(); i+=2){
      string ch; ch+=args[1][i]; ch+=args[1][i+1]; // channel as a string
      int16_t chi = (int16_t)atoi(ch.c_str());     // channel as int
      ChConf_t C;
      C.en  = atoi(args[2].c_str());
      C.sngl = atoi(args[3].c_str());;
      C.rng = atof(args[4].c_str());
      chan_set(chi, C.en, C.sngl, C.rng);
      chconf[chi] = C; // save channel configuration
    }
    return true;
  }

  // set channel parameters
  if (is_cmd(args, "chan_get")) {
    if (args.size()!=2) throw Err() << "Usage: chan_get <ch>";
    int ch = atoi(args[1].c_str());
    if (chconf[ch].en) cout << ch << " "
          << chconf[ch].en << " "
          << chconf[ch].sngl << " "
          << chconf[ch].rng << "\n";
    else cout << ch << " disabled\n";

    return true;
  }

  if (is_cmd(args, "get")) {
    if (args.size()!=1) throw Err()
      << "Usage: get";
    run_block(1);
    usleep(1000);
    while (!is_ready()) {usleep(1000);}
    int16_t overflow;
    auto vals = get_values(1,&overflow);
    for (int i=0; i<chan_get_num(); ++i) {
      if (i>0) std::cout << " ";
      std::cout << vals[i];
    }
    std::cout <<"\n";
    return true;
  }

  throw Err() << "Unknown command: " << args[0];
}


// return newline-separated list of all connected devices
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
std::string
ADC24::dev_list(){
    std::string ret;
    int16_t devices[HRDL_MAX_PICO_UNITS];
    int8_t line[80];

    for (int i = 0; i < HRDL_MAX_UNITS; i++) {
      devices[i] = HRDLOpenUnit();
      if (devices[i] > 0) {
        HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_BATCH_AND_SERIAL);
        ret += std::string((char*)line) + "\n";
      }
      else {
        HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_ERROR);
        if (atoi((char*)line) != HRDL_NOT_FOUND)
          throw Err() << "can't open device " << i << ":" << line;
      }
    }
    // close devices
    for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
      if (devices[i] > 0) HRDLCloseUnit(devices[i]);
    }
    return ret;
  }


// Constructor and destructor: open/close device, throw errors if any.
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
// for their strange way of finding correct device...
ADC24::ADC24(const char *name){

  // open all devices, as in dev_list()
  int16_t devices[HRDL_MAX_PICO_UNITS];
  int8_t line[80];
  devh=-1; devn=-1;

  // initialize device array
  for (int i = 0; i < HRDL_MAX_UNITS; i++) devices[i] = 0;

  // open all devices, as in dev_list(), find which one to use
  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    devices[i] = HRDLOpenUnit();
    if (devices[i] > 0) {
      HRDLGetUnitInfo(devices[i], line, sizeof (line), HRDL_BATCH_AND_SERIAL);
      if (name==0 || name[0] == 0 || strcasecmp(name, (char*)line)==0){
        devh = devices[i]; devn = i; break;
      }
    }
  }

  // close all devices except one
  for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
    if (devices[i] > 0 && i!=devn) HRDLCloseUnit(devices[i]);
  }

  if (devn == -1){
    if (name==0 || name[0] == 0) throw Err() << "No PicoLog devices found";
    else throw Err() << "PicoLog device not found: " << name;
  }

  chN=HRDL_MAX_ANALOG_CHANNELS;
  chconf.resize(chN);
}

ADC24::~ADC24(){}


// get avaiable ranges: chan_get_ranges("A");
std::string
ADC24::chan_get_ranges(const char * chan){
  std::string ret;
  for (int i = 0; i != HRDL_MAX_RANGES; ++i) {
    if (i>0) ret+=" ";
    ret+=range2str((HRDL_RANGE)i);
  }
  return ret;
}

// get avaiable time conversion conctants
std::string
ADC24::chan_get_tconvs() {
  std::string ret;
  for (int i = 0; i != HRDL_MAX_CONVERSION_TIMES; ++i) {
    if (i>0) ret+=" ";
    ret+=tconv2str((HRDL_CONVERSION_TIME)i);
  }
  return ret;
}

void
ADC24::chan_set(int16_t chan, bool enable, bool sngl, float rng) {
  if (!(HRDLSetAnalogInChannel(devh,chan,enable,volt2range(rng),sngl))) throw Err()
    << "failed to set channel " << chan;
}

int16_t
ADC24::chan_get_num() {
  int16_t n;
  if (HRDLGetNumberOfEnabledChannels(devh, &n)) return n;
  throw Err() << "chan_get_num() failed: invalid handle";
}

const char *
ADC24::get_unit_id() {
  int16_t len=20;
  const char *str;
  str = (char*)malloc(sizeof(char)*20);
  HRDLGetUnitInfo(devh,(int8_t *)str,len,HRDL_BATCH_AND_SERIAL);
  return str;
}

// returns settings error
const char *
ADC24::get_err() {
  return NULL;
};

// configures the mains noise rejection setting
void
ADC24::set_mains(bool m60Hz) {
  if (!HRDLSetMains(devh,m60Hz))
    throw Err() << "failed to set mains";
}

// sets the sampling time interval
void
ADC24::set_interval(int32_t dt, int16_t conv) {
  if (!HRDLSetInterval(devh,dt,tconvi2convtime(conv))) throw Err() << "failed to set time interval";
};

// run block mode
void
ADC24::run_block(int32_t nvals) {
  if (!HRDLRun(devh,nvals,str2m("block")))
    throw Err() << "failed to run block mode";
}

// is device ready?
bool
ADC24::is_ready() { return HRDLReady(devh); }

// get the requested number of samples for each enabled channel
std::vector<float>
ADC24::get_values(int32_t nvals, int16_t *overflow) {
  int16_t nch = chan_get_num();
  std::vector<int32_t> dvals(nch*nvals);
  std::vector<float>   vals(nvals);

  // returns actual number of samples
  int32_t resn = HRDLGetValues(devh,dvals.data(),overflow,nvals);
  float rngs[chN], maxcounts[chN];
  int rs_n=0;
  for (int ch = 0; ch<chN; ++ch) {
    if (chconf[ch].en) {
      rngs[rs_n]=chconf[ch].rng;
      maxcounts[rs_n]=chconf[ch].max;
      rs_n++;
    }
  }
  int32_t v=0;
  for (int n=0; n<resn; ++n) {
    for (int ri=0; ri<rs_n; ++ri ) {
      vals[v]=rngs[ri]*dvals[v]/maxcounts[ri]*0.001; // Volts
      v++;
    }
  }
  return vals;
}

// get the maximum and minimum ADC count available for the device
int32_t
ADC24::get_max(int16_t ch) {
  int32_t min,max;
  if (!HRDLGetMinMaxAdcCounts(devh,&min,&max,ch)) throw Err() << "failed to get max ADC count";
  return max;
}

// get device info
std::string
ADC24::get_info(){
  const int16_t ui_buflen=1024;
  char ui_v[ui_buflen],ui_n[ui_buflen];
  if (!HRDLGetUnitInfo(devh,(int8_t *)ui_v,ui_buflen, HRDL_VARIANT_INFO) ||
      !HRDLGetUnitInfo(devh,(int8_t *)ui_n,ui_buflen, HRDL_BATCH_AND_SERIAL))
    throw Err() << "failed to get unit info";
  return std::string(ui_v)+" "+std::string(ui_n);
}


