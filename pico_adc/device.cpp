#include <cstring> // strcasecmp
#include <unistd.h> // usleep
#include <fstream>
#include <iostream>
#include <cmath>
#include <bitset>
#include <iomanip>    // setfill
#include "err/err.h"

#include <pico/HRDL.h>
#include "device.h"
#include "ADC24_const.h"

using namespace std;

/**********************************************************/
// return newline-separated list of all connected devices
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
// (There is an error in this code with HRDL_MAX_PICO_UNITS/HRDL_MAX_UNITS. Here I use HRDL_MAX_UNITS everywhere)
std::string
ADC24::dev_list(){
  std::string ret;
  int16_t devices[HRDL_MAX_UNITS];

  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    devices[i] = HRDLOpenUnit();
    if (devices[i] > 0) {
      auto line = get_unit_info(devices[i], HRDL_BATCH_AND_SERIAL);
      ret += line + "\n";
    }
    else {
      auto err = str_to_type<int>(get_unit_info(devices[i], HRDL_ERROR));
      if (err != HRDL_NOT_FOUND)
        throw Err() << "can't open device " << i << ":" << err_to_str(err);
    }
  }
  // close devices
  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    if (devices[i] > 0) HRDLCloseUnit(devices[i]);
  }
  return ret;
}

// Constructor and destructor: open/close device, throw errors if any.
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
// for their strange way of finding correct device...
ADC24::ADC24(const std::string & name): time_conf(false){

  // open all devices, as in dev_list()
  int16_t devices[HRDL_MAX_UNITS];
  devh=-1; devn=-1;

  // initialize device array
  for (int i = 0; i < HRDL_MAX_UNITS; i++) devices[i] = 0;

  // open all devices, as in dev_list(), find which one to use
  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    devices[i] = HRDLOpenUnit();
    if (devices[i] > 0) {
      try {
        auto line = get_unit_info(devices[i], HRDL_BATCH_AND_SERIAL);
        if (name=="" || strcasecmp(name.c_str(), line.c_str())==0){
          devh = devices[i]; devn = i; break;
        }
      }
      catch (const Err & e) {}
    }
  }

  // close all devices except one
  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    if (devices[i] > 0 && i!=devn) HRDLCloseUnit(devices[i]);
  }

  if (devn == -1){
    if (name=="") throw Err() << "No PicoLog devices found";
    else throw Err() << "PicoLog device not found: " << name;
  }

  chconf.resize(HRDL_MAX_ANALOG_CHANNELS);
}

// destructor
ADC24::~ADC24(){
  HRDLCloseUnit(devh);
}


/**********************************************************/

// Returns unit info for a device handle (used in list method and constuctor)
std::string
ADC24::get_unit_info(const int16_t h, const uint16_t info) {
  std::string str('0', 80);
  int16_t l = HRDLGetUnitInfo(h,(int8_t *)str.data(),str.size(),info);
  if (l==0) throw Err() << "failed to get unit info";
  return str.substr(0,l);
}

std::string
ADC24::get_error(){
  // In the case of parameter error HRDL returns strange string: 8PPPP<...>.
  // Use only the first digit to get error code:
  auto e = get_unit_info(devh, HRDL_ERROR);
  int ne = e.size()? e[0]-'0' : 0;
  if (ne != 0 && ne != 8) return err_to_str(ne);

  e = get_unit_info(devh, HRDL_SETTINGS);
  ne = e.size()? e[0]-'0' : 0;
  return serr_to_str(ne);
}

// configures the mains noise rejection setting
void
ADC24::set_mains(bool m60Hz) {
  if (!HRDLSetMains(devh,m60Hz))
    throw Err() << "failed to set mains: " << get_error();
}

// get device information
void
ADC24::print_info(){
  const int16_t ui_buflen=1024;
  char ui_v[ui_buflen],ui_n[ui_buflen];
  std::cout << "DRIVER_VERSION:        " << get_unit_info(devh, HRDL_DRIVER_VERSION) << "\n";
  std::cout << "HARDWARE_VERSION:      " << get_unit_info(devh, HRDL_HARDWARE_VERSION) << "\n";
  std::cout << "VARIANT_INFO:          " << get_unit_info(devh, HRDL_VARIANT_INFO) << "\n";
  std::cout << "BATCH_AND_SERIAL:      " << get_unit_info(devh, HRDL_BATCH_AND_SERIAL) << "\n";
  std::cout << "CAL_DATE:              " << get_unit_info(devh, HRDL_CAL_DATE) << "\n";
  std::cout << "KERNEL_DRIVER_VERSION: " << get_unit_info(devh, HRDL_KERNEL_DRIVER_VERSION) << "\n";
  std::cout << "ERROR CODE:            " << get_unit_info(devh, HRDL_ERROR) << "\n";
  std::cout << "SETTINGS ERROR CODE:   " << get_unit_info(devh, HRDL_SETTINGS) << "\n";
  std::cout << "ERROR MESSAGE:         " << get_error() << "\n";
}

// get available range constants
std::string
ADC24::get_ranges(){
  std::string ret;
  for (int i = 0; i != HRDL_MAX_RANGES; ++i) {
    if (i>0) ret+=" ";
    ret+=range_to_str((HRDL_RANGE)i);
  }
  return ret;
}

// get available time conversion constants
std::string
ADC24::get_tconvs() {
  std::string ret;
  for (int i = 0; i != HRDL_MAX_CONVERSION_TIMES; ++i) {
    if (i>0) ret+=" ";
    ret+=convt_to_str((HRDL_CONVERSION_TIME)i);
  }
  return ret;
}

