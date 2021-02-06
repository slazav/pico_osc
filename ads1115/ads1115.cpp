#include <unistd.h>
#include <iostream>
#include <iomanip>

#include "err/err.h"
#include "read_words/read_words.h"

#include "ads1115_dev.h"

#include <sys/time.h> // gettimeofday
#include <cstring>

#define VERSION "1.0"

/* SPP interface to ADS1113/1114/1115 ADC converters (i2c connection) */

using namespace std;

/*********************************************************************/

// compare strings
bool
is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0;
}

void
print_tstamp(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec;
}

/*********************************************************************/

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *path = "/dev/i2c-0"; // i2c bus
    const char *chan  = "AB";
    const char *range = "2.048";
    const char *rate  = "8";
    const char *mode  = "spp";
    uint8_t addr=0x48;            // default device address
    int delay = 1000000;          // delay in cont mode [us]

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "hd:a:c:v:r:m:t:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'd': path  = optarg; break;
        case 'c': chan  = optarg; break;
        case 'v': range = optarg; break;
        case 'r': rate  = optarg; break;
        case 'm': mode = optarg; break;
        case 'a': addr = atoi(optarg);
                  if (addr==0) throw Err() << "bad address: " << optarg << "\n";
                  break;
        case 't': delay = atof(optarg)*1e6;
                  if (delay<0) throw Err() << "bad delay: " << optarg << "\n";
                  break;
        case 'h':
          cout << "ads1115 -- SPP interface to ADS1113/1114/1115 ADC converters\n"
                  "Usage: ads1115 [options]\n"
                  "Options:\n"
                  " -d <dev>  -- I2C device path (default /dev/i2c-0)\n"
                  " -a <addr> -- I2C address (default 0x48)\n"
                  " -c <chan> -- change default channel setting (default AB)\n"
                  " -v <chan> -- change default range setting (default 2.048)\n"
                  " -r <chan> -- change default rate setting (default 8)\n"
                  " -m <mode> -- program mode: spp, single, info, cont (default: spp)\n"
                  " -d <time> -- delay in cont mode [s] (default: 1.0)\n"
                  " -h        -- write this help message and exit\n"
          ;
          return 0;
      }
    }

    // open device
    ADS1115 dev(path, addr);

    // single measurement mode, measure and print one value
    if (strcasecmp(mode, "single") == 0){
      std::cout << fixed << setw(6) << dev.meas(chan,range,rate) << "\n";
      return 0;
    }

    // conf mode, print device configuration
    if (strcasecmp(mode, "info") == 0){
      dev.print_info();
      return 0;
    }

    // cont mode: measure and print values with timestamps
    if (strcasecmp(mode, "cont") == 0){
      while(1){
        print_tstamp();
        std::cout << "\t" << fixed << setw(6) << dev.meas(chan,range,rate) << "\n";
        usleep(delay);
      }
      return 0;
    }

    if (strcasecmp(mode, "spp") != 0)
      throw Err() << "unknown mode: " << mode;

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "Using " << path << ":0x" << hex << (int)addr << " as a ADS1113/1114/1115 device.\n";
    cout << "Type help to see command list.\n";
    cout << "#OK\n";

    while (1){
      try {
        auto args = read_words(cin);
        if (args.size()<1) break;

        do {
          // print help
          if (is_cmd(args, "help")){
            if (args.size()!=1) throw Err() << "Usage: help";
            cout <<
              "help  -- Get list of commands.\n"
              "get_time -- Get current time.\n"
              "*idn? -- Get ID string: \"ads1115 " VERSION "\".\n"
              "get_conf -- Get device configuration.\n"
              "get [(A|B|C|D|AB|AD|BD|CD) [<range> [<rate>]]] -- do a single measurement.\n"
              " Arguments (if skipped, use default values):\n"
              " - Channels A..D, double letters for differential mode.\n"
              " - Range: 6.144, 4.096, 2.048, 1.024, 0.512, 0.256 [V],\n"
              "   bi-directional in differential mode.\n"
              " - Rate: 8,16,32,64,128,250,475,860 [1/s].\n"
            ;
            break;
          }

          // print time
          if (is_cmd(args, "get_time")){
            if (args.size()!=1) throw Err() << "Usage: get_time";
            print_tstamp();
            cout << "\n";
            break;
          }

          // print id
          if (is_cmd(args, "*idn?")){
            if (args.size()!=1) throw Err() << "Usage: *idn?";
            cout << "ads1115 " VERSION "\n";
            break;
          }

          // print device configuration
          if (is_cmd(args, "get_conf")){
            if (args.size()!=1) throw Err() << "Usage: get_conf";
            dev.print_info();
            break;
          }

          // do a measurement
          if (is_cmd(args, "get")){
            if (args.size()<1 || args.size()>4) throw Err()
              << "Usage: get [<chan> [<range> [<rate>]]]";
            std::string c = args.size()>1? args[1]:chan;
            std::string v = args.size()>2? args[2]:range;
            std::string r = args.size()>3? args[3]:rate;
            std::cout << fixed << setw(6) << dev.meas(c,v,r) << "\n";
            break;
          }

        } while(0);
        cout << "#OK\n" << flush;
      }
      catch (Err E){ cout << "\n#Error: " << E.str() << "\n" << flush; }
    }

    return 0;
  }
  catch (Err E){
    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cerr << "#Error: " << E.str() << "\n";
    return 1;
  }

}
