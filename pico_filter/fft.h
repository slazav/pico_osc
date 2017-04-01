#include <cmath>
#include <fftw3.h>
#include "../pico_rec/err.h"

/******************************************************************/
// fft wrapper
class FFT{
  // blackman window parameters
  const static double a0=0.42659, a1=0.49656, a2=0.076849;

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

};

