#include <cstdlib>
#include <iostream>
#include <cmath>
#include <getopt.h>
#include "err/err.h"


using namespace std;

void help(){
  cout << "testsig_2decay -- create test signals with two decaying oscillations\n"
          "Usage: testsig [options] > <file>\n"
          "Options:\n"
          " -N <num>  -- number of points (default: 100000)\n"
          " -D <num>  -- time step, s  (default: 1e-5)\n"
          " -F <num>  -- signal-1 frequency, Hz (default: 32674)\n"
          " -G <num>  -- signal-2 frequency, Hz (default: 32674)\n"
          " -T <num>  -- signal-1 decay time, s (default: 0.325), 0 for non-decaying signal\n"
          " -U <num>  -- signal-2 decay time, s (default: 0.325), 0 for non-decaying signal\n"
          " -A <num>  -- signal-1 amplitude,  V (default: 0.582) (full scale=1V)\n"
          " -B <num>  -- signal-2 amplitude,  V (default: 0.582) (full scale=1V)\n"
          " -n <num>  -- noise amplitude, V (default: 0)\n"
          " -h        -- write this help message and exit\n";
}

int
main(int argc, char *argv[]){
  try {

    double N  = 100000;
    double dt = 1e-5;

    double fre1 = 32674;
    double tau1 = 0.325;
    double amp1 = 0.182;

    double fre2 = 31234;
    double tau2 = 0.234;
    double amp2 = 0.123;

    double noise  = 0;

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hN:D:F:T:A:n:G:U:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'N': N   = atoi(optarg); break;
        case 'D': dt  = atof(optarg); break;
        case 'F': fre1 = atof(optarg); break;
        case 'G': fre2 = atof(optarg); break;
        case 'T': tau1 = atof(optarg); break;
        case 'U': tau2 = atof(optarg); break;
        case 'A': amp1 = atof(optarg); break;
        case 'B': amp2 = atof(optarg); break;
        case 'n': noise  = atof(optarg); break;
        case 'h': help(); return 0;
      }
    }

    double max = amp1 + amp2 + noise;
    double sc = max/(1<<15);

    cout << "*SIG001\n"
         << "  points:   " << N << "\n"
         << "  dt:       " << dt << "\n"
         << "  t0:       " << 0 << "\n"
         << "  chan: A "   << sc << " 0\n"
         << "# test signal parameters:\n"
         << "  fre1:  "   << fre1 << " 0\n"
         << "  tau1:  "   << tau1 << "\n"
         << "  amp1:  "   << amp1 << "\n"
         << "  fre2:  "   << fre2 << "\n"
         << "  tau2:  "   << tau2 << "\n"
         << "  amp2:  "   << amp2 << "\n"
         << "  noise: "   << noise << "\n"
         << "*\n";
    double phi1  = 0;
    double phi2 = 0;
    for (int i = 0; i<N; i++){
      double t = i*dt;

      double y1 = amp1*sin(phi1);
      if (tau1 != 0) y1 *= exp(-t/tau1);
      phi1 += 2*M_PI*fre1*dt;

      double y2 = amp2*sin(phi2);
      if (tau2 != 0) y2 *= exp(-t/tau2);
      phi2 += 2*M_PI*fre2*dt;

      double n = noise*(2.0*random()/RAND_MAX-1.0);

      int16_t v = (y1+y2+n)/sc;
      cout.write((const char*)&v, sizeof(int16_t));
    }

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
