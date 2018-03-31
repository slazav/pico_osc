#include <string>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <cstdlib> // atof
#include <cstring>
#include <stdint.h>

#include <cstdlib> // atof
#include <cmath>
#include <iomanip>
#include <complex>
#include <fftw3.h>

#include "../pico_rec/err.h"
#include "signal.h"
#include "fft.h"
#include "fit_signal.h"
#include "dimage.h"

#define FTYPE double

using namespace std;
/***********************************************************/
// Select time range
void
Signal::crop_t(double tmin, double tmax){
  int n = get_n();
  // adjust tmin/tmax
  if (tmax<tmin) std::swap(tmax, tmin);
  if (tmax > t0 + n*dt) tmax = t0 + n*dt;
  if (tmin < t0) tmin = t0;
  // select time indices
  size_t i1t = std::max(0.0,  floor((tmin-t0)/dt));
  size_t i2t = std::min(1.0*n, ceil((tmax-t0)/dt));
  if (i2t<=i1t) throw Err() << "too small time range: " << tmin << " - " << tmax;
  for (int c = 0; c<get_ch(); c++){
    chan[c].assign(chan[c].begin()+i1t, chan[c].begin()+i2t);
  }
  t0 = tmin;
}

// Select channels
void
Signal::crop_c(const std::vector<int> & channels){
  std::vector<Channel> old_chan;
  std::swap(chan, old_chan);
  for (int i =0; i<channels.size(); i++){
    if (channels[i]<0 || channels[i]>=old_chan.size())
      throw Err() << "No such channel: " << channels[i];
    chan.push_back(old_chan[channels[i]]);
  }
}

/***********************************************************/
Signal read_signal(istream & ff){
  const int hsize=4;
  char head[hsize];
  if (ff.fail()) throw Err() << "Can't read file";
  ff.read(head, hsize);
  ff.seekg(0, ios::beg);

  if (strncmp(head, "*SIG", hsize)==0) return read_sig(ff);
  if (strncmp(head, "RIFF", hsize)==0) return read_wav(ff);
  throw Err() << "unknown format (not SIG or WAV)";
}

/***********************************************************/
Signal read_sig(istream & ff){

  if (ff.fail()) throw Err() << "Can't read file";
  Signal sig;

  // first line: *SIG001
  string line;
  getline(ff,line);
  if (line != "*SIG001" && line != "*SIGF01") throw Err() << "unsupported format";
  bool is_fft = (line == "*SIGF01");

  int i1f=-1, i2f=-1;
  size_t N=0;

  // read metadata <name>: <value>
  while (!ff.eof()){
    // read line
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
    // number of points
    if (key == "points"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      N = atof(valw[0].c_str());
    }
    // time step
    if (key == "dt"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      sig.dt = atof(valw[0].c_str());
    }
    // time shift
    if (key == "t0"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      sig.t0 = atof(valw[0].c_str());
    }
    // absolute time
    if (key == "t0abs"){
      if (valw.size()<1) throw Err() << "Broken file: " << line;
      sig.t0abs = atoi(valw[0].c_str());
    }

    // channel parameters: name sc overflow
    if (key == "chan"){
      if (valw.size()<3) throw Err() << "Broken file: " << line;
      Signal::Channel ch;
      ch.name = valw[0][0];
      ch.sc   = atof(valw[1].c_str());
      ch.ov   = atoi(valw[2].c_str());
      sig.chan.push_back(ch);
    }

    // fft-specific data
    if (is_fft){
      if (key == "ifmin"){
        if (valw.size()<1) throw Err() << "Broken file: " << line;
        i1f = atoi(valw[0].c_str());
      }
      if (key == "ifmax"){
        if (valw.size()<1) throw Err() << "Broken file: " << line;
        i2f = atoi(valw[0].c_str());
      }
    }
  }
  // number of channels
  int num = sig.chan.size();


  // find data length
  ios::pos_type start_pos = ff.tellg();
  ff.seekg (0, ios::end);
  int length = ff.tellg() - start_pos;
  ff.seekg(start_pos, ios::beg);

  if (!is_fft) {
    if (length != N*num*sizeof(int16_t))
      throw Err() << "wrong data length: " << length;

    // prepare arrays
    for (int n=0; n< num; n++){ sig.chan[n].resize(N); }

    // read data array
    int bufsize = 1<<16;
    int cnt = 0;
    if (num<1) return sig;

    vector<int16_t> buf(bufsize*num);
    while (!ff.eof()){
      ff.read((char *)buf.data(), bufsize*num*sizeof(int16_t));
      int len = ff.gcount()/num/sizeof(int16_t);
      for (int n=0; n< num; n++){
        // This should not happend. If file is longer than we expected
        if (cnt+len > sig.chan[n].size()) sig.chan[n].resize(cnt+len);
        // fill the vector with data
        for (int i=0; i<len; i++) sig.chan[n][cnt+i] = buf[i*num+n];
      }
      cnt+=len;
    }
  }
  else { // fft

    if (i1f<0 || i2f<0 || i1f>N/2 || i2f>N/2 || i1f>=i2f)
      throw Err() << "wrong frequncy indices: " << i1f << ", " << i2f;

    if (length != 2*sizeof(FTYPE)*num*(i2f-i1f))
      throw Err() << "wrong data length: " << length << " (should be " << 2*sizeof(FTYPE)*num*(i2f-i1f)<< ")";

    FFT fft(N, FFTW_BACKWARD);
    for (int n=0; n<num; n++){
      for (int i = 0; i<(N+1)/2; i++){
        if (i>=i1f && i<i2f){
          FTYPE data[2];
          ff.read((char *)data, 2*sizeof(FTYPE));
          fft.set(i,   data[0],  data[1]);
          fft.set(N-i, data[0], -data[1]);
        }
        else {
          fft.set(i, 0.0, 0.0);
          fft.set(N-i, 0.0, 0.0);
        }
      }
      fft.run();
      sig.chan[n].vector<int16_t>::operator=(fft.real(0,N, sig.chan[n].sc*N));
    }
  }
  return sig;
}

