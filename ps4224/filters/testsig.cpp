#include <cstdlib>
#include <iostream>
#include <cmath>
#include "../err.h"


// unpack data written by pico_rec into x-y text table

using namespace std;

void help(){
  cout << "pico_testsig -- write test signal\n";
}

int
main(int argc, char *argv[]){
  try {

    double N  = 500000;
    double dt = 1e-5;
    double f0 = 32680;
    double tau = 0.5;
    double amp = 0.954;
    double noise = 0.1;

    double fsw   = 0; // frequency switch
    double tsw   =  0.5;
    double tausw = 0.1;

    double sc = 1.0/(1<<14);

    cout << "  dt:       " << dt << "\n"
         << "  t0:       " << 0 << "\n"
         << "  data_num: " << 1 << "\n"
         << "  data: 0 A " << sc << " 0 \n"
         << "\n*\n";
    double phi = 0;
    for (int i = 0; i<N; i++){
      double t = i*dt;
      double f = f0 + fsw*tanh((t-tsw)/tausw);
      phi += 2*M_PI*f*dt;
      double y = amp*exp(-t/tau)*sin(phi) +
                 amp*noise*(2.0*random()/RAND_MAX-1.0);
      int16_t v = y/sc;
      cout.write((const char*)&v, sizeof(int16_t));
    }

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
