#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <stdint.h>
#include "../pico_rec/err.h"
#include "signal.h"

// print a simple x-y text table
void flt_txt(const Signal & s);

// make image with a raw signal
void flt_pnm(const Signal & s, int W, int H);

// fft of the whole signal, txt table. Rectangular window
void flt_fft_txt(const Signal & s, double fmin, double fmax);

// FFT power, reduced number of points. V^2/Hz output
void flt_fft_pow_avr(const Signal & s, double fmin, double fmax, int npts);

// same but log scale.
void flt_fft_pow_lavr(const Signal & s, double fmin, double fmax, int npts);

// Text table with sliding fft. Blackman window. 1st channel
void flt_sfft_txt(const Signal & s, double fmin, double fmax, int win);

// PNM with sliding fft. Blackman window. 1st channel
void flt_sfft_pnm(const Signal & s, double fmin, double fmax, int win, int W, int H);

// Adaptive window, no smoothing. Blackman window.
void flt_sfft_pnm_ad(const Signal & s, double fmin, double fmax, int W, int H);

// Print t-a-f table. Adaptive window, no smoothing. Blackman window.
//void taf_ad(const Signal & s, double fmin, double fmax);

// fit fork signal (exponential decay, constant frequency)
void fit(const Signal & s, double fmin, double fmax);

// fit fork signal (exponential decay, constant frequency)
void lockin(const Signal & s, double fmin, double fmax);

// Remove unwanted time ans frequency.
//void crop(const Signal & s, double fmin, double fmax);

#endif