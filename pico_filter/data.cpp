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

/******************************************************************/
// constructor -- read one data channel from a file
Data::Data(const char *fname, int n){
  ifstream ff(fname);
  if (ff.fail()) throw Err() << "Can't read file: " << fname;

  t0=0;
  dt=1;
  sc=1;
  t0abs=0;

  // read metadata <name>: <value>
  while (!ff.eof()){
    // read line
    string line;
    getline(ff,line);

    // after * we have data array!
    if (line == "*") break;

    // remove comments
    size_t nc = line.find('#');
    if (nc!=string::npos) line = line.substr(0,nc);

    string key, val;

    // find key: value pair
    nc = line.find(':');
    if (nc!=string::npos){
      key = line.substr(0,nc);
      val = line.substr(nc+1);
    }
    else  val = line;

    // split into words
    istringstream ss1(key), ss2(val);
    vector<string> keyw, valw;
    while (!ss1.eof()){ string w; ss1>>w; if (w!="") keyw.push_back(w); }
    while (!ss2.eof()){ string w; ss2>>w; if (w!="") valw.push_back(w); }

    // empty line
    if (keyw.size()==0 && valw.size()==0) continue;

    if (keyw.size()!=1) throw Err() << "Broken file: " << line;
    key=keyw[0];

    // collect useful parameters
    if (key == "dt"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      dt = atof(valw[0].c_str());
    }
    if (key == "t0"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      t0 = atof(valw[0].c_str());
    }
    if (key == "t0abs"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      t0abs = atoi(valw[0].c_str());
    }
    if (key == "data"){
      if (valw.size()<3) throw Err() << "Broken file: " << line;
      if (atoi(valw[0].c_str())==n) sc = atof(valw[2].c_str());
    }
    if (key == "data_num"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      num = atoi(valw[0].c_str());
    }
  }

  // read data array
  int bufsize = 1<<16;
  int cnt = 0;
  vector<int16_t> buf(bufsize*num);
  data.clear();
  while (!ff.eof()){
    ff.read((char *)buf.data(), bufsize*num*sizeof(int16_t));
    int len = ff.gcount()/num/sizeof(int16_t);
    data.resize(cnt+len);
    for (int i=0; i<len; i++){
      data[cnt+i] = buf[i*num+n];
    }
    cnt+=len;
  }

  //set ind
  i1t=0; i2t=lent=data.size();
  i1f=0; i2f=lenf=data.size();
  df = 1/dt/data.size();
}

/******************************************************************/
// check ranges and update signal indices
void
Data::set_sig_ind(double &fmin, double &fmax, double &tmin, double &tmax, int win){
  // adjust tmin/tmax/fmin/fmax
  if (tmax<tmin) swap(tmax, tmin);
  if (tmax > t0+data.size()*dt) tmax = t0+data.size()*dt;
  if (tmin < t0)   tmin = t0;
  if (fmax<fmin) swap(fmax, fmin);
  if (fmax > 0.5/dt) fmax = 0.5/dt;
  if (fmin > 0.5/dt) fmin = 0.5/dt;
  if (fmin < 0)    fmin = 0;
  // select time indices
  i1t = max(0.0, floor((tmin-t0)/dt));
  i2t = min(1.0*data.size(), ceil((tmax-t0)/dt));
  lent = i2t-i1t;
  if (lent<1) throw Err() << "Error: too small time range: " << tmin << " - " << tmax;
  // select frequency indices
  if (win==0) win = lent;
  df = 1/dt/win;
  i1f = max(0.0, floor(fmin/df));
  i2f = min(0.5*win, ceil(fmax/df));
  lenf = i2f-i1f;
  if (lent<1) throw Err() << "Error: too small frequency range: " << fmin << " - " << fmax;
}

/******************************************************************/
void
Data::print_txt() const{
  cout << scientific;
  for (int i=0; i<data.size(); i++)
    cout << t0+dt*i << "\t" << sc*data[i] << "\n";
}

/******************************************************************/
void
Data::print_pnm(int w, int h, int color) const{
  vector<int> pic(w*h, (int)0);
  int cmax=0;
  for (int i=0; i<data.size(); i++){
    int x = ((double)i*w)/data.size(); // be worry about int overfull
    int y = h/2 - (data[i]*h)/(1<<16);
    if (y<0 || y>=h || x<0 || x>=w) continue;
    pic[y*w+x]++;
    if (cmax < pic[y*w+x]) cmax = pic[y*w+x];
  }
  unsigned char r = (color & 0xFF);
  unsigned char g = ((color>>8) & 0xFF);
  unsigned char b = ((color>>16) & 0xFF);

  cout << "P6\n" << w << " " << h << "\n255\n";
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){

    unsigned char r1 = pic[y*w+x] ? r:0xFF;
    unsigned char g1 = pic[y*w+x] ? g:0xFF;
    unsigned char b1 = pic[y*w+x] ? b:0xFF;

      cout.write((char *)&r1, 1);
      cout.write((char *)&g1, 1);
      cout.write((char *)&b1, 1);
    }
  }
}

