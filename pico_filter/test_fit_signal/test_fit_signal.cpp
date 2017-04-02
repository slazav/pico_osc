#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex>
#include <vector>
#include <fftw3.h>
#include "../fit_signal.h"

using namespace std;

main(){
  // parameters
  double dt = 0.411e-5;
  double f0 = 32674;
  double w0 = 2*M_PI*f0;
  double tau = 0.925;
  double amp = 0.582;
  double ph  = 0.1234;

  int N1 = 10;
  int N2 = 10000;


  for (N2=100000; N2<100150; N2++){
    int N=N2-N1;
    vector<int16_t> buf(N);
    int max=1<<15;
    double sc = amp/max;

    // build a pure sine signal
    for (int i = N1; i<N2; i++){
      double t = i*dt;
      buf[i-N1] = max*exp(-t/tau)*sin(w0*t + ph);
    //  buf[i-N1] = max*cos(w0*t);
    }

    vector<double> ret = fit_signal(buf.data(), buf.size(), sc, dt, N1*dt);
    cout << setprecision(12) << ret[0] << " "
         << setprecision(6)  << ret[1] << " "
         << setprecision(6)  << ret[2] << " "
         << setprecision(6)  << ret[3] << "\n";
  }

}