// measure a single value with full channel setup.
double
ADC24::get_single( const int ch, const bool single,
    const std::string & rng, const std::string & convt ) {

  // more understandable error message:
  if (ch<1 || ch>HRDL_MAX_ANALOG_CHANNELS) throw Err()
   << "channel number out of range";
  if (ch%2==0 && !single) throw Err()
   << "only odd channels can be set for differential measurements";

  int32_t val;
  int16_t ovfl;

  // get value
  auto res = HRDLGetSingleValue(devh, ch, str_to_range(rng),
    str_to_convt(convt), (int16_t)single, &ovfl, &val);

  if (res==0) throw Err()
    << "can't get value from ADC: " << get_error();

  // Block mode channel configuration becomes invalid after
  // call to HRDLGetSingleValue. Update it.
  for (int n=1; n<=HRDL_MAX_ANALOG_CHANNELS; n++){
    ChConf_t C;
    if (ch == n){ // only one channel enabled
      C.en = true; C.rng = str_to_range(rng);
      C.sngl = single;
    }
    chconf[(int)ch] = C;
  }

  // get max integer value
  int32_t min,max;
  if (!HRDLGetMinMaxAdcCounts(devh,&min,&max,ch))
    throw Err() << "failed to get max ADC count: " << get_error();

  double v = (str_to_volt(rng)*val)/max;
  if (ovfl & (1<<ch)) v*=INFINITY;
  return v;
}

// Set four digital lines as outputs, set values
// according with the bitmask
void
ADC24::set_dig_out( const int bitmask){
  if (!HRDLSetDigitalIOChannel(devh, 0xF, bitmask & 0xF, 0))
    throw Err() << "can't set digital output: " << get_error();
}

// Block read mode:

// Set channel parameters.
void
ADC24::set_channel(int ch, bool enable,
                bool single, const std::string & rng){

  // more understandable error message:
  if (ch<1 || ch>HRDL_MAX_ANALOG_CHANNELS) throw Err()
   << "channel number out of range";
  if (ch%2==0 && !single) throw Err()
   << "only odd channels can be set for differential measurements";

  // convert values to ADC24 types.
  ChConf_t C;
  C.en   = enable; C.sngl = single;
  C.rng  = str_to_range(rng);

  // apply changes
  if (!(HRDLSetAnalogInChannel(devh,ch,C.en,C.rng,C.sngl)))
    throw Err() << "failed to set channel " << ch << ": " << get_error();

  // get max/min SDC count for this channel
  if (!HRDLGetMinMaxAdcCounts(devh,&C.min,&C.max,ch))
    throw Err() << "failed to get min/max ADC count: " << get_error();

  chconf[ch] = C; // save channel configuration
}

// Disable all channels
void
ADC24::disable_all(){
  for (int ch=1; ch<=HRDL_MAX_ANALOG_CHANNELS; ch++){
    ChConf_t C;
    if (!(HRDLSetAnalogInChannel(devh,ch,false,C.rng,true)))
      throw Err() << "failed to disable channel " << ch << ": " << get_error();
    chconf[(int)ch] = C;
  }
}

// Print channel settings.
void
ADC24::print_channel(int ch){
  if (chconf[ch].en) {
    cout << ch << " "
         << chconf[ch].en << " "
         << chconf[ch].sngl << " "
         << range_to_str(chconf[ch].rng) << "\n";
  }
  else cout << ch << " disabled\n";
}

// Set timing parameters.
void
ADC24::set_timing(int32_t dt, const std::string & convt){
  if (!HRDLSetInterval(devh,dt,str_to_convt(convt)))
    throw Err() << "failed to set timings: " << get_error();
  time_conf = true;
}

// Returns the number of channels enabled.
int
ADC24::chan_get_num(){
  int16_t n;
  if (HRDLGetNumberOfEnabledChannels(devh, &n)) return n;
    throw Err() << "can't get number of enabled channels: " << get_error();
  return n;
}

// Returns list of enabled channels.
std::vector<int>
ADC24::chan_get_list(){
  std::vector<int> ret;
  for (int i=0; i<chconf.size(); ++i)
    if (chconf[i].en) ret.push_back(i);

  // this should not happen:
  if (ret.size() != chan_get_num()) throw Err()
      << "Channel enable settings are not syncronised!";
  return ret;
}

// get the requested number of samples for each enabled channel
std::vector<double>
ADC24::get_block(int32_t nvals) {

  // check if timings are configured (or block mode will stuck!)
  if (!time_conf) throw Err() << "timings are not configured";

  if (!chan_get_num()) throw Err() << "channels are not configured";

  // run block mode for a ginev number of readings
  if (!HRDLRun(devh, nvals, HRDL_BM_BLOCK))
    throw Err() << "failed to run block mode: " << get_error();

  // wait until values are ready
  while (1) {
    usleep(1000);
    if (HRDLReady(devh)) break;
  }

  // get list of enabled channels, allocate data arrays
  auto ch_e = chan_get_list();
  size_t N = nvals*ch_e.size();
  std::vector<int32_t> ivals(N);
  std::vector<double>  vals(N);
  std::vector<int16_t> ovfl(nvals);

  // read data, (res - actual number of samples)
  auto res = HRDLGetValues(devh,ivals.data(),ovfl.data(),nvals);
  if (res==0) throw Err() << "can't get values from ADC: " << get_error();

  // convert int data to double
  for (int32_t nv = 0; nv<nvals; ++nv){
    for (size_t nc = 0; nc<ch_e.size(); ++nc){
      size_t ind = nv*ch_e.size() + nc;
      int ch = ch_e[nc];
      vals[ind] = range_to_volt(chconf[ch].rng)*ivals[ind]/chconf[ch].max;
      if (ovfl[nv] & (1<<ch)) vals[ind] *= INFINITY;
    }
  }
  return vals;
}
