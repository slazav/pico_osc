#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <memory>

#include "err/err.h"
#include "iofilter/iofilter.h"

#include "ads1115_dev.h"

#include <sys/time.h> // gettimeofday
#include <cstring>

/* Pressure measurements for f4rpi computer */

using namespace std;

/*********************************************************************/

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *path = "/dev/i2c-3"; // i2c bus
    uint8_t addr1=0x48; // 1st card
    uint8_t addr2=0x49; // 2nd card

    std::vector<std::string> chans = {"A", "B", "C", "D", "E"};

    const char *range = "4.096";
    const char *rate  = "8";
    double delay = 1; // delay between measurements, s
    std::string fpref = "/root/press_log"; // data folder

    // wait until time will be set (in RPi clock is set via network)
    while (1){
      time_t ts = time(NULL);
      if (ts > 1e9) break;
      std::cerr << "Waiting for correct clock setting...\n";
      sleep(10);
    }

    // open device
    ADS1115 dev1(path, addr1);
    ADS1115 dev2(path, addr2);

    // open database
    OFilter db("graphene -i -d /root/db | grep '#Error'");

    // open logfiles
    std::ofstream full_log;
    {
      time_t ts = time(NULL);
      struct tm * t = localtime(&ts);
      std::ostringstream fname;
      fname << fpref << "/"
          << std::setfill('0') << std::setw(4) << t->tm_year+1900 << "-"
          << std::setw(2) << t->tm_mon+1 << "-"
          << std::setw(2) << t->tm_mday << "_"
          << std::setw(2) << t->tm_hour << ":"
          << std::setw(2) << t->tm_min << ":"
          << std::setw(2) << t->tm_sec << "_full";
      full_log = std::ofstream(fname.str());
    }

    // main loop
    while(1){

      full_log << fixed << setw(6) << dev1.tstamp();
      for (int i=0; i<chans.size(); i++){
        auto ch = chans[i];
        double v = 0;
        if (ch.size()==0) continue;

        if (ch[0] <= 'D'){
          v = dev1.meas(ch,range,rate);
        }
        else {
          ch[0]-=4;
          v = dev2.meas(ch,range,rate);
        }
        // voltage dividers 10k/(10k+20k) on the board
        v*=3;
        db.stream() << "put_flt pumps_p" << i+1 << " " << fixed << setw(6) << dev1.tstamp() << " " << v << "\n";
        full_log << " " << v;
      }
      full_log << "\n";
      full_log.flush();
      db.stream().flush();
      usleep(delay*1e6);
    }

  }
  catch (const Err & E){
    cerr << "Error: " << E.str() << "\n";
    return 1;
  }

}
