#include "data.h"
#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <cmath>


// unpack data written by pico_rec into x-y text table

using namespace std;

void help(){
  cout << "pico_filter -- read and process pico_rec datafiles\n"
          "Usage: pico_filter [options] <filename>\n"
          "Options:\n"
          " -n <num>  -- channel number (default: 0)\n"
          " -f <name> -- filter type:\n"
          "              txt (default) -- Print a simple x-y text table.\n"
          "              pnm -- Make image with a raw signal.\n"
          "              fft_txt -- FFT of the whole signal, txt table. Rectangular window.\n"
          "              sfft_txt -- Text table with sliding fft. Blackman window.\n"
          "              sfft_pnm -- PNM with sliding fft. Blackman window.\n"
          "              sfft_pnm_ad -- Adaptive window, no smoothing. Blackman window.\n"
          "              taf         -- time-amp-fre table with adaptive window.\n"
          "              fit_fork    -- Fit fork signal (exponential decay, constant frequency).\n"
          " -W <num>  -- image width\n"
          " -H <num>  -- image height\n"
          " -F <num>  -- min frequency\n"
          " -G <num>  -- max frequency\n"
          " -T <num>  -- min time\n"
          " -U <num>  -- max time\n"
          " -w <num>  -- fft window (for fft_txt, fft_pnm), 1024 by default\n"
          " -h        -- write this help message and exit\n";
}

int
main(int argc, char *argv[]){
  try {

    /* default values */
    int n = 0; // channel
    char f_def[] = "txt";
    char *f = f_def;
    int W=1024, H=768;
    int win = 1024;
    double fmin = -HUGE_VAL;
    double fmax = +HUGE_VAL;
    double tmin = -HUGE_VAL;
    double tmax = +HUGE_VAL;

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hn:f:W:H:w:F:G:T:U:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'n': n = atoi(optarg); break;
        case 'f': f = optarg; break;
        case 'W': W = atoi(optarg); break;
        case 'H': H = atoi(optarg); break;
        case 'w': win = atoi(optarg); break;
        case 'F': fmin = atof(optarg); break;
        case 'G': fmax = atof(optarg); break;
        case 'T': tmin = atof(optarg); break;
        case 'U': tmax = atof(optarg); break;
        case 'h': help(); return 0;
      }
    }
    argc-=optind;
    argv+=optind;
    if (argc!=1) { help(); return 0; }
    const char *fname = argv[0];

    Data D(fname, n);
    if (strcasecmp(f, "txt")==0){
      D.print_txt();
    }
    else if (strcasecmp(f, "pnm")==0){
      D.print_pnm(W,H, 0xA00000);
    }
    else if (strcasecmp(f, "fft_txt")==0){
       D.print_fft_txt(fmin,fmax, tmin, tmax);
    }
    else if (strcasecmp(f, "sfft_txt")==0){
      D.print_sfft_txt(fmin,fmax, tmin, tmax, win);
    }
    else if (strcasecmp(f, "sfft_pnm")==0){
       D.print_sfft_pnm(32000,34000, tmin, tmax, win, W,H);
    }
    else if (strcasecmp(f, "sfft_pnm_ad")==0){
       D.print_sfft_pnm_ad(32000,34000, tmin, tmax, W,H);
    }
    else if (strcasecmp(f, "taf")==0){
      D.taf_ad(fmin,fmax, tmin, tmax);
    }
    else if (strcasecmp(f, "fork")==0){
      D.fit_fork(fmin,fmax, tmin, tmax);
    }
    else throw Err() << "Unknown filter: " << f;
  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
