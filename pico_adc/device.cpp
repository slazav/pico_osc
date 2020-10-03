#include <cstring> // strcasecmp
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
  "get_time -- Get current time.\n"
  "*idn? -- Get ID string: \"pico_adc " VERSION "\".\n"
  "help  -- Get list of commands.\n"
  "get_info -- Get device information.\n"
  "ranges   -- Get available range settings.\n"
  "tconvs   -- Get available conversion time settings.\n"
  "get_val <chan> <single> <rng> <convt> -- Measure a single value.\n"
  "set_dig_out <v1> <v2> <v3> <v4> -- Set digital outputs.\n"
  "\n"
  "  Below are commands for 'block read mode'.\n"
  "  It's not recommended to use them.\n"
  "\n"
  "chan_set <chs> <en> <sngl> <rng> -- set channel parameters\n"
  "   chs  -- Channels: 01 to 16, multiple channels allowed\n"
  "   en   -- Enable channels: 1 or 0\n"
  "   sngl -- Single/Differential mode: 1 or 0.\n"
  "   rng  -- Input range, mV (see ranges command).\n"
  "chan_get <ch>  -- get channel parameters (<ch> <en> <sngl> <rng>,\n"
  "   or '<ch> disabled' for disabled channels.\n"
  "chan_get_n  -- Get number of enabled analog channels.\n"
  "set_t <dt> <tconv> -- Set timings.\n"
  "   dt    -- time step between measurements\n"
  "   tconv -- conversion time for one channel (see tconvs command)\n"
  "get -- collect and return a single data block.\n"
  "       chan_set and set_t should be done before.\n"
  ;
}

void
ADC24::cmd(const vector<string> & args){
  if (args.size()<1) return; // should not happen

  // print time
  if (is_cmd(args, "get_time")){
    if (args.size()!=1) throw Err() << "Usage: get_time";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec << "\n";
    return;
  }

  // print id
  if (is_cmd(args, "*idn?")){
    if (args.size()!=1) throw Err() << "Usage: *idn?";
    cout << "pico_adc " VERSION "\n";
    return;
  }

  // print help
  if (is_cmd(args, "help")){
    if (args.size()!=1) throw Err() << "Usage: help";
    cout << cmd_help();
    return;
  }

  // print device info
  if (is_cmd(args, "get_info")){
    if (args.size()!=1) throw Err() << "Usage: get_info";
    cout << get_info() <<  "\n";
    return;
  }

  // show available range settings
  if (is_cmd(args, "ranges")){
    if (args.size()!=1) throw Err() << "Usage: ranges";
    cout << get_ranges() <<  "\n";
    return;
  }

  // show available time conversion settings
  if (is_cmd(args, "tconvs")){
    if (args.size()!=1) throw Err() << "Usage: tconvs";
    cout << get_tconvs() <<  "\n";
    return;
  }

  // get a single value.
  if (is_cmd(args, "get_val")) {
    if (args.size()!=5) throw Err()
      << "Usage: get_val <chan> <single> <rng> <convt>";

    int ch  = str_to_type<int>(args[1]);
    bool sngl = str_to_type<bool>(args[2]);
    auto & rng   = args[3];
    auto & convt = args[4];

    auto v = get_single(ch, sngl, rng, convt);
    std::cout << v << "\n";
    return;
  }

  // set digital output.
  if (is_cmd(args, "set_dig_out")) {
    if (args.size()!=5) throw Err()
      << "Usage: get_val <v1> <v2> <v3> <v4>";
    int v = 0;
    for (int i=0; i<4; i++)
      v = (v<<1) | (str_to_type<bool>(args[i+1])? 1:0);
    set_dig_out(v);
    return;
  }

  // Block read commands:

  // set channel parameters for block mode
  if (is_cmd(args, "chan_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: chan_set <ch> <en> <sngl> <rng>";
    // set multiple channels
    int8_t len = args[1].length();
    if (len%2) throw Err() << "bad channel option";
    for (int i=0; i<args[1].length(); i+=2){
      string sch(&args[1][i], &args[1][i+2]); // channel as a string
      auto ch   = str_to_type<int>(sch);
      auto en   = str_to_type<bool>(args[2]);
      auto sngl = str_to_type<bool>(args[3]);
      auto & rng = args[4];
      set_channel(ch, en, sngl, rng);
    }
    return;
  }

  // disable all channels
  if (is_cmd(args, "disable_all")) {
    if (args.size()!=1) throw Err() << "Usage: disable_all";
    disable_all();
    return;
  }

  // print number of active channels for block mode
  if (is_cmd(args, "chan_get_n")){
    if (args.size()!=1) throw Err() << "Usage: get_chan_n";
    cout << chan_get_num() << "\n";
    return;
  }

  // set timings for block mode
  if (is_cmd(args, "set_t")){
    if (args.size()!=3) throw Err() << "Usage: set_t <dt> <tconv>";
    set_timing(str_to_type<int>(args[1]), args[2]);
    return;
  }

  // get values in block mode
  if (is_cmd(args, "get")) {
    if (args.size()!=1) throw Err()
      << "Usage: get";

    int16_t nvals = 1;
    auto v = get_block(nvals);

    auto nchan = chan_get_num();
    if (nvals*nchan != v.size())
     throw Err() << "get_block() returned wrong number of samples";

    for (size_t nv = 0; nv<nvals; ++nv){
      for (size_t nc = 0; nc<nchan; ++nc){
         size_t ind = nv*nchan + nc;
         if (nc>0) std::cout << " ";
         std::cout << v[ind];
      }
      std::cout << "\n";
    }
    return;
  }


  throw Err() << "Unknown command: " << args[0];
}

/**********************************************************/
// return newline-separated list of all connected devices
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
std::string
ADC24::dev_list(){
  std::string ret;
  int16_t devices[HRDL_MAX_PICO_UNITS];

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
  for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
    if (devices[i] > 0) HRDLCloseUnit(devices[i]);
  }
  return ret;
}

