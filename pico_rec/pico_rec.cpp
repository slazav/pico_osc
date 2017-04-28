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
  // read line
  string line;
  getline(cin,line);

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
    cout << "#OK\n";
    while (1){
      try {
        osc.cmd(read_cmd());
        if (!cin.eof()) cout << "#OK\n" << flush;
        else break;
      }
      catch (Err E){ cout << "#Error: " << E.str() << "\n" << flush; }
    }

  }
  catch (Err E){
    cerr << "#Error: " << E.str() << "\n";
    return 1;
  }

}
