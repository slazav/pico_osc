#include "commands.h"
#include "err/err.h"

#include <iomanip>
#include <cstring>
#include <sys/time.h> // gettimeofday


using namespace std;

// compare strings
bool
is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0;
}

// print command list
const char *
cmd_help() {
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
cmd(ADC24 & dev, const vector<string> & args){
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
    cout << dev.get_info() <<  "\n";
    return;
  }

  // show available range settings
  if (is_cmd(args, "ranges")){
    if (args.size()!=1) throw Err() << "Usage: ranges";
    cout << dev.get_ranges() <<  "\n";
    return;
  }

  // show available time conversion settings
  if (is_cmd(args, "tconvs")){
    if (args.size()!=1) throw Err() << "Usage: tconvs";
    cout << dev.get_tconvs() <<  "\n";
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

    auto v = dev.get_single(ch, sngl, rng, convt);
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
    dev.set_dig_out(v);
    return;
  }

  // Block read commands:

  // set channel parameters for block mode
  if (is_cmd(args, "chan_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: chan_set <ch> <en> <sngl> <rng>";
    // set multiple channels
    int8_t len = args[1].length();

    if (len!=1 && len%2) throw Err() << "bad channel option";
    for (int i=0; i<len; i+=2){
      // channel as a string
      string sch = (len==1)? args[1]: string(&args[1][i], &args[1][i+2]);
      auto ch   = str_to_type<int>(sch);
      auto en   = str_to_type<bool>(args[2]);
      auto sngl = str_to_type<bool>(args[3]);
      auto & rng = args[4];
      dev.set_channel(ch, en, sngl, rng);
    }
    return;
  }

  // disable all channels
  if (is_cmd(args, "disable_all")) {
    if (args.size()!=1) throw Err() << "Usage: disable_all";
    dev.disable_all();
    return;
  }

  // print number of active channels for block mode
  if (is_cmd(args, "chan_get_n")){
    if (args.size()!=1) throw Err() << "Usage: get_chan_n";
    cout << dev.chan_get_num() << "\n";
    return;
  }

  // set timings for block mode
  if (is_cmd(args, "set_t")){
    if (args.size()!=3) throw Err() << "Usage: set_t <dt> <tconv>";
    dev.set_timing(str_to_type<int>(args[1]), args[2]);
    return;
  }

  // get values in block mode
  if (is_cmd(args, "get")) {
    if (args.size()!=1) throw Err()
      << "Usage: get";

    int16_t nvals = 1;
    auto v = dev.get_block(nvals);

    auto nchan = dev.chan_get_num();
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
