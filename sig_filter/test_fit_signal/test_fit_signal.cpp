#include <iostream>
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
  double f0 = 32674;
  double w0 = 2*M_PI*f0;
  double tau = 0.925;
  double amp = 0.582;
  double ph  = 0.1234;
  double noise = 0.5;

  int N1 = 10;
  int N2 = 10000;


  for (N2=100000; N2<100150; N2++){
    int N=N2-N1;
    vector<int16_t> buf(N);
    int max=1<<15;
    double sc = (amp+noise)/max;

    // build a pure sine signal
    for (int i = N1; i<N2; i++){
      double t = i*dt;
      double val = amp*exp(-t/tau)*sin(w0*t + ph) +
                   noise*(2.0*random()/RAND_MAX-1.0);
      buf[i-N1] = val/sc;
    }

    vector<double> ret = fit_signal(buf.data(), buf.size(), sc, dt, N1*dt);
    cout << setprecision(12) << ret[0] << " "
         << setprecision(6)  << ret[1] << " "
         << setprecision(6)  << ret[2] << " "
         << setprecision(6)  << ret[3] << "\n";
  }

}
