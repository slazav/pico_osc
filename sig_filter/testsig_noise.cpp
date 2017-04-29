#include <cstdlib>
#include <iostream>
#include <cmath>
#include <getopt.h>
#include "../pico_rec/err.h"


// unpack data written by pico_rec into x-y text table

using namespace std;

void help(){
  cout << "testsig -- create 2-channal test signals with noise\n"
          "Usage: testsig [options] > <file>\n"
          "Options:\n"
          " -N <num>  -- number of points (default: 100000)\n"
          " -D <num>  -- time step, s  (default: 1e-5)\n"
          " -A <num>  -- amplitude of noise, V/sqrt(Hz) (default: 1.0)\n"
          " -c <num>  -- correlation in noise power 0..1 (default: 0.5)\n"
          " -h        -- write this help message and exit\n";
}

int
main(int argc, char *argv[]){
  try {

    double N  = 100000;
    double dt = 1e-5;
    double A = 1.0;
    double C = 0.5;

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hN:D:A:c:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'N': N   = atoi(optarg); break;
        case 'D': dt  = atof(optarg); break;
        case 'A': A = atof(optarg); break;
        case 'c': C = atof(optarg); break;
        case 'h': help(); return 0;
      }
    }

    if (C<0.0) C=0.0;
    if (C>1.0) C=1.0;

    // convert V/sqrt(Hz) -> Vpp
    double k = 2*sqrt(sqrt(2))/sqrt(dt);

    // noise amplitudes:
    // A = sqrt(a0^2 + a1^2)
    // C = a0^2/(a0^2+a1^2)
    double a0 = k*A * sqrt(C);
    double a1 = k*A * sqrt(1-C);
    double a2 = a1;

    // channel scale
    double sc = (a0 + std::max(a1,a2))/(1<<15);

    cout << "*SIG001\n"
         << "  dt:       " << dt << "\n"
         << "  t0:       " << 0 << "\n"
         << "  chan: A "   << sc << " 0\n"
         << "  chan: B "   << sc << " 0\n"
         << "\n*\n";
    for (int i = 0; i<N; i++){

      double n0 = a0*(1.0*random()/RAND_MAX-0.5);
      double n1 = a1*(1.0*random()/RAND_MAX-0.5);
      double n2 = a2*(1.0*random()/RAND_MAX-0.5);


      int16_t v1 = (n0+n1)/sc;
      int16_t v2 = (n0+n2)/sc;
      cout.write((const char*)&v1, sizeof(int16_t));
      cout.write((const char*)&v2, sizeof(int16_t));
    }

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
