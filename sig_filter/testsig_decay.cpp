#include <cstdlib>
#include <iostream>
#include <cmath>
#include <getopt.h>
#include "err/err.h"


// unpack data written by pico_rec into x-y text table

using namespace std;

void help(){
  cout << "testsig -- create test signals with decaying oscillations\n"
          "Usage: testsig [options] > <file>\n"
          "Options:\n"
          " -N <num>  -- number of points (default: 100000)\n"
          " -D <num>  -- time step, s  (default: 1e-5)\n"
          " -F <num>  -- signal frequency, Hz (default: 32674)\n"
          " -T <num>  -- signal decay time, s (default: 0.325), 0 for non-decaying signal\n"
          " -A <num>  -- signal amplitude,  Vpp (default: 0.582) (full scale=1V)\n"
          " -n <num>  -- noise amplitude, Vpp (default: 0)\n"
          " -G <num>  -- frequency change, Hz (default: 0)\n"
          " -U <num>  -- frequency relaxation time, s (default: 0.112)\n"
          " -h        -- write this help message and exit\n";
}

int
main(int argc, char *argv[]){
  try {

    double N  = 100000;
    double dt = 1e-5;
    double f0 = 32674;
    double tau = 0.325;
    double amp = 0.582;
    double noise  = 0;

    double ftau = 0.112;
    double famp = 0;

    //double fsw   = 0; // frequency switch
    //double tsw   =  0.5;
    //double tausw = 0.1;

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hN:D:F:T:A:n:G:U:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'N': N   = atoi(optarg); break;
        case 'D': dt  = atof(optarg); break;
        case 'F': f0  = atof(optarg); break;
        case 'T': tau = atof(optarg); break;
        case 'A': amp = atof(optarg); break;
        case 'n': noise  = atof(optarg); break;
        case 'G': famp   = atof(optarg); break;
        case 'U': ftau   = atof(optarg); break;
        case 'h': help(); return 0;
      }
    }

    double max = amp+noise;
    double sc = max/(1<<15);

    cout << "*SIG001\n"
         << "  points:   " << N << "\n"
         << "  dt:       " << dt << "\n"
         << "  t0:       " << 0 << "\n"
         << "  chan: A "   << sc << " 0\n"
         << "*\n";
    double phi = 0;
    for (int i = 0; i<N; i++){
      double t = i*dt;
      double f = f0 + famp*exp(-t/ftau);
      double y = 0.5*amp*sin(phi);
      if (tau != 0) y *= exp(-t/tau);

      y += noise*(1.0*random()/RAND_MAX-0.5);
      phi += 2*M_PI*f*dt;

      int16_t v = y/sc;
      cout.write((const char*)&v, sizeof(int16_t));
    }

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