// Constructor and destructor: open/close device, throw errors if any.
// See code in https://github.com/picotech/picosdk-c-examples/blob/master/picohrdl/picohrdlCon/picohrdlCon.c
// for their strange way of finding correct device...
ADC24::ADC24(const std::string & name){

  // open all devices, as in dev_list()
  int16_t devices[HRDL_MAX_PICO_UNITS];
  devh=-1; devn=-1;

  // initialize device array
  for (int i = 0; i < HRDL_MAX_UNITS; i++) devices[i] = 0;

  // open all devices, as in dev_list(), find which one to use
  for (int i = 0; i < HRDL_MAX_UNITS; i++) {
    devices[i] = HRDLOpenUnit();
    if (devices[i] > 0) {
      auto line = get_unit_info(devices[i], HRDL_BATCH_AND_SERIAL);
      if (name=="" || strcasecmp(name.c_str(), line.c_str())==0){
        devh = devices[i]; devn = i; break;
      }
    }
  }

  // close all devices except one
  for (int i = 0; i < HRDL_MAX_PICO_UNITS; i++) {
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
  if (!HRDLGetUnitInfo(h,(int8_t *)str.data(),str.size(),info))
    throw Err() << "failed to get unit info";
  return str;
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
  if (ne != 0) return serr_to_str(ne);
  return "no erorrs";
}

// configures the mains noise rejection setting
void
ADC24::set_mains(bool m60Hz) {
  if (!HRDLSetMains(devh,m60Hz))
    throw Err() << "failed to set mains: " << get_error();
}

// get device information
std::string
ADC24::get_info(){
  const int16_t ui_buflen=1024;
  char ui_v[ui_buflen],ui_n[ui_buflen];
  auto var = get_unit_info(devh, HRDL_VARIANT_INFO);
  auto ser = get_unit_info(devh, HRDL_BATCH_AND_SERIAL);
  return var + " " + ser;
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
  if (ch%2==0 && !single) throw Err()
   << "only odd channels can be set for differential measurements";

  int32_t val;
  int16_t ovfl;

  // get value
  auto res = HRDLGetSingleValue(devh, int_to_ch(ch), str_to_range(rng),
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

  if (ovfl & (1<<ch)) return std::nan("");
  return (str_to_volt(rng)*val)/max;
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
ADC24::set_channel(int chan, bool enable,
                bool single, const std::string & rng){

  // more understandable error message:
  if (chan%2==0 && !single) throw Err()
   << "only odd channels can be set for differential measurements";

  // convert values to ADC24 types.
  ChConf_t C;
  auto ch = int_to_ch(chan);
  C.en   = enable; C.sngl = single;
  C.rng  = str_to_range(rng);

  // apply changes
  if (!(HRDLSetAnalogInChannel(devh,ch,C.en,C.rng,C.sngl)))
    throw Err() << "failed to set channel " << ch << ": " << get_error();

  // get max/min SDC count for this channel
  if (!HRDLGetMinMaxAdcCounts(devh,&C.min,&C.max,ch))
    throw Err() << "failed to get min/max ADC count: " << get_error();

  chconf[(int)ch] = C; // save channel configuration
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
ADC24::print_channel(int chan){
  auto ch = int_to_ch(chan);
  if (chconf[(int)ch].en) {
    cout << ch << " "
         << chconf[ch].en << " "
         << chconf[ch].sngl << " "
         << range_to_str(chconf[ch].rng) << "\n";
  }
  else cout << ch << " disabled\n";
}

// Set timing parameters.
// - dt is measurement time [ms]. If N is number of enabled channels
//   then dt should be: N*conv_t < dt <= 1000*N*conv_t.
//   For a single measurement set this to the smallest possible value.
// - conv_t: ADC conversion time for a single value [ms].
//   Possible values: 60 100 180 340 660.
//   Use longer times for more accurate measurements.
void
ADC24::set_timing(int32_t dt, const std::string & convt){
  if (!HRDLSetInterval(devh,dt,str_to_convt(convt)))
    throw Err() << "failed to set timings: " << get_error();
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
      if (ovfl[nv] & (1<<ch)) vals[ind] = std::nan("");
    }
  }
  return vals;
}
