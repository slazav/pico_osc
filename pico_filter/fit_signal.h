#include <vector>
#include <stdint.h>

/*
Fit a signal by a model:
  amp * exp(-t/tau) * sin(2*pi*fre*t + ph),
it also works for a non-decaying signals with 1/tau=0,

output: a vector with 4 double values:
  fre, 1/tau, amp, ph

input:
  buf -- signal data array
  len -- array length
  sc  -- amplitude scale
  dt  -- time step
  t0  -- time of the first point, returned amplitude and phase is converted to t=0
  fmin, fmax -- where to look for a frequency
*/

std::vector<double> fit_signal(const int16_t *buf, int len, double sc, double dt, double t0=0,
                               double fmin=0, double fmax=+HUGE_VAL);
