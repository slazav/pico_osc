#include <fstream>
#include <cstdlib> // atof
#include <cmath>
#include <string>
#include <iomanip>
#include <complex>
#include <vector>
#include <fftw3.h>
#include "data.h"
#include "fit_signal.h"
#include "fft.h"
#include "dimage.h"

using namespace std;

// get_n()>0 get_c()>0 -- already checked

/******************************************************************/
void
flt_txt(const Signal & s){
  int n = s.get_n();
  cout << scientific;
  for (int i=0; i<n; i++){
    double t = s.t0 + s.dt*i;
    cout << s.t0 + s.dt*i;
    for (int c = 0; c<s.get_ch(); c++){
      cout << "\t" << s.get_val(c,i);
    }
    cout << "\n";
  }
}

/******************************************************************/

void
flt_pnm(const Signal & s, int W, int H){
  int colors[] = {0x00880000, 0x00008800, 0x00000088,
                  0x00888800, 0x00008888, 0x00880088,
                  0x00000000};
  int cn = sizeof(colors)/sizeof(int); // number of colors

  // the picture
  vector<int> pic(W*H, (int)0xFFFFFFFF);
  int N = s.get_n();

  // plot t=0 line
  int x0 = (-s.t0/s.dt*W)/N; 
  if (x0>=0 && x0<W) for (int y=0;y<H;y++){ pic[y*W+x0] = 0x00888888;}
  //// plot y=0 line
  // int y0 = H/2;
  // for (int x=0;x<W;x++){ pic[y0*W+x] = 0x00888888;}

  for (int ch = 0; ch<s.get_ch(); ch++){
    for (int i=0; i<N; i++){
      int x = ((double)i*W)/N; // be worry about int overfull
      int y = H/2 - (s.chan[ch][i]*H)/(1<<16);
      if (y<0 || y>=H || x<0 || x>=W) continue;
      pic[y*W+x] = colors[ch % cn];
    }
  }
  cout << "P6\n" << W << " " << H << "\n255\n";
  for (int y=0; y<H; y++){
    for (int x=0; x<W; x++){
      cout.write((char *)(&pic[y*W+x])+1, 3);
    }
  }
}


/******************************************************************/

void
flt_fft_txt(const Signal & s, double fmin, double fmax){

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
  vector<vector<double> > dat_im(cN), dat_re(cN);
  for (int c = 0; c<cN; c++){
    fft.run(s.chan[c].data(), s.chan[c].sc);
    dat_re[c] = fft.real(i1f,i2f);
    dat_im[c] = fft.imag(i1f,i2f);
  }

  cout << scientific;
  for (int i=i1f; i<i2f; i++){
    cout << df*i;
    for (int c = 0; c<cN; c++){
      cout << "\t" << dat_re[c][i-i1f] << "\t" << dat_im[c][i-i1f];
    }
    cout << "\n";
  }
}

/******************************************************************/

void
flt_fft_pow_avr(const Signal & s, double fmin, double fmax, int npts){

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

  double k = 2*s.dt/N; // convert power to V^2/Hz

  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
  vector<vector<double> > dat(cN);
  for (int c = 0; c<cN; c++){
    fft.run(s.chan[c].data(), s.chan[c].sc);
    dat[c] = fft.abs(i1f,i2f);
  }

  double fstep = (fmax-fmin)/npts;

  cout << scientific;
  vector<double> ss(cN, 0);
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    for (int c = 0; c<cN; c++) ss[c]+=pow(dat[c][i-i1f],2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin + fstep || i==i2f-1){
      cout << (i-0.5*(n-1))*df;
      for (int c = 0; c<cN; c++){
        cout << "\t" << k*ss[c]/n;
        ss[c]=0;
      }
      cout << "\n";
      n=0; fmin=i*df;
    }
  }

}

/******************************************************************/

