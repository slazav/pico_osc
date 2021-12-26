#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <complex>
#include <vector>
#include "fit_signal/fit_signal.h"

using namespace std;

int
main(){
  // parameters
  double dt = 0.411e-5;
  double f0 = 32764;
  double amp  = 0.582;
  double ph   = 0.1234;

  ofstream ff("single_pure.dat");

  int N = 100000;
  double df = 1.0/((N-1)*dt);


  ff << "# f0, noise/amp,  fre_err, rtau_err, amp_err, ph_err";
  for (double n2a=0; n2a<2; n2a+=0.1){ // noise/amp ratio
    vector<int16_t> buf(N);
    int max=1<<15;
    double sc = amp*(1+n2a)/0.5/max;


    for (double f=f0; f<f0+df; f+=df/30.0){ // freq

      // build signal
      for (int i = 0; i<N; i++){
        double t = i*dt;
        double val = 0.5*sin(2*M_PI*f*t + ph)
                   + n2a*(1.0*random()/RAND_MAX-0.5);
        buf[i] = val*amp/sc;
      }

      vector<double> ret1 = fit_signal(buf.data(), buf.size(), sc, dt, 0);
      vector<double> ret2 = fit_signal_fixfre(buf.data(), buf.size(), sc, dt, 0);

      ff   << f << " "
           << n2a << " "
           << (ret1[0]-f)/f << " "
           <<  ret1[1] << " "
           << (ret1[2]-amp)/amp   << " "
           <<  ret1[3]-ph << "   "
           << (ret2[0]-f)/f << " "
           <<  ret2[1] << " "
           << (ret2[2]-amp)/amp   << " "
           <<  ret2[3]-ph << "\n";
    }
    ff << "\n";
  }

}
