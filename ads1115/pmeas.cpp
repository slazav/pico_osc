#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

#include "err/err.h"

#include "ads1115_dev.h"

#include <sys/time.h> // gettimeofday
#include <cstring>

/*
Pressure measurements for f4rpi computer.
All parameters are hardcoded in the program.

4 channels are recorded into separate files (time, mean, uncertainty),
points are skipped if difference with a previous one is less
then 3*uncertainty.

*/

using namespace std;

/*********************************************************************/

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *path = "/dev/i2c-3"; // i2c bus
    uint8_t addr=0x48;            // default device address
    std::vector<std::string> chans = {"A", "B", "C", "D"};
    const char *range = "4.096";
    const char *rate  = "8";
    double delay = 0; // delay between measurements, s
    double dt = 600;  // max point distance, s
    size_t nmeas = 10; // number of measurements in each point (to get uncertainty)
    std::string fpref = "/root/press_log"; // data folder

    // open device
    ADS1115 dev(path, addr);

    // time, value, uncertainty for each channel
    std::vector<double> tp(chans.size(), HUGE_VAL);
    std::vector<double> vp(chans.size(), HUGE_VAL);
    std::vector<double> sp(chans.size(), HUGE_VAL);
    std::vector<double> tflush(chans.size(), 0);

    // open logfiles
    std::vector<std::ofstream> files;
    for (int i=0; i<chans.size(); i++){
      time_t ts = time(NULL);
      struct tm * t = localtime(&ts);
      std::ostringstream fname;
      fname << fpref << "/" << chans[i] << "_"
            << std::setfill('0') << std::setw(4) << t->tm_year+1900 << "-"
            << std::setw(2) << t->tm_mon+1 << "-"
            << std::setw(2) << t->tm_mday << "_"
            << std::setw(2) << t->tm_hour << ":"
            << std::setw(2) << t->tm_min << ":"
            << std::setw(2) << t->tm_sec;
      files.emplace_back(fname.str());
    }

    // main loop
    while(1){
      for (int i=0; i<chans.size(); i++){

        // do nmeas measurements
        // calculate time, mean value, uncertainty
        double t,v,s;
        dev.meas_n(chans[i],range,rate, nmeas, t,v,s);

        // write the point if it is far enough from prev.value
        // For normal distribution 1*s: 68%, 2*s: 95%, 3*s: 99.7%.
        if ( abs(t-tp[i]) > dt || abs(v-vp[i]) > 3*s) {
          tp[i] = t; vp[i] = v; sp[i] = s;
          files[i] << chans[i] << " " << fixed << setw(6)
                   << tp[i] << " " << vp[i] << " " << sp[i] << "\n";
        }

        // flush file
        if (abs(t-tflush[i]) > dt) {
          files[i].flush();
          tflush[i] = t;
        }

      }
      usleep(delay*1e6);
    }

  }
  catch (const Err & E){
    cerr << "Error: " << E.str() << "\n";
    return 1;
  }

}
