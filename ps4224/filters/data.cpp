#include <fstream>
#include <cstdlib> // atof
#include <cmath>
#include <string>
#include <iomanip>
#include <complex>
#include <vector>
#include <fftw3.h>
#include "../err.h"
#include "data.h"

using namespace std;


/******************************************************************/
// constructor -- read one data channel from a file
Data::Data(const char *fname, int n){
  ifstream ff(fname);
  if (ff.fail()) throw Err() << "Can't read file: " << fname;

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
  int bufsize = 1<<12;
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

}

/******************************************************************/
void
Data::print_txt() const{
  cout << scientific;
  for (int i=0; i<data.size(); i++){
    cout << t0+dt*i << "\t" << sc*data[i] << "\n";
  }
}

/******************************************************************/
void
Data::print_pnm(int w, int h, int color) const{
  vector<int> pic(w*h, (int)0);
  int cmax=0;
  for (int i=0; i<data.size(); i++){
    int x = (i*w)/data.size();
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
Data::print_fft_txt(double fmin, double fmax, double tmin, double tmax) const{

  // select time indices
  size_t i1 = max(0.0, floor((tmin-t0)/dt));
  size_t i2 = min(1.0*data.size(), ceil((tmax-t0)/dt));

  size_t len = i2-i1;

  fftw_complex  *cbuf;
  fftw_plan     plan;
  cbuf = fftw_alloc_complex(len);
  plan = fftw_plan_dft_1d(len, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);
  double df = 1/dt/len;

  // fill complex buffers
  for (int i=i1; i<i2; i++){
    cbuf[i-i1][0] = sc*data[i];
    cbuf[i-i1][1] = 0;
  }
  // do fft
  fftw_execute(plan);

  // print selected frequency range
  i1 = floor(fmin/df);
  i2 = ceil(fmax/df);
  cout << scientific;
  for (int i=max((size_t)0,i1); i < min(i2,len); i++){
    cout << i*df << "\t" << cbuf[i][0] << "\t" << cbuf[i][1] << "\n";
  }

  fftw_destroy_plan(plan);
  fftw_free(cbuf);
}

/******************************************************************/
void
Data::print_sfft_txt(double fmin, double fmax, double tmin, double tmax, int win) const{

  if (tmax<tmin) swap(tmax, tmin);
  if (fmax<fmin) swap(fmax, fmin);
  if (tmax > t0+data.size()*dt) tmax = t0+data.size()*dt;
  if (tmin < t0) tmin = t0;

  if (fmax > 1/dt) fmax = 1/dt;
  if (tmin < 0)    fmin = 0;

  // select time indices
  size_t i1 = max(0.0, floor((tmin-t0)/dt));
  size_t i2 = min(1.0*data.size(), ceil((tmax-t0)/dt));
  size_t len = i2-i1;

  // select frequency indices
  double df = 1/dt/win;
  size_t i1f = max(0.0, floor(fmin/df));
  size_t i2f = min(1.0*win, ceil(fmax/df));

  fftw_complex  *cbuf;
  fftw_plan     plan;
  cbuf = fftw_alloc_complex(win);
  plan = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);

  for (int iw=i1; iw<i2-win; iw+=win){
    // fill array and do fft
    for (int i=0; i<win; i++){
      cbuf[i][0] = sc*data[iw+i];
      cbuf[i][1] = 0;
    }
    fftw_execute(plan);

    // print selected frequency range
    cout << scientific;
    for (int i=i1f; i<i2f; i++){
      cout << t0+dt*(iw+win/2) << "\t" << i*df << "\t"
                << cbuf[i][0] << "\t" << cbuf[i][1] << "\n";
    }
    cout << "\n";
  }
  fftw_destroy_plan(plan);
  fftw_free(cbuf);
}


/******************************************************************/
void
Data::print_sfft_pnm(double fmin, double fmax, double tmin, double tmax) const{


  // adjust tmin/tmax/fmin/fmax
  if (tmax<tmin) swap(tmax, tmin);
  if (tmax > t0+data.size()*dt) tmax = t0+data.size()*dt;
  if (tmin < t0) tmin = t0;
  if (fmax<fmin) swap(fmax, fmin);
  if (fmax > 1/dt) fmax = 1/dt;
  if (fmin < 0)    fmin = 0;

  // min window: we have NM points in fmin:fmax range
  // max window: we have NM points in tmin:tmax range
  int NM = 10;
  int wmin = floor(NM/(fmax-fmin)/dt);
  int wmax = ceil((tmax-tmin)/NM/dt);

  // select time indices
  size_t i1 = max(0.0, floor((tmin-t0)/dt));
  size_t i2 = min(1.0*data.size(), ceil((tmax-t0)/dt));
  size_t len = i2-i1;

  vector<int> start; // start points for the full calculation

  // first pass
  {
    int win = wmin;
    fftw_complex  *cbuf;
    fftw_plan     plan;
    cbuf = fftw_alloc_complex(win);
    plan = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);
    double df = 1/dt/win;

    // frequency range
    size_t i1f = max(0.0, floor(fmin/df));
    size_t i2f = min(1.0*win, ceil(fmax/df));
    size_t lenf = i2f-i1f;
    vector<double> vp(lenf,0); // previous step data

    // for each x point
    for (int iw=i1; iw<i2-win; iw+=win){
      // fill array and do fft
      for (int i=0; i<win; i++){
        cbuf[i][0] = sc*data[iw+i];
        cbuf[i][1] = 0;
      }
      fftw_execute(plan);

      // calculate normalized distance from previous point
      double d0=0, d1=0;
      vector<double> vn(lenf,0); // new data
      for (int i=i1f; i<i2f; i++){
        vn[i-i1f] = hypot(cbuf[i][0], cbuf[i][1]);
        d1 += pow(vn[i-i1f]-vp[i-i1f], 2);
        d0 += fabs(vn[i-i1f]+vp[i-i1f]);
      }
      bool change = sqrt(d1)/d0>0.14;
      bool longwin = start.size() && (iw+win-start[start.size()-1] > wmax);
      if (change || longwin){
        vp.swap(vn);
        start.push_back(iw);
        cerr << iw << " " << sqrt(d1)/d0 << "\n";
      }
    }
    fftw_destroy_plan(plan);
    fftw_free(cbuf);
    start[start.size()-1] = i2;
  }

  // second pass
  int w = 1024, h = 768;
  vector<double> pic(w*h,0);
  for (int is = 0; is<start.size()-1; is++){
    int win = start[is+1]-start[is];

    fftw_complex  *cbuf;
    fftw_plan     plan;
    cbuf = fftw_alloc_complex(win);
    plan = fftw_plan_dft_1d(win, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);
    double df = 1/dt/win;

    // frequency range
    size_t i1f = max(0.0, floor(fmin/df));
    size_t i2f = min(1.0*win, ceil(fmax/df));
    size_t lenf = i2f-i1f;

    // fill array and do fft
    for (int i=0; i<win; i++){
      cbuf[i][0] = sc*data[start[is]+i];
      cbuf[i][1] = 0;
    }
    fftw_execute(plan);

    size_t x1 = (start[is]*w)/len;
    size_t x2 = (start[is+1]*w)/len;
    for (int y = 0; y<h; y++){
      // convert y -> f
      double f = fmin + ((fmax-fmin)*(h-1-y))/h;
      int fi = floor(f/df);
      if (fi<0) fi=0;
      if (fi>win-2) fi=win-2;
      double v1 = hypot(cbuf[fi][0], cbuf[fi][1]);
      double v2 = hypot(cbuf[fi+1][0], cbuf[fi+1][1]);
      double v = (v1 + (f/df-fi)*(v2-v1))/win;
      for (int x=x1; x<x2; x++) pic[y*w+x] = v;
    }
  fftw_destroy_plan(plan);
  fftw_free(cbuf);
  }



  // find data range
  double cmax=0;
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
      if (pic[y*w+x] > cmax) cmax = pic[y*w+x];
    }
  }

  // print data
  cout << "P6\n" << w << " " << h << "\n255\n";
  for (int y=0; y<h; y++){
    for (int x=0; x<w; x++){
       unsigned char c = 0xFF*(pic[y*w+x]/cmax);
      cout.write((char *)&c, 1);
      cout.write((char *)&c, 1);
      cout.write((char *)&c, 1);
    }
  }

}

