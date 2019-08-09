#include "adc.h"
#include "adc_pico24.h"
#include "err.h"
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

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "h");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'h':
          cout << "pico_adc24 -- record values using Pico ADC24\n"
                  "Usage: pico_adc24 [options]\n"
                  "Options:\n"
                  " -h        -- write this help message and exit\n";
          return 0;
      }
    }
    // open the device
    int16_t dev = HDRLOpenUnit();
    if (dev == 0) throw Err() << "Can't find the device";
    if (dev < 0)  throw Err() << "Device found but can not be opened";

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
