#include <cstdlib>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <cstring>
#include <cmath>

#include "signal/signal.h"
#include "str2vec/str2vec.h"
#include "filters.h"


// unpack data written by pico_rec into x-y text table

using namespace std;

// print help
void help(){
  cout << "pico_filter -- read and process pico_rec datafiles\n"
          "Usage: pico_filter [options] <filename> [filter options]\n"
          "       pico_filter [options] -M <filename> ... -- [filter options]\n"
          "Options:\n"
          " -h        -- write this help message and exit\n"
          " -f <name> -- filter name\n"
          " -P <pars> -- Find pulse and set t0. <pars> are comma- or space-separated values:\n"
          "              channel (0,1,etc.), threshold (0..1, default 0.9),\n"
          "              start time, stop time.\n"
          " -c <chan> -- Comma- or space-separated channel numbers (\"0\", \"1\", \"1,0\", etc.).\n"
          "              Different filters use different number of channels.\n"
          " -T <num>  -- min time (default -infinity)\n"
          " -U <num>  -- max time (default +infinity)\n"
          "              If -T or -U is set the signal is cut to the specified time range before filtering.\n"
          " -M        -- Load multiple signals. Filter options should be separated by '--'.\n"
          "              Signals are aligned by t=0 point and cropped to the shortest signal.\n"
          "              -c, -T, -U options are applied to each signal before joining.\n"
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
          "     sfft_pow      -- Sliding fft with Blackman window, text table (T,F,A).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -w <value>  -- Window length (points)\n"
          "       -a          -- average all channels instead of using the first one\n"
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
          "     sfft_peak -- Sliding fft with Blackman window, detect peak near some line.\n"
          "       Hints for peak position is taken from parameters.\n"
          "       Table with time, peak frequency, peak amplitude, base amplitude\n"
          "       is printed\n"
          "       Options:\n"
          "       -T <value>  -- time array (comma- or space-separated)\n"
          "       -F <value>  -- frequency array (comma- or space-separated)\n"
          "       -w <value>  -- window length (points, default 1024)\n"
          "       -s <value>  -- window step (points, default equals to window)\n"
          "       -f <value>  -- frequency window (default 20/dt/window)\n"
          "     sfft_pnm      -- Sliding fft with Blackman window, pnm picture\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -A <value>  -- low amplitude limit\n"
          "       -B <value>  -- high amplitude limit\n"
          "       -w <value>  -- Window length (points)\n"
          "       -W <value>  -- image width, pixels (default: 1024)\n"
          "       -H <value>  -- image height, pixels (default: 768)\n"
          "       -S <value>  -- width of color scale, pixels (default: 16)\n"
          "       -l          -- use log scale\n"
          "       -g <value>  -- color gradient (default \"KRYW\")\n"
          "       -a          -- average all channels instead of using the first one\n"
          "     sfft_pnm_ad   -- Adaptive window, no smoothing. Blackman window.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -A <value>  -- low amplitude limit\n"
          "       -B <value>  -- high amplitude limit\n"
          "       -W <value>  -- image width, pixels (default: 1024)\n"
          "       -H <value>  -- image height, pixels (default: 768)\n"
          "       -S <value>  -- width of color scale, pixels (default: 16)\n"
          "       -l          -- use log scale\n"
          "       -g <value>  -- color gradient (default \"KRYW\")\n"
          "     fit -- Fit fork signal (exponential decay, constant frequency).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "     fitn -- Fit N fork signals (sort by frequency).\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -N <value>  -- number of signals (default 1)\n"
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
          "     peak          -- peak position on FFT absolute value.\n"
          "       Options:\n"
          "       -F <value>  -- low frequency limit\n"
          "       -G <value>  -- high frequency limit\n"
          "       -a          -- average all channels instead of using the first one\n"
          "     dc            -- Print mean value for each channel.\n"
          "     minmax        -- Print min/max values for each channel.\n"
          "     overload      -- Print overload flag for each channel.\n"
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
    bool mult = false;

    std::vector<double> pulse_pars;
    double pulse_th = 0.9;
    int pulse_ch = -1;
    double pulse_t1 = tmin;
    double pulse_t2 = tmax;

    /* parse  options */
    opterr=0;
    while(1){
      int c = getopt(argc, argv, "+hf:c:T:U:C:D:P:M");
      if (c==-1) break;
      switch (c){
        case '?': throw Err() << "Unknown option: -" << (char)optopt;
        case ':': throw Err() << "No argument: -" << (char)optopt;
        case 'h': help(); return 0;
        case 'f': flt = optarg; break;
        case 'c': cn = optarg; break;
        case 'T': tmin = atof(optarg); break;
        case 'U': tmax = atof(optarg); break;
        case 'P': pulse_pars = str2dvec(optarg); 
                  if (pulse_pars.size()>0) pulse_ch = pulse_pars[0];
                  if (pulse_pars.size()>1) pulse_th = pulse_pars[1];
                  if (pulse_pars.size()>2) pulse_t1 = pulse_pars[2];
                  if (pulse_pars.size()>3) pulse_t2 = pulse_pars[3];
                  break;
        case 'M': mult = true; break;
      }
    }
    argc-=optind;
    argv+=optind;
    optind=1;
    if (argc<1) { help(); return 0; }
    if (flt==NULL) throw Err() << "Filter is not specified, use -f option\n";

    Signal sig;
    int i;
    for (i=0; i<(mult?argc:1); i++){
      if (mult && strcmp(argv[i],"--")==0) {i++; break;}
      std::ifstream ff(argv[i]);
      Signal sig1 = read_signal(ff);
      /* crop time, rearrange channels */
      if (pulse_ch>=0) sig1.find_pulse(pulse_ch, pulse_th, pulse_t1, pulse_t2);
      sig1.crop_t(tmin, tmax);
      if (strlen(cn)>0)  sig1.crop_c(str2ivec(cn));
      if (sig1.get_n()<1 || sig1.get_ch()<1) throw Err() << "empty signal: " << argv[i];
      sig.add(sig1);
    }
    argc-=i-1;
    argv+=i-1;


    if (strcasecmp(flt, "txt")==0)                    flt_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "pnm")==0)               flt_pnm(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_txt")==0)           flt_fft_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_pow")==0)           flt_fft_pow(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fft_pow_corr")==0)      flt_fft_pow_corr(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_txt")==0)          flt_sfft_txt(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_pow")==0)          flt_sfft_pow(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_int")==0)          flt_sfft_int(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_diff")==0)         flt_sfft_diff(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_peaks")==0)        flt_sfft_peaks(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_peak")==0)         flt_sfft_peak(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_pnm")==0)          flt_sfft_pnm(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sfft_pnm_ad")==0)       flt_sfft_pnm_ad(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fit")==0)               fit(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "fitn")==0)              fitn(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "lockin")==0)            lockin(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "slockin")==0)           slockin(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "peak")==0)              peak(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "dc")==0)                dc(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "minmax")==0)            minmax(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "overload")==0)          overload(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sigf")==0)              flt_sigf(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "sig")==0)               flt_sig(std::cout, sig, argc, argv);
    else if (strcasecmp(flt, "wav")==0)               flt_wav(std::cout, sig, argc, argv);
    else throw Err() << "Unknown filter: " << flt;
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
