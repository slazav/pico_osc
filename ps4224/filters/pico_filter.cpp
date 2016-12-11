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
          "              TXT (default) -- txt table\n"
          "              PNM\n"
          "              TXT_SFFT\n"
          "              PNM_SFFT\n"
          " -W <num>  -- image width\n"
          " -H <num>  -- image height\n"
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

    /* parse  options */
    while(1){
      int c = getopt(argc, argv, "hn:f:W:H:");
      if (c==-1) break;
      switch (c){
        case -1: return 0; /* end*/
        case '?':
        case ':': continue; /* error msg is printed by getopt*/
        case 'n': n = atoi(optarg); break;
        case 'f': f = optarg; break;
        case 'W': W = atoi(optarg); break;
        case 'H': H = atoi(optarg); break;
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
    else if (strcasecmp(f, "sfft_txt")==0){
      D.print_sfft_txt(30000,35000, 100/32900.0, HUGE_VAL, 512);
    }
    else if (strcasecmp(f, "fork")==0){
      D.fit_fork(30000,35000, 100/32900.0, HUGE_VAL);
    }

//     D.print_fft_txt(30000,35000, 100/32900.0, HUGE_VAL);

//    D.print_sfft_pnm(32000,34000, 100/32900.0, HUGE_VAL);
//    D.print_sfft_pnm(32000,34000, -HUGE_VAL, HUGE_VAL);

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
