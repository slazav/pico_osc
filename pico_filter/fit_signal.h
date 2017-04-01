#include <vector>

/*
Fit a signal by a model:
  amp * exp(-t/tau) * sin(2*pi*fre*t + ph),
it also works for a non-decaying signals with 1/tau=0,

output: a vector with 4 double values:
  fre, 1/tau, amp, ph

input:
  buf -- signal data, double array
  len -- array length
  dt  -- time step
  t0  -- time of the first point, returned amplitude and phase is converted to t=0
  fmin, fmax -- where to look for a frequency
*/

std::vector<double> fit_signal(double *buf, int len, double dt, double t0=0,
                               double fmin=0, double fmax=+HUGE_VAL);