/******************************************************************/
void
Data::print_fft_txt(double fmin, double fmax, double tmin, double tmax){

  set_sig_ind(fmin,fmax,tmin,tmax);
  FFT fft(lent);
  fft.run(data.data()+i1t, sc);

  // print selected frequency range
  cout << scientific;
  for (int i=i1f; i<i2f; i++)
    cout << i*df << "\t" << fft.real(i) << "\t" << fft.imag(i) << "\n";
}

/******************************************************************/
void
Data::print_fft_pow_avr(double fmin, double fmax, double tmin, double tmax, int N){

  set_sig_ind(fmin,fmax,tmin,tmax);
  FFT fft(lent);
  fft.run(data.data()+i1t, sc);

  double k = 2*dt/lent; // convert power to V^2/Hz

  double fstep = (fmax-fmin)/N;

  cout << scientific;
  double s = 0;
  int n = 0; // number of samples in the average
  int j = 0; // output counter
  for (int i=i1f; i<i2f; i++){
    s+=pow(fft.abs(i),2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin + fstep*j || i==i2f-1){
      cout << (i-0.5*n)*df << "\t" << k*s/n << "\n";
      s=0; n=0; j++;
    }
  }

}

/******************************************************************/
void
Data::print_fft_pow_lavr(double fmin, double fmax, double tmin, double tmax, int N){

  set_sig_ind(fmin,fmax,tmin,tmax);
  FFT fft(lent);
  fft.run(data.data()+i1t, sc);

  double k = 2*dt/lent; // convert power to V^2/Hz

  // frequency step
  if (fmin==0) fmin=df;
  double fstep = pow(fmax/fmin, 1.0/N);

  // print selected frequency range
  cout << scientific;
  double s = 0;
  int n = 0; // number of samples in the average
  int j = 0; // output counter
  for (int i=i1f; i<i2f; i++){
    s+=pow(fft.abs(i),2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin*pow(fstep,j+1) || i==i2f-1){
      cout << (i-0.5*n)*df << "\t" << k*s/n << "\n";
      s=0; n=0; j++;
    }
  }
}

/******************************************************************/
void
Data::print_sfft_txt(double fmin, double fmax, double tmin, double tmax, int win) {

  set_sig_ind(fmin,fmax,tmin,tmax, win);
  FFT fft(win);

  for (int iw=i1t; iw<i2t-win; iw+=win){
    fft.run(data.data()+iw, sc, true);

    // print selected frequency range
    cout << scientific;
    for (int i=i1f; i<i2f; i++){
      cout << t0+dt*(iw+win/2) << "\t" << i*df << "\t"
                << fft.real(i) << "\t" << fft.imag(i) << "\n";
    }
    cout << "\n";
  }
}

