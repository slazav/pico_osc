#include "pico_int.h"
#include "pico_4224.h"
#include "err.h"
#include "m_getopt.h"
#include <unistd.h>
#include <cstdlib> // exit

/* working with Pico oscilloscope in interactive mode */

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
  {0,0,0,0}
};

// show help message and exit
void usage(bool pod=false){
  string head = pod? "\n=head1 ":"\n";
  const char * fname = "pico4224block";
  cerr << fname << " -- read data block from the Picoscope 4224\n"
       << head << "Usage:\n"
       << "\t"<< fname << " [options] < <commands>\n";
  cerr << head << "Option:\n";
  print_options(options, OPT1, cerr, pod);
  exit(1);
}

// show device list and exit
void list(){
  cout << Pico4224::dev_list() << "\n";
  exit(1);
}

void ranges(const char *dev){
  Pico4224 osc(dev);
  cout << osc.chan_get_ranges("A") << "\n";
  exit(1);
}

/*********************************************************************/
// read command from stdin, remove comments,
// split into words
vector<string> read_cmd(){
  // read line, combine lines with \ at the end
  string line = "\\";
  while (cin.good() && line.rfind('\\')==line.size()-1){
    string l;
    getline(cin,l);
    line = line.substr(0,line.size()-1) + l;
    // remove comments
    size_t nc = line.find('#');
    if (nc!=string::npos) line = line.substr(0,nc);
  }

  // split into words
  istringstream ss(line);
  vector<string> words;
  while (!ss.eof()){
    string w; ss>>w;
    if (w!="") words.push_back(w);
  }
  return words;
}


/*********************************************************************/
int
main(int argc, char *argv[]){
  try {

    // parse options
    vector<string> non_opts;
    Options O = parse_options_all(&argc, &argv, options, OPT_ALL, non_opts);

    // help/list operations
    if (O["help"]!="") usage();
    if (O["pod"]!="") usage(true);
    if (O["list"]!="") list();

    // open the device
    Pico4224 osc(O["device"].c_str());

    cout << "Picoscope device is opened. Type help to see command list.\n";
    while (cin.good()){
      try { osc.cmd(read_cmd()); }
      catch (Err e){ cout << e.str() << "\n"; }
    }

  }
  catch (Err E){
    cerr << E.str() << "\n";
  }
}
