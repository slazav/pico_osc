#include <vector>

/*
Fit a signal by a model:
  A*exp(-t/t0)*sin(2*pi*f0*t + ph),

output: a vector with 4 double values:
  f0, t0, A, ph

input:
  buf -- signal data, double array
  len -- array length
  dt  -- time step
  fmin, fmax -- where to look for a frequency
*/

std::vector<double> fit_signal(double *buf, int len, double dt,
                               double fmin=0, double fmax=+HUGE_VAL);
