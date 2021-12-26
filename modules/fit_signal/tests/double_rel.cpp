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
  double f1 = 32660;
  double f2 = 32764;
  double rtau1 = 1.2;
  double rtau2 = 30;
  double amp  = 0.582;
  double ph   = 0.1234;
  double n2a  = 0.2;

  ofstream ff("double_rel.dat");

  int N = 100000;
  double df = 1.0/((N-1)*dt);


  ff << "# f2, noise/amp,  fre_err, rtau_err, amp_err, ph_err";
  vector<int16_t> buf(N);
  int max=1<<15;
  double sc = amp*(2+n2a)/0.5/max;


  for (double f=f1; f<f2-rtau1; f+=(f2-f1)/200.0){ // freq

    // build signal
    for (int i = 0; i<N; i++){
      double t = i*dt;
      double val = 0.5*exp(-t*rtau1)*sin(2*M_PI*f*t + ph)
                 + 0.1*0.5*exp(-t*rtau2)*sin(2*M_PI*f2*t)
                 + n2a*(1.0*random()/RAND_MAX-0.5);
      buf[i] = val*amp/sc;
    }

    vector<double> ret = fit_signal(buf.data(), buf.size(), sc, dt, 0, 0, f2);
    ff   << f << " "
         << n2a << " "
         << setprecision(12) << ret[0] << setprecision(6) << " "
         <<  ret[1] << " "
         <<  ret[2] << " "
         <<  ret[3] << "\n";
  }
  ff << "\n";

}
