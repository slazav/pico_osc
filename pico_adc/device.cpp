#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include <cmath>
#include <bitset>
#include <iomanip>    // setfill
#include <sys/time.h> // gettimeofday
#include "err/err.h"

#include "device.h"

#define VERSION "1.0"

using namespace std;

bool
ADCInt::is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0;
}

const char *
ADCInt::cmd_help() const {
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
  "get_info -- print unit info\n"
  "get -- collect and return a single data block.\n"
  "       chan_set and set_t should be done before.\n"
  "get_val <chan> <rng> <convt> <single> -- get a single value.\n";
}

bool
ADCInt::cmd(const vector<string> & args){
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

  // set time intervals
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
      ChConf C;
      C.en  = atoi(args[2].c_str());
      C.sngl = atoi(args[3].c_str());;
      C.rng = atof(args[4].c_str());
      chan_set(chi, C.en, C.sngl, C.rng);
      chconf[chi] = C; // save channel configuration
      chconf_en[chi] = atoi(args[2].c_str());
      chconf_sngl[chi] = atoi(args[3].c_str());
      chconf_rng[chi] = atof(args[4].c_str());
      chconf_max[chi] = get_max(chi);
    }
    return true;
  }

  // set channel parameters
  if (is_cmd(args, "chan_get")) {
    if (args.size()!=2) throw Err() << "Usage: chan_get <ch>";
    int ch = atoi(args[1].c_str());
    if (chconf_en[ch]) cout << ch << " "
          << chconf_en[ch] << " "
          << chconf_sngl[ch] << " "
          << chconf_rng[ch] << "\n";
    else cout << ch << " disabled\n";

    return true;
  }

  // get
  if (is_cmd(args, "get")) {
    if (args.size()!=1) throw Err()
      << "Usage: get";
    run_block(1);
    usleep(1000);
    while (!is_ready()) {usleep(1000);}
    int16_t overflow;
    float *vals = get_values(1,&overflow);
    for (int i=0; i<chan_get_num(); ++i) {
      if (i>0) std::cout << " ";
      std::cout << vals[i];
    }
    std::cout <<"\n";
    return true;
  }

  // get_val <chan> <rng> <convt> <single> -- get a single value.
  if (is_cmd(args, "get_val")) {
    if (args.size()!=5) throw Err()
      << "Usage: get_val <chan> <rng> <convt> <single>";

    int16_t ch    = atoi(args[1].c_str());
    float   rng   = atof(args[2].c_str());
    int16_t convt = atoi(args[3].c_str());
    int16_t sngl  = atoi(args[4].c_str());;

    bool overflow;
    auto v = get_single_value(ch,rng,convt,sngl,overflow);
    if (overflow) throw Err() << "overflow";
    std::cout << v << "\n";
    return true;
  }

  throw Err() << "Unknown command: " << args[0];
}