/***********************************************************/
void write_sig(ostream & ff, const Signal & sig){
  // number of points
  int N = sig.get_n();

  ff << scientific;
  ff << "*SIG001\n";
  ff << "\n# Signal parameters:\n";
  ff << "  points:   " << N  << "  # number of points\n"
     << "  dt:       " << sig.dt << "  # time step\n"
     << "  t0:       " << sig.t0 << "  # relative time of the first sample\n"
     << "  t0abs:    " << sig.t0abs << "  # system time of trigger position\n";

  ff << "\n# Data channels (osc channel, scale factor, overload):\n";
  for (int j=0; j<sig.chan.size(); j++){
    char   ch = sig.chan[j].name;
    double sc = sig.chan[j].sc;
    bool   ov = sig.chan[j].ov;
    ff << "  chan: " << " " << ch << " " << sc << " " << ov << "\n";
  }

  ff << "\n*\n";
  for (int i = 0; i<N; i++){
    for (int n=0; n<sig.chan.size(); n++){
      ff.write((const char*)&sig.chan[n][i], sizeof(int16_t));
    }
  }
}

/***********************************************************/
void write_sigf(ostream & ff, const Signal & sig, double fmin, double fmax){
  // write fft data to the file

  // number of points
  int N = sig.get_n();
  int cN = sig.chan.size();

  // same header, but SIGF instead of SIG
  ff << scientific;
  ff << "*SIGF01\n";
  ff << "\n# Signal parameters:\n";
  ff << "  points:   " << N  << "  # number of points\n"
     << "  dt:       " << sig.dt << "  # time step\n"
     << "  t0:       " << sig.t0 << "  # relative time of the first sample\n"
     << "  t0abs:    " << sig.t0abs << "  # system time of trigger position\n";

  ff << "\n# Data channels (osc channel, scale factor, overload):\n";
  for (int j=0; j<cN; j++){
    char   ch = sig.chan[j].name;
    double sc = sig.chan[j].sc;
    bool   ov = sig.chan[j].ov;
    ff << "  chan: " << " " << ch << " " << sc << " " << ov << "\n";
  }

  // prepare FFT, write additional frequency information
  FFT fft(N);
  int i1f, i2f;
  double df;
  fft.get_ind(sig.dt, &fmin, &fmax, &i1f, &i2f, &df);
  ff << "\n# FFT parameters:\n";
  ff << "  fmin:   " << fmin  << "  # lowest frequncy\n";
  ff << "  fmax:   " << fmax  << "  # highest frequncy\n";
  ff << "  df:     " << df    << "  # frequncy step\n";
  ff << "  ifmin:  " << i1f   << "  # lowest frequncy index\n";
  ff << "  ifmax:  " << i2f   << "  # highest frequncy index\n";

  ff << "\n*\n";
  if (N<1 || cN<1) return;

  // do fft and write data
  for (int c = 0; c<cN; c++){
    fft.run(sig.chan[c].data(), sig.chan[c].sc);
    for (int i=i1f; i<i2f; i++){
      FTYPE re = fft.real(i);
      FTYPE im = fft.imag(i);
      ff.write((char*)&re, sizeof(FTYPE));
      ff.write((char*)&im, sizeof(FTYPE));
    }
  }
}

