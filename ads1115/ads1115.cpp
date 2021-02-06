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

/*********************************************************************/

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *path = "/dev/i2c-0"; // i2c bus
    const char *chan  = "AB";
    const char *range = "2.048";
    const char *rate  = "8";
    uint8_t addr=0x48;            // default device address
    bool single = false;

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "hd:a:c:v:r:S");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'd': path  = optarg; break;
        case 'c': chan  = optarg; break;
        case 'v': range = optarg; break;
        case 'r': rate  = optarg; break;
        case 'a': addr = atoi(optarg);
                  if (addr==0) throw Err() << "bad address: " << optarg << "\n";
                  break;
        case 'S': single = true; break;
        case 'h':
          cout << "ads1115 -- SPP interface to ADS1113/1114/1115 ADC converters\n"
                  "Usage: ads1115 [options]\n"
                  "Options:\n"
                  " -d <dev>  -- I2C device path (default /dev/i2c-0)\n"
                  " -a <addr> -- I2C address (default 0x48)\n"
                  " -c <chan> -- change default channel setting (default AB)\n"
                  " -v <chan> -- change default range setting (default 2.048)\n"
                  " -r <chan> -- change default rate setting (default 8)\n"
                  " -S        -- do a single measurement and print result (without SPP interface)\n";
                  " -h        -- write this help message and exit\n";
          return 0;
      }
    }

    // open device
    ADS1115 dev(path, addr);

    if (single){
      std::cout << dev.meas(chan,range,rate) << "\n";
      return 0;
    }

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "Using " << path << ":0x" << hex << addr << " as a ADS1113/1114/1115 device.\n";
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
            struct timeval tv;
            gettimeofday(&tv, NULL);
            cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec << "\n";
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
            std::cout << dev.meas(c,v,r) << "\n";
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