/******************************************************************/
void
Data::print_sfft_pnm(double fmin, double fmax, double tmin, double tmax, int win, int w, int h) {

  set_sig_ind(fmin,fmax,tmin,tmax, win);
  FFT fft(win);
  dImage pic(w,h,0);

  for (int x = 0; x<w; x++){
    int il = i1t + ((double)(lent-win)*x)/(w-1); // be worry about int overfull
    fft.run(data.data()+il, sc, true);
    for (int y = 0; y<h; y++){
      // convert y -> f
      double f = fmin + ((fmax-fmin)*(double)(h-1-y))/h;
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
Data::crop(double fmin, double fmax, double tmin, double tmax) {

  set_sig_ind(fmin,fmax,tmin,tmax);
  int win = lent;
  fftw_complex  *cbuf;
  fftw_plan     plan1, plan2;
  cbuf = fftw_alloc_complex(win);
  plan1 = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_FORWARD,  FFTW_ESTIMATE);
  plan2 = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_BACKWARD, FFTW_ESTIMATE);

  for (int i=i1t; i<i2t; i++){
    cbuf[i-i1t][0] = sc*data[i];
    cbuf[i-i1t][1] = 0;
  }
  // do fft
  fftw_execute(plan1);
  for (int i=0; i<win; i++){
    if (!(i>=i1f && i<i2f) && !(i>=win-i2f && i<win-i1f))
       cbuf[i][0] = cbuf[i][1] = 0.0;
  }
  fftw_execute(plan2);
  for (int i=i1t; i<i2t; i++){
    cout << t0+dt*i << "\t"
         << sc*data[i] << "\t"
         << cbuf[i-i1t][0]/win << "\n";
  }

  fftw_destroy_plan(plan1);
  fftw_destroy_plan(plan2);
  fftw_free(cbuf);
}

/******************************************************************/
void
Data::print_sfft_pnm_ad(double fmin, double fmax, double tmin, double tmax, int w, int h) {

  set_sig_ind(fmin,fmax,tmin,tmax);

  // min window: we have NM points in fmin:fmax range
  // max window: we have NM points in tmin:tmax range
  int NM = 5;
  int wmin = floor(NM/(fmax-fmin)/dt);
  int wmax = ceil((tmax-tmin)/NM/dt);


  // first pass
  vector<int> start; // start points for the full calculation
  {
    int win=wmin;
    set_sig_ind(fmin,fmax,tmin,tmax, win);
    FFT fft(win);
    vector<double> vp(lenf,0); // previous step data

    // for each x point
    for (int iw=i1t; iw<i2t-win; iw+=win){
      fft.run(data.data()+iw, sc, true);

      // calculate normalized distance from previous point
      double d0=0, d1=0;
      vector<double> vn(lenf,0); // new data
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
    start[start.size()-1] = i2t;
  }

  // second pass
  dImage pic(w,h,0);
  for (int is = 0; is<start.size()-1; is++){
    int win = start[is+1]-start[is];
    set_sig_ind(fmin,fmax,tmin,tmax, win);
    FFT fft(win);
    fft.run(data.data()+start[is], sc, true);

    size_t x1 = ((double)start[is]*w)/lent; // be worry about int overfull
    size_t x2 = ((double)start[is+1]*w)/lent;
    for (int y = 0; y<h; y++){
      // convert y -> f
      double f = fmin + ((fmax-fmin)*(h-1-y))/h;
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
Data::taf_ad(double fmin, double fmax, double tmin, double tmax) {

  set_sig_ind(fmin,fmax,tmin,tmax);

  // min window: we have NM points in fmin:fmax range
  // max window: we have NM points in tmin:tmax range
  int NM = 5;
  int wmin = floor(NM/(fmax-fmin)/dt);
  int wmax = ceil((tmax-tmin)/NM/dt);

  // first pass with minimal window
  vector<int> start; // start points for the full calculation
  {
    int win=wmin;
    set_sig_ind(fmin,fmax,tmin,tmax, win);
    FFT fft(win);
    vector<double> vp(lenf,0); // previous step data

    // for each x point
    for (int iw=i1t; iw<i2t-win; iw+=win){
      fft.run(data.data()+iw, sc, true);

      // calculate normalized distance from previous point
      double d0=0, d1=0;
      vector<double> vn(lenf,0); // new data
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
    start[start.size()-1] = i2t;
  }

  // second pass
  for (int is = 0; is<start.size()-1; is++){
    int win = start[is+1]-start[is];
    set_sig_ind(fmin,fmax,tmin,tmax, win);
    FFT fft(win);
    fft.run(data.data()+start[is], sc, true);

    // print point: time, amplitude, frequency
    double t = t0 + dt*(start[is]+start[is+1])/2;
    double amp=0;
    for (int i=i1f; i<i2f; i++) amp+=pow(fft.abs(i),2);
    amp = sqrt(amp)/lenf/11;

    double A,B,C;
    fft.find_max_par(i1f,i2f,df, A,B,C);
    double fre = -B/(2*A);

    cout << setprecision(6)  << t << " "
         << setprecision(6)  << amp << " "
         << setprecision(12) << fre << "\n";

  }
}

/******************************************************************/
void
Data::fit_signal(double fmin, double fmax, double tmin, double tmax) {

  set_sig_ind(fmin,fmax,tmin,tmax);
  vector<double> buf(lent);
  for (int i=i1t; i<i2t; i++)
    buf[i-i1t] = data[i] * sc;

  vector<double> ret = ::fit_signal(buf.data(), lent, dt, tmin, fmin, fmax);

  cout << t0abs << " "
       << setprecision(12) << ret[0] << " "
       << setprecision(6)  << ret[1] << " "
       << setprecision(6)  << ret[2] << " "
       << setprecision(6)  << ret[3] << "\n";

}
