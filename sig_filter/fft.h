#ifndef FFT_H
#define FFT_H

#include <cmath>
#include <fftw3.h>
#include "../pico_rec/err.h"

// FFTW documentation:
// http://www.fftw.org/fftw3_doc/

/******************************************************************/
// fft wrapper
class FFT{
  // blackman window parameters
  constexpr static double a0=0.42659, a1=0.49656, a2=0.076849;

  fftw_complex *cbuf;
  fftw_plan     plan;
  int           len;
  public:

  FFT(int len_, int fl1=FFTW_FORWARD, int fl2=FFTW_ESTIMATE): len(len_){
    cbuf = fftw_alloc_complex(len);
    plan = fftw_plan_dft_1d(len, cbuf, cbuf, fl1, fl2);
  }
  ~FFT(){
    fftw_destroy_plan(plan);
    fftw_free(cbuf);
  }
  double real(const int i) const {return cbuf[i][0];}
  double imag(const int i) const {return cbuf[i][1];}
  double abs(const int i) const {return hypot(cbuf[i][0],cbuf[i][1]);}

  // get real/imag/abs as a vector of double
  std::vector<double> real(const int i1, const int i2) const {
    std::vector<double> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = cbuf[i][0];
    return ret;
  }
  std::vector<double> imag(const int i1, const int i2) const {
    std::vector<double> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = cbuf[i][1];
    return ret;
  }
  std::vector<double> abs(const int i1, const int i2) const {
    std::vector<double> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = hypot(cbuf[i][0],cbuf[i][1]);
    return ret;
  }

  // get real/imag/abs as a vector of int16_t (useful for converting fft to original signal)
  std::vector<int16_t> real(const int i1, const int i2, const double sc) const {
    std::vector<int16_t> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = cbuf[i][0]/sc;
    return ret;
  }
  std::vector<int16_t> imag(const int i1, const int i2, const double sc) const {
    std::vector<int16_t> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = cbuf[i][1]/sc;
    return ret;
  }
  std::vector<int16_t> abs(const int i1, const int i2, const double sc) const {
    std::vector<int16_t> ret(i2-i1);
    if (i1<0 || i2>len) throw Err() << "index out of range";
    for (int i=i1; i<i2; i++) ret[i-i1] = hypot(cbuf[i][0],cbuf[i][1])/sc;
    return ret;
  }


  void set(const int i, const double re, const double im) {cbuf[i][0]=re; cbuf[i][1]=im;}

  void run(const int16_t *dbuf, double sc, bool use_blackman=false){
    // fill complex buffers
    for (int i=0; i<len; i++){
      cbuf[i][0] = sc*dbuf[i];
      cbuf[i][1] = 0;
      if (use_blackman) cbuf[i][0] *= a0-a1*cos(2*M_PI*i/(len-1))+a2*cos(4*M_PI*i/(len-1));
    }
    // do fft
    fftw_execute(plan);
  }

  void run(){
    fftw_execute(plan);
  }


  // find absolute maxinmum
  int find_max(int i1f, int i2f) const{
    // find maximum
    double vm = abs(i1f);
    int im = i1f;
    for (int i = i1f; i<i2f; i++){
      double v = abs(i);
      if (v>=vm) {vm=v; im=i;}
    }
    return im;
  }

  // find parabolic fit near maximum
  void find_max_par(int i1f, int i2f, double df, double &A, double &B, double &C) const {
    int im = find_max(i1f, i2f);
    if (im<i1f+1 || im>=i2f-1) throw Err() << "Maximum on the edge of the frequency range";
    double x1 = df*(im-1);
    double x2 = df*im;
    double x3 = df*(im+1);
    double y1 = abs(im-1);
    double y2 = abs(im);
    double y3 = abs(im+1);
    A = ((y1-y2)/(x1-x2) - (y2-y3)/(x2-x3))/(x1-x3);
    B = (y1-y2)/(x1-x2) - A*(x1+x2);
    C = y1 - A*x1*x1 - B*x1;
  }

  // adjust fmin/fmax
  void get_ind(const double dt, double *fmin, double *fmax, int *i1f, int *i2f, double *df){
    if (*fmax<*fmin) std::swap(*fmax, *fmin);
    if (*fmax > 0.5/dt) *fmax = 0.5/dt;
    if (*fmin > 0.5/dt) *fmin = 0.5/dt;
    if (*fmin < 0) *fmin = 0;
    // select frequency indices
    *df = 1/dt/len;
    *i1f = std::max(0.0, floor(*fmin / *df));
    *i2f = std::min(0.5*len, ceil(*fmax / *df));
    *fmin = *df * *i1f;
    *fmax = *df * *i2f;
    if (*i2f-*i1f<1) throw Err() << "Error: too small frequency range: " << *fmin << " - " << *fmax;
  }


};

#endif