/***********************************************************/
// WAV files
// See: http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
/***********************************************************/

// fmt chunk structure
struct fmt_t {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

// sig1 chunk structure
//struct sig1_t {
//  double dt;
//  double t0;
//  time_t t0abs;
//  int16_t n; // number of channel blocks
//};
//struct sig1c_t {
//  char name;
//  char        name; // channel name
//  bool        ov;   // overload flag
//  double      sc;   // scale factor
//}

/***********************************************************/
Signal read_wav(istream & ff){

  if (ff.fail()) throw Err() << "Can't read file";
  Signal sig;

  int id_size=4;
  char id[id_size+1]; id[4]=0;
  uint32_t ch_len;

  ff.read(id, id_size);
  if (strncmp(id, "RIFF", id_size)!=0)
    throw Err() << "Not a WAV file, RIFF chunk is missing";

  ff.read((char *)&ch_len, sizeof(ch_len));

  ff.read(id, id_size);
  if (strncmp(id, "WAVE", id_size)!=0)
    throw Err() << "Not a WAV file, WAVE id is missing";

  fmt_t fmt;
  bool fmt_read=false;

  // read skip unknown chunks, read format chunk
  while (ff.good()) {

    // read chunk id and length
    ff.read(id, id_size);
    ff.read((char *)&ch_len, sizeof(ch_len));
    cerr << "chunk: " << id << ", length: " << ch_len << "\n";

    // fmt chunk
    if (strncmp(id, "fmt ", id_size)==0) {

      // check if the fmt chunk containt enough information
      if (ch_len < sizeof(fmt_t))
        throw Err() << "Bad WAV file, FMT chunk is too short";

      // read fmt structure
      ff.read((char *)&fmt, sizeof(fmt_t));
      if (fmt.wBitsPerSample!=16) throw Err() << "Only 16bits per sample supported";

      // skip rest of the chunk
      ff.seekg(ch_len-sizeof(fmt_t), ios::cur);
      fmt_read=true;

      continue;
    }

    // data chunk
    if (strncmp(id, "data", id_size)==0) {
      if (!fmt_read) throw "FMT chunk not found";

      // prepare arrays
      size_t nchan = fmt.nChannels;         // number of channels
      size_t ssize = fmt.wBitsPerSample/8;  // sample size, bytes
      size_t bsize = nchan*ssize;           // block size, bytes
      size_t count = ch_len/bsize;          // number of blocks
      size_t spp   = fmt.nSamplesPerSec;    // samples per second
      for (int n=0; n< fmt.nChannels; n++){
        Signal::Channel ch;
        ch.name = 'A'+n;
        ch.sc   = 1.0;
        ch.ov   = 0;
        sig.chan.push_back(ch);
        sig.chan[n].resize(count);
      }

      // time step
      sig.dt = 1.0/spp;

      char buf[bsize];
      // read data
      for (int i=0; i<count; i++){
        ff.read(buf, sizeof(buf));
        for (int n=0; n<fmt.nChannels; n++){
          sig.chan[n][i] = *(int16_t *)(buf + n*ssize);
        }
      }
      break;
    }

    // skip unknown chunk and continue reading
    ff.seekg(ch_len, ios::cur);
  }

  return sig;
}

/***********************************************************/
void write_wav(ostream & ff, const Signal & sig){
  // write data to the file

  int32_t N = sig.get_n();       // number of blocks
  int16_t C = sig.chan.size();   // number of channels
  int16_t B = sizeof(int16_t);   // bytes per sample

  // format chunk
  fmt_t fmt;
  fmt.wFormatTag = 1; // PCM
  fmt.nChannels  = C;
  fmt.nSamplesPerSec  = int(1.0/sig.dt);
  fmt.nAvgBytesPerSec = fmt.nSamplesPerSec*C*B;
  fmt.nBlockAlign    = C*B;
  fmt.wBitsPerSample = 8*B;

  // data length (4(wave) + 8(fmt chunk header) + 16(fmt chunk) + 8(data chunk header) + data)
  int32_t len = 4 + 8 + sizeof(fmt) + 8 + N*C*B;

  ff << "RIFF";
  ff.write((char *)&len, sizeof(len));
  ff << "WAVE";

  ff << "fmt ";
  int32_t fmt_len = sizeof(fmt);
  ff.write((char *)&fmt_len, sizeof(fmt_len));
  ff.write((char *)&fmt, sizeof(fmt));

  // TODO: signal chunk
  // sig.dt // time step
  // sig.t0 // relative time of the first sample
  // sig.t0abs // system time of trigger position

  // data chunk
  ff << "data";
  int32_t dat_len = N*C*B;
  ff.write((char *)&dat_len, sizeof(dat_len));

  char buf[C*B];
  for (int i=0; i<N; i++){
    for (int n=0; n<C; n++){
      *(int16_t *)(buf + n*B) = sig.chan[n][i];
    }
    ff.write(buf, sizeof(buf));
  }
}


/******************************************************************/
void
flt_txt(ostream & ff, const Signal & s){
  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

  ff << scientific;
  for (int i=0; i<N; i++){
    double t = s.t0 + s.dt*i;
    ff << s.t0 + s.dt*i;
    for (int c = 0; c<cN; c++){
      ff << "\t" << s.get_val(c,i);
    }
    ff << "\n";
  }
}

/******************************************************************/

void
flt_pnm(ostream & ff, const Signal & s, int W, int H){
  int colors[] = {0x00880000, 0x00008800, 0x00000088,
                  0x00888800, 0x00008888, 0x00880088,
                  0x00000000};
  int cn = sizeof(colors)/sizeof(int); // number of colors

  // the picture
  vector<int> pic(W*H, (int)0xFFFFFFFF);
  int N = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

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
  ff << "P6\n" << W << " " << H << "\n255\n";
  for (int y=0; y<H; y++){
    for (int x=0; x<W; x++){
      ff.write((char *)(&pic[y*W+x])+1, 3);
    }
  }
}


/******************************************************************/

void
flt_fft_txt(ostream & ff, const Signal & s, double fmin, double fmax){

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

  ff << scientific;
  for (int i=i1f; i<i2f; i++){
    ff << df*i;
    for (int c = 0; c<cN; c++){
      ff << "\t" << dat_re[c][i-i1f] << "\t" << dat_im[c][i-i1f];
    }
    ff << "\n";
  }
}

/******************************************************************/

void
flt_fft_pow_avr(ostream & ff, const Signal & s, double fmin, double fmax, int npts){

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

  ff << scientific;
  vector<double> ss(cN, 0);
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    for (int c = 0; c<cN; c++) ss[c]+=pow(dat[c][i-i1f],2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin + fstep || i==i2f-1){
      ff << (i-0.5*(n-1))*df;
      for (int c = 0; c<cN; c++){
        ff << "\t" << k*ss[c]/n;
        ss[c]=0;
      }
      ff << "\n";
      n=0; fmin=i*df;
    }
  }

}

