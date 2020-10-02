#include <unistd.h>

#include "err/err.h"
#include "read_words/read_words.h"

#include "device.h"

/* SPP-compatable interface to ADC-24 device */

using namespace std;

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
      int c = getopt(argc, argv, "hm:d:l");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'd': dev = optarg; break;
        case 'l': cout << ADC24::dev_list(); return 0;

        case 'm':
          mainsHz=atoi(optarg);
          if (mainsHz != 50 && mainsHz != 60)  throw Err()
            << "incorrect mains freq. option: should be 50 or 60 Hz";
          break;
        case 'h':
          cout << "pico_adc -- record values using Pico ADC\n"
                  "Usage: pico_adc [options]\n"
                  "Options:\n"
                  " -l       -- list all connected devices and exit\n"
                  " -d <dev> -- device ID (autodetect by default)\n"
                  " -m <Hz>  -- set mains frequency\n"
                  " -h       -- write this help message and exit\n";
          return 0;
      }
    }

    // open the device
    ADC24 adc24(dev);
    // configure the mains noise rejection
    adc24.set_mains(mainsHz == 60);

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "Pico ADC24 device is opened. Type help to see command list.\n";
    cout << "#OK\n";
    while (1){
      try {
        auto args = read_words(cin);
        if (args.size()<1) break;
        adc24.cmd(args);
        cout << "#OK\n" << flush;
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
