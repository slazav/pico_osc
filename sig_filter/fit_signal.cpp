#include <vector>
#include <complex>
#include <cmath>
#include <iostream>
#include <fftw3.h>
#include <stdint.h>
#include "../pico_rec/err.h"

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
  // index limits
  int i1f = max(0.0,    floor(fmin/df));
  int i2f = min(0.5*len, ceil(fmax/df));
  double vm = hypot(cbuf[i1f][0], cbuf[i1f][1]);
  int im = i1f;
  for (int i = i1f; i<i2f; i++){
    double v = hypot(cbuf[i][0], cbuf[i][1]);
    if (v>=vm) {vm=v; im=i;}
  }

  // second step: find parabolic fit near maximum
  if (im<i1f+1 || im>=i2f-1)
    throw Err() << "Can't find signal frequency";

  double x1 = df*(im-1);
  double x2 = df*im;
  double x3 = df*(im+1);
  double y1 = hypot(cbuf[im-1][0], cbuf[im-1][1]);
  double y2 = hypot(cbuf[im  ][0], cbuf[im  ][1]);
  double y3 = hypot(cbuf[im+1][0], cbuf[im+1][1]);
  double A = ((y1-y2)/(x1-x2) - (y2-y3)/(x2-x3))/(x1-x3);
  double B = (y1-y2)/(x1-x2) - A*(x1+x2);
  double C = y1 - A*x1*x1 - B*x1;
  // fre - positin of the parabola maximum
  // rtau - distance between zero crossings
  // amp0 - value in the maximum
  double fre = -B/(2*A);
  double rtau = -sqrt(B*B-4*A*C)*M_PI/2/A;
  double amp0 = C - B*B/(4*A);


  // third step: fit 1/fft by a linear function
  // adjust index limits
  double dff = rtau;
  i1f = max(0.0,     floor((fre-dff)/df));
  i2f = min(0.5*len, ceil((fre+dff)/df));

  // linear fit with weight w
  double sx2=0, sx1=0, sx0=0;
  complex<double> sxy(0,0), sy(0,0);
  for (int i = i1f; i<i2f; i++){
    double x = df*i-fre;
    complex<double> y = amp0/complex<double>(cbuf[i][0], cbuf[i][1]);
    double w = pow(1.0/(x*x+rtau), 4.0);
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
  rtau = -2*M_PI*(BB/AA).imag();
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