/******************************************************************/
void
Data::fit_fork(double fmin, double fmax, double tmin, double tmax) const{

  // adjust tmin/tmax/fmin/fmax
  if (tmax<tmin) swap(tmax, tmin);
  if (tmax > t0+data.size()*dt) tmax = t0+data.size()*dt;
  if (tmin < t0) tmin = t0;
  if (fmax<fmin) swap(fmax, fmin);
  if (fmax > 1/dt) fmax = 1/dt;
  if (fmin < 0)    fmin = 0;

  // select time indices
  size_t i1 = max(0.0, floor((tmin-t0)/dt));
  size_t i2 = min(1.0*data.size(), ceil((tmax-t0)/dt));
  size_t len = i2-i1;

  // select frequency indices
  double df = 1/dt/len;
  size_t i1f = max(0.0, floor(fmin/df));
  size_t i2f = min(1.0*len, ceil(fmax/df));
  size_t lenf = i2f-i1f;

  fftw_complex  *cbuf;
  fftw_plan     plan;
  cbuf = fftw_alloc_complex(len);
  plan = fftw_plan_dft_1d(len, cbuf, cbuf, FFTW_FORWARD, FFTW_ESTIMATE);

  // fill array and do fft
  for (int i=0; i<len; i++){
    cbuf[i][0] = sc*data[i];
    cbuf[i][1] = 0;
  }
  fftw_execute(plan);

  // find maximum
  if (i2f-i1f<1) throw Err() << "Too small frequency range";
  double vm = hypot(cbuf[i1f][0],cbuf[i1f][1]);
  int im = i1f;
  for (int i = i1f; i<i2f; i++){
    double v = hypot(cbuf[i][0],cbuf[i][1]);
    if (v>vm) {vm=v; im=i;}
  }

  // 3-point fit
  if (im<i1+1 || im>=i2-1) throw Err() << "Maximum on the edge of the frequency range";
  double x1 = df*(im-1);
  double x2 = df*im;
  double x3 = df*(im+1);
  double y1 = hypot(cbuf[im-1][0], cbuf[im-1][1]);
  double y2 = hypot(cbuf[im][0],   cbuf[im][1]);
  double y3 = hypot(cbuf[im+1][0], cbuf[im+1][1]);
  double A = ((y1-y2)/(x1-x2) - (y2-y3)/(x2-x3))/(x1-x3);
  double B = (y1-y2)/(x1-x2) - A*(x1+x2);
  double C = y1 - A*x1*x1 - B*x1;
  double Max = -B*B/(4*A) + C;

  // fre - parabola maximum
  // tau - one over distance between zero crossings
  double fre = -B/(2*A);
  double tau = -2*A/sqrt(B*B-4*A*C)/M_PI;

  // secons pass
  double dff = M_PI/tau;

  // update frequency indices
  i1f = max(0.0, floor((fre-dff)/df));
  i2f = min(1.0*len, ceil((fre+dff)/df));
  lenf = i2f-i1f;

  double sx2=0, sx1=0, sx0=0;
  complex<double> sxy(0,0), sy(0,0);
  for (int i = i1f; i<i2f; i++){
    double x = df*i-fre;
    double Re = cbuf[i][0];
    double Im = cbuf[i][1];
    complex<double> y = Max/complex<double>(Re,Im);
    double w = pow(hypot(Re,Im), 4.0);
    sx2 += x*x*w;
    sx1 += x*w;
    sx0 += w;
    sxy += x*y*w;
    sy  += y*w;
  }
  complex<double> BB = (sxy*sx1 - sy*sx2)/(sx1*sx1 - sx0*sx2);
  complex<double> AA = (sxy - BB*sx1)/sx2;

  // this complex amplitudes are fit the original complex fft signal
  double AmpR =  2*M_PI*(1.0/AA).imag()*Max;
  double AmpI =  2*M_PI*(1.0/AA).real()*Max;
  // this amplitude corresponds to the original signal (volts an t=0)
  double Amp =  hypot(AmpR, AmpI)*2*(tmax-tmin)/len;
  fre = fre - (BB/AA).real();
  tau = -1/(2*M_PI*(BB/AA).imag());
  cout << setprecision(6)  << Amp << " "
       << setprecision(12) << fre << " "
       << setprecision(6)  << tau << "\n";

  fftw_destroy_plan(plan);
  fftw_free(cbuf);
}
