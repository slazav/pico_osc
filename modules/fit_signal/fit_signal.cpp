#include <vector>
#include <complex>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include <stdint.h>
#include "err/err.h"

using namespace std;




vector<double> fit_signal(const int16_t *buf, int len, double sc, double dt, double t0=0,
                          double fmin=0, double fmax=+HUGE_VAL){
  // signal parameters:
  double tmin=0;
  double tmax=dt*len;
  double df = 1/tmax;

  // do fft
  fftw_complex *cbuf = fftw_alloc_complex(len);
  fftw_plan     plan = fftw_plan_dft_1d(len, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);
  for (int i=0; i<len; i++){
    cbuf[i][0] = buf[i]*sc;
    cbuf[i][1] = 0;
  }
  fftw_execute(plan);

  // first step: find max(abs(fft))
  int i1f = max(0,   int(floor(fmin/df)));
  int i2f = min(0.5*len, ceil(fmax/df));
  double vm = 0;
  int im = i1f;
  for (int i = i1f+1; i<i2f-1; i++){
    double v = hypot(cbuf[i][0], cbuf[i][1]);
    double v1 = hypot(cbuf[i-1][0], cbuf[i-1][1]);
    double v2 = hypot(cbuf[i+1][0], cbuf[i+1][1]);
    if (v>v1 && v>=v2 && v>=vm) {vm=v; im=i;}
  }

  if (im<i1f+1 || im>=i2f-1)
    throw Err() << "Can't find signal frequency";

  double amp0 = df;
  double fre = im*df;

  // third step: fit 1/fft by a linear function

  // Adjust index limits.
  // Select range where amplitude is growing from both ends
  // towards the maximum at im:
  int i1 = im;
  while (1){
    if (i1 >= i2f) break;
    if (hypot(cbuf[i1][0], cbuf[i1][1]) <
        hypot(cbuf[i1+1][0], cbuf[i1+1][1])) break;
    i1++;
  }
  i2f = i1;

  i1 = im;
  while (1){
    if (i1 <= i1f) break;
    if (hypot(cbuf[i1][0], cbuf[i1][1]) <
        hypot(cbuf[i1-1][0], cbuf[i1-1][1])) break;
    i1--;
  }
  i1f = i1;

  // linear fit with weight w
  double sx2=0, sx1=0, sx0=0;
  complex<double> sxy(0,0), sy(0,0);
  for (int i = i1f; i<i2f; i++){
    double x = df*i-fre;
    complex<double> y = amp0/complex<double>(cbuf[i][0], cbuf[i][1]);
    double w = pow(1.0/abs(y), 4.0);
    sx2 += x*x*w;
    sx1 += x*w;
    sx0 += w;
    sxy += x*y*w;
    sy  += y*w;
  }
  fftw_destroy_plan(plan);
  fftw_free(cbuf);

  // complex linear fit  AA*(f-fre) + BB
  complex<double> BB = (sxy*sx1 - sy*sx2)/(sx1*sx1 - sx0*sx2);
  complex<double> AA = (sy*sx1 - sxy*sx0)/(sx1*sx1 - sx0*sx2);
  complex<double> I = complex<double>(0,1);

  // this complex amplitude corresponds to the original complex fft signal
  complex<double> amp = 2*M_PI*I/AA * amp0;
  // Convert to volts:
  amp*=2*dt;
  // Exact freq an tau:
  fre = fre - (BB/AA).real();
  double rtau = -2*M_PI*(BB/AA).imag();
  // Boundary-dependent factor:
  complex<double> v1 = exp(2*M_PI*fre*I*tmin - tmin*rtau);
  complex<double> v2 = exp(2*M_PI*fre*I*tmax - tmax*rtau);
  amp /= v2-v1;

  //convert from t=t0 to t=0:
  amp *=exp(t0*rtau - 2*M_PI*fre*I*t0);

  vector<double> ret(5,0.0);
  ret[0] = fre;
  ret[1] = rtau;
  ret[2] = 2.0*abs(amp); // -> pk-pk amplitude
  ret[3] = arg(amp)-M_PI/2;
  return ret;
}

// fix problem with 'pure signals' where F = 1/tmax and only one fft component is
// large
vector<double> fit_signal_fixfre(const int16_t *buf, int len, double sc, double dt, double t0=0,
                          double fmin=0, double fmax=+HUGE_VAL){

  // run fit_signal
  vector<double> ret = fit_signal(buf, len, sc, dt, t0, fmin, fmax);

  // frequency resolution:
  double df = 1/(dt*len);

  // line is wider then frequency resolution, no need for the fix
  if (ret[1] > 2*df) return ret;

  double fft_freq = floor(ret[0]/df)*df; // fft grid point below signal frequency

  // We want to remove k points from the signal in such a way that
  // signal frequency will be between FFT grid points

  // how much should we shift grid point near the peak:
  double fshift = ret[0] - fft_freq - 0.5*df;
  if (fshift < 0) fshift += df; // always positive!

  // how much points shoud we remove to shift fft grid by fshift?
  int k = rint(fshift/fft_freq * len);

  if (k<1) return ret;

  ret = fit_signal(buf, len-k, sc, dt, t0, fmin, fmax);

  return ret;

}
