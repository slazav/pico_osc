#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include <cmath>
#include "picoadc_int.h"
#include "err/err.h"

#define VERSION "1.0"

using namespace std;

bool
ADCInt::is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0; }

ADCInt::ADCInt() {
  h = HRDLOpenUnit();
  if (h == 0) throw Err() << "Can't find the device";
  if (h < 0)  throw Err() << "Device found but can not be opened";
}

ADCInt::~ADCInt() {
  HRDLCloseUnit(h);
}

const char *
ADCInt::cmd_help() const {
  return
  "help -- show command list\n"
  "*idn? -- write id string: \"pico_adc " VERSION "\"\n";
  ;
}

bool
ADCInt::cmd(const vector<string> & args){
  if (args.size()<1) return false;


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

  throw Err() << "Unknown command: " << args[0];
}