/******************************************************************/

void
flt_fft_pow_lavr(ostream & ff, const Signal & s, double fmin, double fmax, int npts){

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

  ff << scientific;
  vector<double> ss(cN, 0);
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    for (int c = 0; c<cN; c++) ss[c]+=pow(dat[c][i-i1f],2);
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin*fstep || i==i2f-1){
      ff << (i-0.5*(n-1))*df;
      for (int c = 0; c<cN; c++){
        ff << "\t" << k*ss[c]/n;
        ss[c]=0;
      }
      ff << "\n";
      n=0; fmin=i*df;
    }
  }
}

/******************************************************************/

void
flt_fft_pow_avr_corr(ostream & ff, const Signal & s, double fmin, double fmax, int npts){

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

  ff << scientific;
  double sre=0, sim=0;
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    sre+=dat0re[i-i1f];
    sim+=dat0im[i-i1f];
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin + fstep || i==i2f-1){
      ff << (i-0.5*(n-1))*df << "\t" << k*hypot(sre,sim)/n << "\n";
      sre=0; sim=0;
      n=0; fmin=i*df;
    }
  }

}

/******************************************************************/

void
flt_fft_pow_lavr_corr(ostream & ff, const Signal & s, double fmin, double fmax, int npts){

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

  ff << scientific;
  double sre=0, sim=0;
  int n = 0; // number of samples in the average
  for (int i=i1f; i<i2f; i++){
    sre+=dat0re[i-i1f];
    sim+=dat0im[i-i1f];
    n++;
    // print point and reset counters if needed
    if (i*df >= fmin*fstep || i==i2f-1){
      ff << (i-0.5*(n-1))*df << "\t" << k*hypot(sre,sim)/n << "\n";
      sre=0; sim=0;
      n=0; fmin=i*df;
    }
  }
}


