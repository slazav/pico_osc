#include "pico_4224.h"
#include "pico_cmd.h"
#include "err.h"
#include "m_getopt.h"
#include <unistd.h>

/* read data block from the oscilloscope */

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

  {"trig_src", 1,'T', OPT2, "trigger source: NONE,A,B (default NONE)\n"},
  {"trig_dir", 1,0,   OPT2, "trigger direction: ABOVE, BELOW, RISING, FALLING (default RISING)\n"},
  {"trig_lvl", 1,0,   OPT2, "trigger level, V (default 0)\n"},
  {"trig_del", 1,0,   OPT2, "trigger delay, s (default 0)\n"},

  {"dt",       1,'t',   OPT2, "time step, s (default 1e-3)\n"},
  {"nrec",     1,'n',   OPT2, "total number of points (default 1024)\n"},
  {"npre",     1,'p',   OPT2, "number of pretrigger points (default 0)\n"},

  {0,0,0,0}
};

// show help message and exit
void usage(bool pod=false){
  string head = pod? "\n=head1 ":"\n";
  const char * fname = "pico4224block";
  cerr << fname << " -- read data block from the Picoscope 4224\n"
       << head << "Usage:\n"
       << "\t"<< fname << " <options>\n";
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

    if (O["trig_src"]!="") pi.trig_src = O["trig_src"];
    if (O["trig_lvl"]!="") pi.trig_lvl = atof(O["trig_lvl"].c_str());
    if (O["trig_del"]!="") pi.trig_del = atof(O["trig_del"].c_str());
    if (O["trig_dir"]!="") pi.trig_dir = O["trig_dir"];

    if (O["dt"]!="")   pi.dt   = atof(O["dt"].c_str());
    if (O["nrec"]!="") pi.nrec = atoi(O["nrec"].c_str());
    if (O["npre"]!="") pi.npre = atoi(O["npre"].c_str());

    // record the signal
    OutPars po = record_block(osc, pi);

    // dump results
    for (int i = 0; i<po.bufa.size; i++){
      double x = po.t0 + po.dt*i;
      double y = po.sc_a * po.bufa.data[i];
      double z = po.sc_b * po.bufb.data[i];
      std::cout << x << "\t" << y << "\t" << z << "\n";
    }
    if (po.overflow_a) std::cerr << "Overload A\n";
    if (po.overflow_b) std::cerr << "Overload B\n";

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
