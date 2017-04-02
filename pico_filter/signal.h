#ifndef SIGNAL_H
#define SIGNAL_H

#include <string>
#include <vector>
#include <stdint.h>
#include "../pico_rec/err.h"

// A data-file object. Read/Write functions

class Channel : public std::vector<int16_t> {
  public:
    char        name; // channel name
    bool        ov;   // overload flag
    double      sc;   // scale factor
    Channel(){ sc=1.0; ov=false; name='A'; }
};

class Signal{
  public:
    double t0,dt; // time shift, time step
    time_t t0abs; // absolute time
    std::vector<Channel> chan;

    // constructors
    Signal(){ t0=0.0; dt=1.0; t0abs=0;}
};

Signal read_signal(const char *fname);
void write_signal(const char *fname, const Signal & sig);

#endif