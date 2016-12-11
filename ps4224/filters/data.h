#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <stdint.h>
#include "../err.h"

// read data file

class Data{

  public:

  double t0,dt,sc; // time shift, time step, data scale
  int num;         // number of data columns in the file
  std::vector<int16_t> data;

  // constructor -- read one data channel from a file
  Data(const char *fname, int n);

  void print_txt() const;

  void print_pnm(int w, int h, int color=0) const;

  void print_fft_txt(double fmin, double fmax, double tmin, double tmax) const;

  // text table with sliding fft
  void print_sfft_txt(double fmin, double fmax, double tmin, double tmax, int win) const;

  void print_sfft_pnm(double fmin, double fmax, double tmin, double tmax) const;

  void print_sfft_pnm1(double fmin, double fmax, double tmin, double tmax) const;


  void fit_fork(double fmin, double fmax, double tmin, double tmax) const;

};

#endif