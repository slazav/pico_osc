#ifndef OSCSIG_H
#define OSCSIG_H

#include <iostream>
#include <complex>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fftw3.h>

struct Lor{
  double A,B; // complex amplitude
  double fre; // frequency
  double tau; // tau
  Lor(): A(0),B(0),fre(0),tau(0){}
};

typedef std::complex<double> Comp;

// Signal from oscilloscope,
// data storadge with memory menegement
//
class OscSig{

  public:
    fftw_complex *data; /* main data array */
    int     size;       /* data size       */
    double  start,step; /* x-coord start and step*/

  /************************************/
  /* Refcounter and memory management */

  private:
    int * refcounter;

    void create(int size_, double step_, double start_){
      size  = size_;
      step  = step_;
      start = start_;
      data = fftw_alloc_complex(size);
      assert(data);
      refcounter   = new int;
      *refcounter  = 1;
    }
    void copy(const OscSig & other){
      data  = other.data;
      size  = other.size;
      step  = other.step;
      start = other.start;
      refcounter = other.refcounter;
      (*refcounter)++;
      assert(*refcounter >0);
    }
    void destroy(void){
      (*refcounter)--;
      if (*refcounter<=0){
      fftw_free(data);
      delete refcounter;
    }
  }

  /************************************/
  /* Copy constructor, destructor, assignment */
  public:

    OscSig(const OscSig & other){ copy(other); }

    OscSig & operator=(const OscSig & other){
      if (this != &other){ destroy(); copy(other); }
      return *this;
    }

    ~OscSig(){ destroy(); }

  /************************************/
  /* Main constructor -- create uninitialized array */
  OscSig(int size, double step, double start=0){ create(size,step,start); }


  /************************************/
  // create test function: decaying oscillations
  void mk_test_func(double amp, double fre, double phi, double tau, double noise){
    for (int i = 0; i<size; i++){
      double t = start+i*step;
      data[i][0] = amp*sin(2*M_PI*fre*t + phi)*exp(-t/tau)
              + noise*(2.0*random()/RAND_MAX-1.0);
      data[i][1] = 0;
    }
  }

  // dump data as a text file
  void text_dump(){
    for (int i = 0; i<size; i++){
      double t = start+i*step;
      std::cout << t << "\t" << data[i][0] << "\t" << data[i][1] << "\n";
    }
  }

  // do fft
  void fft(){
    /* fftw plan */
    fftw_plan p = fftw_plan_dft_1d(size, data, data, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    step = 1/(size*step);
    start = 0;
  }

  // get_index_range
  void get_index_range(double v1, double v2, int *i1, int *i2) const{
    *i1 = floor((v1-start)/step);
    *i2 = ceil((v2-start)/step);
    if (*i1<0)    *i1=0;
    if (*i2>size) *i2=size;
    if (*i2<=*i1) *i2=*i1;
    return;
  }
  // x-value at a given index
  double get_x(int i) const { return start+step*i; }

  // absolute value at a given index
  double get_abs(int i)  const { return hypot(data[i][0],data[i][1]); }
  double get_abs2(int i) const { return pow(data[i][0],2)+pow(data[i][1],2); }
  Comp get_comp(int i) const {return Comp(data[i][0], data[i][1]);}

  // crop data
  OscSig crop(double v1, double v2) const{
    int i1,i2;
    get_index_range(v1,v2,&i1,&i2);
    OscSig ret(i2-i1, step, get_x(i1));
    memcpy(ret.data, data+i1, ret.size*sizeof(fftw_complex));
    return ret;
  }

  // looking for peak
  Lor find_peak(double v1, double v2) const{
    int i1,i2;
    get_index_range(v1,v2,&i1,&i2);

    // find maximum
    if (i2-i1<1) return Lor();
    double vm = get_abs(0);
    double im = i1;
    for (int i = i1; i<i2; i++){
      double v = get_abs(i);
      if (v>vm) {vm=v; im=i;}
    }
    // 3-point fit
    if (im<i1+1 || im>=i2-1) return Lor();
    double x1 = get_x(im-1);
    double x2 = get_x(im);
    double x3 = get_x(im+1);
    double y1 = get_abs(im-1);
    double y2 = get_abs(im);
    double y3 = get_abs(im+1);
    double A = ((y1-y2)/(x1-x2) - (y2-y3)/(x2-x3))/(x1-x3);
    double B = (y1-y2)/(x1-x2) - A*(x1+x2);
    double C = y1 - A*x1*x1 - B*x1;
    double max = -B*B/(4*A) + C;
    Lor l1;
    // fre - parabola maximum
    // tau - one over distance between zero crossings
    l1.fre = -B/(2*A);
    l1.tau = -2*A/sqrt(B*B-4*A*C)/M_PI;
    l1.A = max/l1.tau;
    l1.B = 0;

    // secons pass
    double df = M_PI/l1.tau;
    v1 = l1.fre-df;
    v2 = l1.fre+df;
    get_index_range(v1,v2,&i1,&i2);

    double sx2=0, sx1=0, sx0=0;
    Comp sxy(0,0), sy(0,0);
    for (int i = i1; i<i2; i++){
      double x = get_x(i)-l1.fre;
      Comp y = max/get_comp(i);
      double w = pow(get_abs2(i),2);
      sx2 += x*x*w;
      sx1 += x*w;
      sx0 += w;
      sxy += x*y*w;
      sy  += y*w;
    }
    Comp BB = (sxy*sx1 - sy*sx2)/(sx1*sx1 - sx0*sx2);
    Comp AA = (sxy - BB*sx1)/sx2;

    Lor l2;
    l2.A =  2*M_PI*(1.0/AA).imag()*max;
    l2.B =  2*M_PI*(1.0/AA).real()*max;
    l2.fre = l1.fre - (BB/AA).real();
    l2.tau = -1/(2*M_PI*(BB/AA).imag());
    return l2;
  }


};

#endif