/******************************************************************/
void
flt_sfft_txt(ostream & ff, const Signal & s, double fmin, double fmax, int win) {

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
    ff << scientific;
    for (int i=i1f; i<i2f; i++){
      ff << s.t0 + s.dt*(iw+win/2) << "\t" << i*df << "\t"
         << fft.real(i) << "\t" << fft.imag(i) << "\n";
    }
    ff << "\n";
  }
}

/******************************************************************/
void
flt_sfft_int(ostream & ff, const Signal & s, double fmin, double fmax, int win) {

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
    ff << scientific;
    double sum=0.0;
    int n=0;
    for (int i=i1f; i<i2f; i++){
      sum+=fft.abs(i)*fft.abs(i);
      n++;
    }
    ff << s.t0 + s.dt*(iw+win/2) << "\t" << sqrt(sum/n) << "\n";
  }
}


/******************************************************************/

void
flt_sfft_pnm(ostream & ff, const Signal & s, double fmin, double fmax, int win, int W, int H) {

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
      pic.set(x,y,log(v));
    }
  }
  pic.print_pnm(ff);
}

/******************************************************************/

void
flt_sfft_pnm_ad(ostream & ff, const Signal & s, double fmin, double fmax, int W, int H) {

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
  pic.print_pnm(ff);
}

/******************************************************************/

void
fit(ostream & ff, const Signal & s, double fmin, double fmax) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;
  int ch = 0;

  vector<double> ret = ::fit_signal(
    s.chan[ch].data(), N, s.chan[ch].sc, s.dt, s.t0, fmin, fmax);

  ff << s.t0abs << " "
     << setprecision(12) << ret[0] << " "
     << setprecision(6)  << ret[1] << " "
     << setprecision(6)  << ret[2] << " "
     << setprecision(6)  << ret[3] << "\n";

}

/******************************************************************/

void
fit2(ostream & ff, Signal & s, double fmin, double fmax) {

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

  ff << s.t0abs << " "
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
lockin(ostream & ff, const Signal & s, double fmin, double fmax) {

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
  ff << setprecision(12) << fre << " "
     << setprecision(6)  << 2*ss1/N << " "
     << setprecision(6)  << 2*ss2/N << "\n";

}

/******************************************************************/
void
minmax(ostream & ff, const Signal & s) {

  int N  = s.get_n();
  int cN  = s.get_ch();
  if (N<1 || cN<1) return;

  for (int c = 0; c<cN; c++){
    double min,max;
    min = max = s.chan[c][0]*s.chan[c].sc;
    for (int i=1; i<N; i++){
      double v = s.chan[c][i]*s.chan[c].sc;
      if (v<min) min = v;
      if (v>max) max = v;
    }
    ff << setprecision(6)  << min << " "
       << setprecision(6)  << max << "\n";
  }
}


/******************************************************************/
/*
void
crop(ostream & ff, const Signal & s, double fmin, double fmax) {

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
    ff << t0+dt*i << "\t"
       << s.chan[ch].sc*data[i] << "\t"
       << cbuf[i][0]/win << "\n";
  }

  fftw_destroy_plan(plan1);
  fftw_destroy_plan(plan2);
  fftw_free(cbuf);
}

*/