void
flt_fft_pow_lavr(const Signal & s, double fmin, double fmax, int npts){

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

  double k = 2*s.dt/N; // convert power to V^2/Hz

  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
  vector<vector<double> > dat(cN);
  for (int c = 0; c<cN; c++){
    fft.run(s.chan[c].data(), s.chan[c].sc);
    dat[c] = fft.abs(i1f,i2f);
  }

  if (fmin==0) fmin=df;
  double fstep = pow(fmax/fmin, 1.0/npts);

  cout << scientific;
  vector<double> ss(cN, 0);
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    for (int c = 0; c<cN; c++) ss[c]+=pow(dat[c][i-i1f],2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin*fstep || i==i2f-1){
      cout << (i-0.5*(n-1))*df;
      for (int c = 0; c<cN; c++){
        cout << "\t" << k*ss[c]/n;
        ss[c]=0;
      }
      cout << "\n";
      n=0; fmin=i*df;
    }
  }
}

/******************************************************************/

void
flt_fft_pow_avr_corr(const Signal & s, double fmin, double fmax, int npts){

  int N = s.get_n();
  int cN  = s.get_ch();
  // we need two channels!
  if (N<2 || cN!=2) return;

  double k = 2*s.dt/N; // convert power to V^2/Hz

  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
  vector<double> dat1re, dat1im, dat2re, dat2im, dat0re, dat0im;

  // run fft on the first channel
  fft.run(s.chan[0].data(), s.chan[0].sc);
  // copy complex values
  dat1re = fft.real(i1f,i2f);
  dat1im = fft.imag(i1f,i2f);
  fft.run(s.chan[1].data(), s.chan[1].sc);
  dat2re = fft.real(i1f,i2f);
  dat2im = fft.imag(i1f,i2f);

  // correlation is a A*B^*
  // http://mathworld.wolfram.com/Cross-CorrelationTheorem.html
  for (int i=0; i<dat1re.size(); i++){
    dat0re.push_back(dat1re[i]*dat2re[i]+dat1im[i]*dat2im[i]);
    dat0im.push_back(dat1im[i]*dat2re[i]-dat1re[i]*dat2im[i]);
  }

  double fstep = (fmax-fmin)/npts;

  cout << scientific;
  double sre=0, sim=0;
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    sre+=dat0re[i-i1f];
    sim+=dat0im[i-i1f];
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin + fstep || i==i2f-1){
      cout << (i-0.5*(n-1))*df << "\t" << k*hypot(sre,sim)/n << "\n";
      sre=0; sim=0;
      n=0; fmin=i*df;
    }
  }

}

/******************************************************************/

void
flt_fft_pow_lavr_corr(const Signal & s, double fmin, double fmax, int npts){

  int N = s.get_n();
  int cN  = s.get_ch();
  // we need two channels!
  if (N<2 || cN!=2) return;

  double k = 2*s.dt/N; // convert power to V^2/Hz

  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
  vector<double> dat1re, dat1im, dat2re, dat2im, dat0re, dat0im;

  // run fft on the first channel
  fft.run(s.chan[0].data(), s.chan[0].sc);
  // copy complex values
  dat1re = fft.real(i1f,i2f);
  dat1im = fft.imag(i1f,i2f);
  fft.run(s.chan[1].data(), s.chan[1].sc);
  dat2re = fft.real(i1f,i2f);
  dat2im = fft.imag(i1f,i2f);

  // correlation is a A*B^*
  // http://mathworld.wolfram.com/Cross-CorrelationTheorem.html
  for (int i=0; i<dat1re.size(); i++){
    dat0re.push_back(dat1re[i]*dat2re[i]+dat1im[i]*dat2im[i]);
    dat0im.push_back(dat1im[i]*dat2re[i]-dat1re[i]*dat2im[i]);
  }

  if (fmin==0) fmin=df;
  double fstep = pow(fmax/fmin, 1.0/npts);

  cout << scientific;
  double sre=0, sim=0;
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    sre+=dat0re[i-i1f];
    sim+=dat0im[i-i1f];
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin*fstep || i==i2f-1){
      cout << (i-0.5*(n-1))*df << "\t" << k*hypot(sre,sim)/n << "\n";
      sre=0; sim=0;
      n=0; fmin=i*df;
    }
  }
}


/******************************************************************/
void
flt_sfft_txt(const Signal & s, double fmin, double fmax, int win) {

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch=0;

  FFT fft(win);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);

  for (int iw=0; iw<N-win; iw+=win){
    fft.run(s.chan[ch].data()+iw, s.chan[ch].sc, true);

    // print selected frequency range
    cout << scientific;
    for (int i=i1f; i<i2f; i++){
      cout << s.t0 + s.dt*(iw+win/2) << "\t" << i*df << "\t"
                << fft.real(i) << "\t" << fft.imag(i) << "\n";
    }
    cout << "\n";
  }
}

