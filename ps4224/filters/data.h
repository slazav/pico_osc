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
  time_t t0abs;
  std::vector<int16_t> data;

  int i1t,i2t,i1f,i2f,lent,lenf;
  double df;

  // constructor -- read one data channel from a file
  Data(const char *fname, int n);

  // check ranges and update signal indices
  void set_sig_ind(double &fmin, double &fmax, double &tmin, double &tmax, int win=0);


  // print a simple x-y text table
  void print_txt() const;

  // make image with a raw signal
  void print_pnm(int w, int h, int color=0) const;

  // fft of the whole signal, txt table. Rectangular window
  void print_fft_txt(double fmin, double fmax, double tmin, double tmax);

  // Text table with sliding fft. Blackman window.
  void print_sfft_txt(double fmin, double fmax, double tmin, double tmax, int win);

  // PNM with sliding fft. Blackman window.
  void print_sfft_pnm(double fmin, double fmax, double tmin, double tmax, int win, int w, int h);

  // Adaptive window, no smoothing. Blackman window.
  void print_sfft_pnm_ad(double fmin, double fmax, double tmin, double tmax, int w, int h);

  // fit fork signal (exponential decay, constant frequency)
  void fit_fork(double fmin, double fmax, double tmin, double tmax);

};

#endif