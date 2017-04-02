#include "data.h"
#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <cmath>

#include "signal.h"


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
          "Usage: pico_filter [options] <filename>\n"
          "Options:\n"
          " -h        -- write this help message and exit\n"
          " -c <chan> -- Comma- or space-separated channel numbers (\"0\", \"1\", \"1,0\", etc.).\n"
          "              Different filters use different number of channels.\n"
          " -T <num>  -- min time (default -infinity)\n"
          " -U <num>  -- max time (default +infinity)\n"
          "              The signal is cut to the specified time range before filtering.\n"
          " -W <num>  -- image width (default 1024)\n"
          " -H <num>  -- image height (default 768)\n"
          "              Used in filters with pnm output.\n"
          " -F <num>  -- min frequency\n"
          " -G <num>  -- max frequency\n"
          "              Used in filters with fft calculation.\n"
          " -w <num>  -- fft window for sliding fft filters, 1024 by default\n"
          " -N <num>  -- number of points (for fft_pow_avr, fft_pow_lavr), 1024 by default\n"
          " -f <name> -- filter type:\n"
          "     txt (default) -- Print a text table with all channels.\n"
          "     pnm           -- Make image with all channels.\n"
          "     fft_txt       -- FFT of the whole signal, txt table. Rectangular window, all channels.\n"
          "     fft_pow_avr   -- FFT power with reduced number of points. V^2/Hz output\n"
          "     fft_pow_lavr  -- same but in log scale\n"
          "     sfft_txt      -- Text table with sliding fft. Blackman window.\n"
          "     sfft_pnm      -- PNM with sliding fft. Blackman window.\n"
          "     sfft_pnm_ad   -- Adaptive window, no smoothing. Blackman window.\n"
          "     fit           -- Fit fork signal (exponential decay, constant frequency).\n"
          "     lockin        -- Detect signal in channel 1 use channel 2 as a reference.\n"
/*
          "              taf         -- time-amp-fre table with adaptive window.\n"
          "              crop        -- Crop time and frequency ranges, print table.\n"
*/
  ;
}

int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *cn = ""; // channel names
    double tmin = -HUGE_VAL;
    double tmax = +HUGE_VAL;
    double fmin = -HUGE_VAL;
    double fmax = +HUGE_VAL;
    char f_def[] = "txt";
    char *f = f_def;
    int W=1024, H=768;
    int win = 1024;
    int npts = 1024;

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hf:c:T:U:W:H:F:G:w:N:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'h': help(); return 0;
        case 'f': f = optarg; break;
        case 'c': cn = optarg; break;
        case 'T': tmin = atof(optarg); break;
        case 'U': tmax = atof(optarg); break;
        case 'W': W = atoi(optarg); break;
        case 'H': H = atoi(optarg); break;
        case 'F': fmin = atof(optarg); break;
        case 'G': fmax = atof(optarg); break;
        case 'w': win = atoi(optarg); break;
        case 'N': npts = atoi(optarg); break;
      }
    }
    argc-=optind;
    argv+=optind;
    if (argc!=1) { help(); return 0; }
    const char *fname = argv[0];

    Signal sig = read_signal(fname);
    sig.crop_t(tmin, tmax);
    if (strlen(cn)>0){
      sig.crop_c(str2ivec(cn));
    }
    if (sig.get_n()<1 || sig.get_ch()<1) throw Err() << "empty signal";

    if (strcasecmp(f, "txt")==0){
      flt_txt(sig);
    }
    else if (strcasecmp(f, "pnm")==0){
      flt_pnm(sig, W,H);
    }
    else if (strcasecmp(f, "fft_txt")==0){
       flt_fft_txt(sig, fmin,fmax);
    }
    else if (strcasecmp(f, "fft_pow_avr")==0){
       flt_fft_pow_avr(sig, fmin,fmax, npts);
    }
    else if (strcasecmp(f, "fft_pow_lavr")==0){
       flt_fft_pow_lavr(sig, fmin,fmax, npts);
    }
    else if (strcasecmp(f, "sfft_txt")==0){
      flt_sfft_txt(sig, fmin,fmax, win);
    }
    else if (strcasecmp(f, "sfft_pnm")==0){
       flt_sfft_pnm(sig, fmin ,fmax, win, W,H);
    }
    else if (strcasecmp(f, "sfft_pnm_ad")==0){
       flt_sfft_pnm_ad(sig, fmin, fmax, W,H);
    }
//    else if (strcasecmp(f, "taf")==0){
//      taf_ad(sig, fmin,fmax);
//    }
    else if (strcasecmp(f, "fit")==0){
      fit(sig, fmin,fmax);
    }
    else if (strcasecmp(f, "lockin")==0){
      lockin(sig, fmin,fmax);
    }

//    else if (strcasecmp(f, "crop")==0){
//      crop(sig, fmin,fmax);
//    }
    else throw Err() << "Unknown filter: " << f;
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
