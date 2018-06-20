#ifndef FILTERS_H
#define FILTERS_H

#include "signal.h"

/***********************************************************/
// "Filters": process a signal and write results

typedef void(flt_t)(std::ostream & ff, const Signal & s, const int argc, char **);

flt_t flt_txt, flt_pnm, flt_fft_txt, flt_fft_pow, flt_fft_pow_corr,
      flt_sfft_txt, flt_sfft_int, flt_sfft_diff, flt_sfft_peaks, flt_sfft_pnm, flt_sfft_pnm_ad,
      fit, fit2, lockin, minmax, flt_sigf, flt_sig, flt_wav;

#endif
