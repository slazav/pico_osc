#include "pico_int.h"
#include "pico_4224.h"
#include "err/err.h"
#include "read_words/read_words.h"
#include <unistd.h>

/* record signals using Picoscope */

using namespace std;

// show device list
void list(){
  cout << Pico4224::dev_list() << "\n";
}

/*********************************************************************/
int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *dev="";

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "hld:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
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

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "Picoscope device is opened. Type help to see command list.\n";
    cout << "#OK\n";
    while (1){
      try {
        if (cin.eof()) break;
        if (osc.cmd(read_words(cin))) cout << "#OK\n" << flush;
      }
      catch (Err E){
        // during waiting errors go to the block_err buffer
        if (!osc.is_waiting())
          cout << "#Error: " << E.str() << "\n" << flush;
        else{
          osc.set_block_err(E.str());
        }
      }
    }

  }
  catch (Err E){
    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "#Error: " << E.str() << "\n";
    return 1;
  }

}
