#include "pico_4224.h"
#include "pico_cmd.h"
#include "err.h"
#include "m_getopt.h"
#include <unistd.h>

/* stream data to stdout, no trigger */

#define OPT1  1  //
#define OPT2  2  //
#define OPT3  4  //
#define OPT4  8  //
#define OPT_ALL  (OPT1 | OPT2 | OPT3 | OPT4)

using namespace std;

static struct ext_option options[] = {
  {"help",       0,'h', OPT1, "show this message and exit"},
  {"pod",        0,  0, OPT1, "show help as POD template"},
  {"list",       0,'l', OPT1, "list avalable devices and exit"},
  {"device",     1,'d', OPT1, "device ID (autodetect by default)\n"},
  {"ranges",     1,'r', OPT1, "show avalable inpit ranges and exit\n"},

  {"use_a",    1,'A', OPT2, "use channel: 1,0 (default 1)\n"},
  {"use_b",    1,'B', OPT2, "use channel: 1,0 (default 1)\n"},
  {"rng_a",    1,0,   OPT2, "voltage range for channel A (default 1V)\n"},
  {"rng_b",    1,0,   OPT2, "voltage range for channel B (default 1V)\n"},
  {"cpl_a",    1,0,   OPT2, "coupling for channel A: DC, AC (default DC)\n"},
  {"cpl_b",    1,0,   OPT2, "coupling for channel B: DC, AC (default DC)\n"},

  {"dt",       1,'t',   OPT2, "time step, s (default 1e-3)\n"},
  {"tbuf",     1,'n',   OPT2, "bufsize (default 0.1s)\n"},

  {0,0,0,0}
};

// show help message and exit
void usage(bool pod=false){
  string head = pod? "\n=head1 ":"\n";
  const char * fname = "pico4224block";
  cerr << fname << " -- read data block from the Picoscope 4224\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options>\n";
  cerr << head << "Output format:\n"
       << "Data is written to stdout, first a few metadata lines in\n"
       << "a form <name>: <value>, then an empty line followed by\n"
       << "int16_t array. If use_a=1 and use_b=1 then array contains\n"
       << "values for both channels.\n";
  cerr << head << "General options:\n";
  print_options(options, OPT1, cerr, pod);
  cerr << head << "Measurement parameters:\n";
  print_options(options, OPT2, cerr, pod);
  exit(1);
}

// show device list and exit
void list(){
  std::cout << Pico4224::dev_list() << "\n";
  exit(1);
}

void ranges(const char *dev){
  Pico4224 osc(dev);
  std::cout << osc.chan_get_ranges("A") << "\n";
  exit(1);
}

int
main(int argc, char *argv[]){
  try {

    // parse options
    if (argc==1) usage();
    vector<string> non_opts;
    Options O = parse_options_all(&argc, &argv, options, OPT_ALL, non_opts);

    // help/list operations
    if (O["help"]!="") usage();
    if (O["pod"]!="") usage(true);
    if (O["list"]!="") list();
    if (O["ranges"]!="") ranges(O["device"].c_str());

    // open the device
    Pico4224 osc(O["device"].c_str());

    // input parameters for the measurement
    InPars pi;
    if (O["use_a"]!="") pi.use_a = atoi(O["use_a"].c_str());
    if (O["use_b"]!="") pi.use_b = atoi(O["use_b"].c_str());
    if (O["rng_a"]!="") pi.rng_a = atof(O["rng_a"].c_str());
    if (O["rng_b"]!="") pi.rng_b = atof(O["rng_b"].c_str());
    if (O["cpl_a"]!="") pi.cpl_a = O["cpl_a"];
    if (O["cpl_b"]!="") pi.cpl_b = O["cpl_b"];

    if (O["dt"]!="")   pi.dt   = atof(O["dt"].c_str());
    if (O["tbuf"]!="") pi.tbuf = atoi(O["tbuf"].c_str());

    // record the signal
    stream(osc, pi);

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
