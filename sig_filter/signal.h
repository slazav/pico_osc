#ifndef SIGNAL_H
#define SIGNAL_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdint.h>
#include "../pico_rec/err.h"

/***********************************************************/
// Signal object.

class Signal{
  public:

    struct Channel : public std::vector<int16_t> {
        char        name; // channel name
        bool        ov;   // overload flag
        double      sc;   // scale factor
        Channel(){ sc=1.0; ov=false; name='A'; }
    };

    double t0,dt; // time shift, time step
    time_t t0abs; // absolute time
    std::vector<Channel> chan;

    // constructor
    Signal(){ t0=0.0; dt=1.0; t0abs=0;}

    // Check that all channels have same length and return it.
    // For empty signal n=0.
    size_t get_n() const{
      int n =0;
      for (int i=0; i<chan.size(); i++){
        if (i==0) n = chan[i].size();
        else if (n!=chan[i].size())
          throw Err() << "Broken signal: data arrays have different sizes";
      }
      return n;
    }

    // Get number of channels.
    size_t get_ch() const { return chan.size(); }

    // Get a double value. No range checking!
    double get_val(const int c, const int n) const{ return chan[c][n]*chan[c].sc; }

    // Set double value
    void set_val(const int c, const int n, const double v) { chan[c][n] = int(v/chan[c].sc); }

    // Select time range
    void crop_t(double tmin, double tmax);

    // Select channels ('0', '1,0', '2' etc.)
    void crop_c(const std::vector<int> & channels);
};

/***********************************************************/
// Reading signals

// autodetect format
Signal read_signal(std::istream & ff);

// SIG/SIGF formats
Signal read_sig(std::istream & ff);

// WAV format
Signal read_wav(std::istream & ff);

/***********************************************************/
// "Filters": process a signal and write results

// write SIG format
void write_sig(std::ostream & ff, const Signal & sig);

// write SIGF format (with frequency filtering)
void write_sigf(std::ostream & ff, const Signal & sig, double fmin, double fmax);

// write WAV format
void write_wav(std::ostream & ff, const Signal & sig);



// print a simple x-y text table
void flt_txt(std::ostream & ff, const Signal & s);

// make image with a raw signal
void flt_pnm(std::ostream & ff, const Signal & s, int W, int H);

// fft of the whole signal, txt table. Rectangular window
void flt_fft_txt(std::ostream & ff, const Signal & s, double fmin, double fmax);

// FFT power, reduced number of points. V^2/Hz output
void flt_fft_pow_avr(std::ostream & ff, const Signal & s, double fmin, double fmax, int npts);

// same but log scale.
void flt_fft_pow_lavr(std::ostream & ff, const Signal & s, double fmin, double fmax, int npts);

// FFT power, reduced number of points. V^2/Hz output. ch 1+2 correlaton
void flt_fft_pow_avr_corr(std::ostream & ff, const Signal & s, double fmin, double fmax, int npts);

// same but log scale.
void flt_fft_pow_lavr_corr(std::ostream & ff, const Signal & s, double fmin, double fmax, int npts);

// Text table with sliding fft. Blackman window. 1st channel
void flt_sfft_txt(std::ostream & ff, const Signal & s, double fmin, double fmax, int win);

// Text table with sliding fft integral. Blackman window. 1st channel
void flt_sfft_int(std::ostream & ff, const Signal & s, double fmin, double fmax, int win);

// Sliding fft + peak detection. Blackman window. 1st channel
void flt_sfft_peaks(std::ostream & ff, const Signal & s, double fmin, double fmax, int win, double th);

// Sliding fft + step detection. Blackman window. 1st channel
void flt_sfft_steps(std::ostream & ff, const Signal & s, double fmin, double fmax, int win, double th);

// PNM with sliding fft. Blackman window. 1st channel
void flt_sfft_pnm(std::ostream & ff, const Signal & s, double fmin, double fmax, double amin, double amax, int win, int W, int H);

// Adaptive window, no smoothing. Blackman window.
void flt_sfft_pnm_ad(std::ostream & ff, const Signal & s, double fmin, double fmax, double amin, double amax, int W, int H);

// Print t-a-f table. Adaptive window, no smoothing. Blackman window.
//void taf_ad(std::ostream & ff, const Signal & s, double fmin, double fmax);

// fit fork signal (exponential decay, constant frequency)
void fit(std::ostream & ff, const Signal & s, double fmin, double fmax);

// fit two-fork signal (sort by frequency)
void fit2(std::ostream & ff, Signal & s, double fmin, double fmax);

// fit fork signal (exponential decay, constant frequency)
void lockin(std::ostream & ff, const Signal & s, double fmin, double fmax);

// print min/max values
void minmax(std::ostream & ff, const Signal & s);

// print sigf file (filtered fft)
void sigf(std::ostream & ff, Signal & s, double fmin, double fmax);

// Remove unwanted time ans frequency.
//void crop(std::ostream & ff, const Signal & s, double fmin, double fmax);

#endif