/******************************************************************/

void
flt_sfft_pnm(const Signal & s, double fmin, double fmax, int win, int W, int H) {

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch=0;

  FFT fft(win);
  int i1f, i2f;
  double df;
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);

  dImage pic(W,H,0);

  for (int x = 0; x<W; x++){
    int il = ((double)(N-win)*x)/(W-1); // be worry about int overfull
    fft.run(s.chan[ch].data()+il, s.chan[ch].sc, true);
    for (int y = 0; y<H; y++){
      // convert y -> f
      double f = fmin + ((fmax-fmin)*(double)(H-1-y))/H;
      int fi = floor(f/df);
      if (fi<0) fi=0;
      if (fi>win-2) fi=win-2;
      double v1 = fft.abs(fi);
      double v2 = fft.abs(fi+1);
      double v = (v1 + (f/df-fi)*(v2-v1))/win;
      pic.set(x,y,v);
    }
  }
  pic.print_pnm();
}

/******************************************************************/

void
flt_sfft_pnm_ad(const Signal & s, double fmin, double fmax, int W, int H) {

  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch=0;

  int i1f, i2f;
  double df;

  FFT fft(N);
  fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);

  // min window: we have NM points in fmin:fmax range
  // max window: we have NM points in tmin:tmax range
  int NM = 5;
  int wmin = floor(NM/(fmax-fmin)/s.dt);
  int wmax = ceil(N/NM);

  // first pass
  vector<int> start; // start points for the full calculation
  {
    int win=wmin;
    FFT fft(win);
    fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
    vector<double> vp(win,0); // previous step data

    // for each x point
    for (int iw=0; iw<N-win; iw+=win){
      fft.run(s.chan[ch].data()+iw, s.chan[ch].sc, true);

      // calculate normalized distance from previous point
      double d0=0, d1=0;
      vector<double> vn(win,0); // new data
      for (int i=i1f; i<i2f; i++){
        vn[i-i1f] = fft.abs(i);
        d1 += pow(vn[i-i1f]-vp[i-i1f], 2);
        d0 += fabs(vn[i-i1f]+vp[i-i1f]);
      }
      bool change = sqrt(d1)/d0>0.05;
      bool longwin = start.size() && (iw+win-start[start.size()-1] > wmax);
      if (change || longwin){
        vp.swap(vn);
        start.push_back(iw);
      }
    }
    // move last point to the data end
    start[start.size()-1] = N;
  }

  // second pass
  dImage pic(W,H,0);
  for (int is = 0; is<start.size()-1; is++){
    int win = start[is+1]-start[is];

    FFT fft(win);
    fft.get_ind(s.dt, &fmin, &fmax, &i1f, &i2f, &df);
    fft.run(s.chan[ch].data()+start[is], s.chan[ch].sc, true);

    size_t x1 = ((double)start[is]*W)/N; // be worry about int overfull
    size_t x2 = ((double)start[is+1]*W)/N;
    for (int y = 0; y<H; y++){
      // convert y -> f
      double f = fmin + ((fmax-fmin)*(H-1-y))/H;
      int fi = floor(f/df);
      if (fi<0) fi=0;
      if (fi>win-2) fi=win-2;
      double v1 = fft.abs(fi);
      double v2 = fft.abs(fi+1);
      double v = (v1 + (f/df-fi)*(v2-v1))/win;
      for (int x=x1; x<x2; x++) pic.set(x,y,v);
    }
  }
  pic.print_pnm();
}

/******************************************************************/

void
fit(const Signal & s, double fmin, double fmax) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch = 0;

  vector<double> ret = ::fit_signal(
    s.chan[ch].data(), N, s.chan[ch].sc, s.dt, s.t0, fmin, fmax);

  cout << s.t0abs << " "
       << setprecision(12) << ret[0] << " "
       << setprecision(6)  << ret[1] << " "
       << setprecision(6)  << ret[2] << " "
       << setprecision(6)  << ret[3] << "\n";

}

