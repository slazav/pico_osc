#include "picoadc_int.h"
#include "pico_adc24.h"
#include "err/err.h"
#include <unistd.h>

/* SPP-compatable interface to ADC-24 device */

using namespace std;

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
    int16_t mainsHz;

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "hm:d:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'd': dev = optarg; break;
        case 'm':
          mainsHz=atoi(optarg);
          if (mainsHz != 50 && mainsHz != 60)  throw Err()
            << "incorrect mains freq. option: should be 50 or 60 Hz";
          break;
        case 'h':
          cout << "pico_adc -- record values using Pico ADC\n"
                  "Usage: pico_adc [options]\n"
                  "Options:\n"
                  " -d <dev> -- device ID (autodetect by default)\n"
                  " -m <Hz>  -- set mains frequency\n"
                  " -h       -- write this help message and exit\n";
          return 0;
      }
    }

    // open the device
    ADC24 adc24(dev,mainsHz);

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "Pico ADC24 device is opened. Type help to see command list.\n";
    cout << "#OK\n";
    while (1){
      try {
        if (cin.eof()) break;
        if (adc24.cmd(read_cmd())) cout << "#OK\n" << flush;
      }
      catch (Err E){ cout << "#Error: " << E.str() << "\n" << flush; }
    }

  }
  catch (Err E){
    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cerr << "#Error: " << E.str() << "\n";
    return 1;
  }

}
