#include "pico_int.h"
#include "pico_4224.h"
#include "err.h"
#include <unistd.h>

/* record signals using Picoscope */

using namespace std;

// show device list
void list(){
  cout << Pico4224::dev_list() << "\n";
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

    /* default values */
    const char *dev="";

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hld:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'd': dev = optarg; break;
        case 'l': list(); return 0;
        case 'h':
          cout << "pico_rec -- record signals using Picoscope\n"
                  "Usage: pico_rec [options]\n"
                  "Options:\n"
                  " -d <dev>  -- device ID (autodetect by default)\n"
                  " -l        -- list all connected devices and exit\n"
                  " -h        -- write this help message and exit\n";
          return 0;
      }
    }

    // open the device
    Pico4224 osc(dev);

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