/******************************************************************/

void
fit2(Signal & s, double fmin, double fmax) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch = 0;

  // find first signal (largest amplitude)
  vector<double> ret1 = ::fit_signal(
    s.chan[ch].data(), N, s.chan[ch].sc, s.dt, s.t0, fmin, fmax);

  double f1 = ret1[0];
  double r1 = ret1[1];
  double a1 = ret1[2]/2;
  double p1 = ret1[3];

  // subtract it
  for (int i=0; i<s.get_n(); i++){
    double t = s.t0+i*s.dt;
    s.set_val(ch,i, s.get_val(ch,i) - a1*exp(-t*r1)*sin(2*M_PI*f1*t+p1) );
  }

  // find second signal
  vector<double> ret2 = ::fit_signal(
    s.chan[ch].data(), N, s.chan[ch].sc, s.dt, s.t0, fmin, fmax);

  // sort by frequency
  if (ret1[2] < ret2[2]) ret1.swap(ret2);

  cout << s.t0abs << " "
       << setprecision(12) << ret1[0] << " "
       << setprecision(6)  << ret1[1] << " "
       << setprecision(6)  << ret1[2] << " "
       << setprecision(6)  << ret1[3] << " "
       << setprecision(12) << ret2[0] << " "
       << setprecision(6)  << ret2[1] << " "
       << setprecision(6)  << ret2[2] << " "
       << setprecision(6)  << ret2[3] << "\n";
}

/******************************************************************/
void
lockin(const Signal & s, double fmin, double fmax) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1) return;
  if (cN<2) throw Err() << "at least two channels are needed";
  int ch_sig = 0;
  int ch_ref = 1;

  // get frequncy and phase of the reference signal
  vector<double> ret = ::fit_signal(
    s.chan[ch_ref].data(), N, s.chan[ch_ref].sc, s.dt, s.t0, fmin, fmax);
  double fre = ret[0];
  double ph  = ret[3];

  double ss1=0, ss2=0;
  for (int i=0; i<N; i++){
    double v = s.chan[ch_sig][i]*s.chan[ch_sig].sc;
    ss1+= v*sin(2*M_PI*fre*s.dt*i + ph);
    ss2+= v*cos(2*M_PI*fre*s.dt*i + ph);
  }
  cout << setprecision(12) << fre << " "
       << setprecision(6)  << 2*ss1/N << " "
       << setprecision(6)  << 2*ss2/N << "\n";

}

/******************************************************************/
void
minmax(const Signal & s) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1) return;

  for (int c = 0; c<cN; c++){
    double min,max;
    min = max = s.chan[c][0]*s.chan[c].sc;
    for (int i=1; i<N; i++){
      double v = s.chan[c][i]*s.chan[c].sc;
      if (v<min) min = v;
      if (v>max) max = v;
    }
    cout << setprecision(6)  << min << " "
         << setprecision(6)  << max << "\n";
  }
}


/******************************************************************/
/*
void
crop(const Signal & s, double fmin, double fmax) {

  set_sig_ind(fmin,fmax,tmin,tmax);
  int win = N;
  fftw_complex  *cbuf;
  fftw_plan     plan1, plan2;
  cbuf = fftw_alloc_complex(win);
  plan1 = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_FORWARD,  FFTW_ESTIMATE);
  plan2 = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_BACKWARD, FFTW_ESTIMATE);

  for (int i=0; i<N; i++){
    cbuf[i-0][0] = s.chan[ch].sc*data[i];
    cbuf[i-0][1] = 0;
  }
  // do fft
  fftw_execute(plan1);
  for (int i=0; i<win; i++){
    if (!(i>=i1f && i<i2f) && !(i>=win-i2f && i<win-i1f))
       cbuf[i][0] = cbuf[i][1] = 0.0;
  }
  fftw_execute(plan2);
  for (int i=0; i<N; i++){
    cout << t0+dt*i << "\t"
         << s.chan[ch].sc*data[i] << "\t"
         << cbuf[i][0]/win << "\n";
  }

  fftw_destroy_plan(plan1);
  fftw_destroy_plan(plan2);
  fftw_free(cbuf);
}

*/