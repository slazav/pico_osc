#ifndef SIGNAL_H
#define SIGNAL_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdint.h>
#include "err/err.h"

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
    time_t t0abs; // absolute time (integer seconds)
    std::string t0abs_str; // absolute time (as a string)
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

    // Add channels from another signal:
    // - If the first signal is empty, replace it with new one.
    // - dt,t0 - used from the first signal, if value is different print a warning.
    // - If number of points is different signals are cropped to the shortest one.
    void add(const Signal &other);

    // Find pulse (first point where signal absolute value > k times
    // maximum absolute value) and set t0
    void find_pulse(const int ch, const double k, const double t1, const double t2);
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
// Writing signals

// write SIG format
void write_sig(std::ostream & ff, const Signal & sig);

// write SIGF format (with frequency filtering)
void write_sigf(std::ostream & ff, const Signal & sig, double fmin, double fmax);

// write WAV format
void write_wav(std::ostream & ff, const Signal & sig);

#endif
