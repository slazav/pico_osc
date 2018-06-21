#include <cstdlib>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <cstring>
#include <cmath>

#include "signal.h"
#include "filters.h"


// unpack data written by pico_rec into x-y text table

using namespace std;

// convert a string with comma/space separated integer numbers into an integer vector
vector<int> str2ivec(const char *str){
  char *s, *t, *saveptr;
  const char * delim="\n\t ,";
  int j;
  vector<int> ret;
  for (j = 1, s = (char*)str; ; j++, s = NULL) {
    t = strtok_r(s, delim, &saveptr);
    if (t == NULL) break;
    ret.push_back(atoi(t));
  }
  return ret;
}

// print help
void help(){
  cout << "pico_filter -- read and process pico_rec datafiles\n"
          "Usage: pico_filter [options] <filename> [filter options]\n"
          "Options:\n"
          " -h        -- write this help message and exit\n"
          " -f <name> -- filter name\n"
          " -c <chan> -- Comma- or space-separated channel numbers (\"0\", \"1\", \"1,0\", etc.).\n"
          "              Different filters use different number of channels.\n"
          " -T <num>  -- min time (default -infinity)\n"
          " -U <num>  -- max time (default +infinity)\n"
          "              The signal is cut to the specified time range before filtering.\n"
          "Filters:\n"
          "     txt -- Print a text table with all channels.\n"
          "       No filter options.\n"
          "     pnm -- Make image with all channels.\n"
          "       Options:\n"
          "       -W <value> -- image width, pixels (default: 1024)\n"
          "       -H <value> -- image height, pixels (default: 768)\n"
          "     fft_txt -- Do FFT of the whole signal, print text table. Rectangular window, all channels.\n"
          "       Options:\n"
          "       -F <value> -- low frequency limit\n"
          "       -G <value> -- high frequency limit\n"
          "     fft_pow -- FFT averaged power with reduced number of points. V^2/Hz output, all channels\n"
          "       Options:\n"
          "       -F <value> -- low frequency limit\n"
          "       -G <value> -- high frequency limit\n"
          "       -N <value> -- number of points (default: 1024)\n"
          "       -l         -- use log scale\n"
          "     fft_pow_corr  -- same as fft_pow, but calculates correlation between channal 0 and 1\n"
          "       Options: same as for fft_pow\n"
          "     sfft_txt      -- Sliding fft with Blackman window, text table (T,F,X,Y).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -w <value>  -- Window length (points)\n"
          "     sfft_int      -- Sliding fft with Blackman window, integral\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -w <value>  -- Window length (points)\n"
          "     sfft_diff     -- Sliding fft with Blackman window, difference with previous time bin\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -w <value>  -- Window length (points)\n"
          "     sfft_peaks    -- Sliding fft with Blackman window, peak detection.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -w <value>  -- Window length (points)\n"
          "       -t <value>  -- Threshold (deefault 2.5)\n"
          "     sfft_pnm      -- Sliding fft with Blackman window, pnm picture\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -A <value>  -- low amplitude limit\n"
          "       -B <value>  -- high amplitude limit\n"
          "       -w <value>  -- Window length (points)\n"
          "       -W <value>  -- image width, pixels (default: 1024)\n"
          "       -H <value>  -- image height, pixels (default: 768)\n"
          "     sfft_pnm_ad   -- Adaptive window, no smoothing. Blackman window.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -A <value>  -- low amplitude limit\n"
          "       -B <value>  -- high amplitude limit\n"
          "       -W <value>  -- image width, pixels (default: 1024)\n"
          "       -H <value>  -- image height, pixels (default: 768)\n"
          "     fit           -- Fit fork signal (exponential decay, constant frequency).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "     fit2          -- Fit two fork signal (sort by frequency).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "     lockin        -- Detect signal using another channel as reference.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -s <value>  -- signal channel (default: 0)\n"
          "       -r <value>  -- reference channel (default: 1)\n"
          "     slockin       -- Sliding lockin.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -s <value>  -- signal channel (default: 0)\n"
          "       -r <value>  -- reference channel (default: 1)\n"
          "       -w <value>  -- window length (points)\n"
          "       -f <value>  -- manually set frequency, do not use reference channel\n"
          "       -p <value>  -- manually set phase (works only with -f, degrees, default: 0)\n"
          "     minmax        -- Print min/max values for each channel.\n"
          "     sigf          -- Print SIGF file (fft with frequency filtering).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "     sig           -- Print SIG file.\n"
          "     wav           -- Print WAV file.\n"
  ;
}

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *cn = ""; // channel names
    const char *flt = NULL; // filter name
    double tmin = -HUGE_VAL;
    double tmax = +HUGE_VAL;

    /* parse  options */
    opterr=0;
    while(1){
      int c = getopt(argc, argv, "+hf:c:T:U:");
      if (c==-1) break;
      switch (c){
        case '?': throw Err() << "Unknown option: -" << (char)optopt;
        case ':': throw Err() << "No argument: -" << (char)optopt;
        case 'h': help(); return 0;
        case 'f': flt = optarg; break;
        case 'c': cn = optarg; break;
        case 'T': tmin = atof(optarg); break;
        case 'U': tmax = atof(optarg); break;
      }
    }
    argc-=optind;
    argv+=optind;
    optind=1;
    if (argc<1) { help(); return 0; }
    if (flt==NULL) throw Err() << "Filter is not specified, use -f option\n";
    const char *fname = argv[0];

    /* read the signal */
    std::ifstream ff(fname);
    Signal sig = read_signal(ff);

    /* crop time, rearrange channels */
    sig.crop_t(tmin, tmax);
    if (strlen(cn)>0)  sig.crop_c(str2ivec(cn));
    if (sig.get_n()<1 || sig.get_ch()<1) throw Err() << "empty signal";

    if (strcasecmp(flt, "txt")==0)                    flt_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "pnm")==0)               flt_pnm(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_txt")==0)           flt_fft_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_pow")==0)           flt_fft_pow(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_pow_corr")==0)      flt_fft_pow_corr(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_txt")==0)          flt_sfft_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_int")==0)          flt_sfft_int(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_diff")==0)         flt_sfft_diff(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_peaks")==0)        flt_sfft_peaks(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_pnm")==0)          flt_sfft_pnm(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_pnm_ad")==0)       flt_sfft_pnm_ad(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fit")==0)               fit(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fit2")==0)              fit2(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "lockin")==0)            lockin(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "slockin")==0)           slockin(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "minmax")==0)            minmax(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sigf")==0)              flt_sigf(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sig")==0)               flt_sig(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "wav")==0)               flt_wav(std::cout, sig, argc, argv);
    else throw Err() << "Unknown filter: " << flt;
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
