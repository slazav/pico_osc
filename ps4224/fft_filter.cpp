#include "err.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <fftw3.h>

// unpack data written by pico4224block/pico4224stream and apply sliding fft
#define MAXCH 4

using namespace std;
int
main(){
  try {

  double sc_a=1, sc_b=1, t0=0, dt=1;
  bool use_a=true, use_b=true;

  // read metadata <name>:<value>
  while (!cin.eof()){
    string l;
    getline(cin, l);

    if (l=="") break;
    cout << "# " << l << "\n";
    // extract values for conversion
    size_t i = l.find(":");
    if (i!=string::npos){
      string key = l.substr(0,i);
      string val = l.substr(i+1);
      if (key=="sc_a") sc_a = atof(val.c_str());
      if (key=="sc_b") sc_b = atof(val.c_str());
      if (key=="t0") t0 = atof(val.c_str());
      if (key=="dt") dt = atof(val.c_str());
      if (key=="use_a") use_a = atoi(val.c_str());
      if (key=="use_b") use_b = atoi(val.c_str());
    }
  }
  int nchan = (int)use_a + (int)use_b;
  if (nchan>MAXCH) throw Err() << "too many channels: " << nchan;

  const int len=1024;
  double minf = 32000;
  double maxf = 34000;
  double df = 1/dt/len;

  int16_t ibuf[2*len];
  fftw_complex *cbufs[MAXCH];
  fftw_plan     plans[MAXCH];
  double sc[MAXCH];
  if (use_a && use_b) { sc[0] = sc_a; sc[1] = sc_b; }
  else if (use_a) {sc[0] = sc_a;}
  else if (use_b) {sc[0] = sc_b;}

  long int cnt=0;

  // two channels
  for (int c = 0; c<nchan; c++){
    cbufs[c] = fftw_alloc_complex(len);
    plans[c] = fftw_plan_dft_1d(len, cbufs[c], cbufs[c], FFTW_FORWARD, FFTW_ESTIMATE);
  }

  cout << scientific;
  while (!cin.eof()){
    // read data
    cin.read((char *)ibuf, nchan*len*sizeof(int16_t));
    if (cin.eof()) break; // we do not want partial reads

    // fill complex buffers
    for (int i=0; i<len; i++){
      for (int c = 0; c<nchan; c++){
        cbufs[c][i][0] = sc[c]*ibuf[nchan*i+c];
        cbufs[c][i][1] = 0;
      }
    }

    // do fft
    for (int c = 0; c<nchan; c++) fftw_execute(plans[c]);

    // print selected frequency range
    int i1 = floor(minf/df);
    int i2 = ceil(maxf/df);
    for (int i=max(0,i1); i<min(i2,len); i++){
      cout << (cnt+len/2)*dt << "\t" << i*df;
      for (int c = 0; c<nchan; c++)
        cout << "\t" << cbufs[c][i][0] << "\t" << cbufs[c][i][1];
      cout << "\n";
    }
    cnt+=len;
    cout << "\n";
    if ((cnt+len/2)*dt > 1) break;
  }
  for (int c = 0; c<nchan; c++){
    fftw_destroy_plan(plans[c]);
    fftw_free(cbufs[c]);
  }


  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
