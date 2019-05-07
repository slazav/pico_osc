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

#include "iofilter/iofilter.h"
#include "err/err.h"
#include "fft/fft.h"
#include "signal.h"

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
  if (tmin > t0 + n*dt) tmin = t0 + n*dt;
  if (tmax < t0) tmax = t0;
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

void
Signal::add(const Signal &other){
  if (chan.size()==0) {
    *this = other;
    return;
  }
  // use dt,t0 from the first signal, write warning if values are different:
  if (dt!=other.dt) std::cerr << "Appending signals with different time step: "
                              << dt << " and " << other.dt << "\n";

  if (t0!=other.t0) std::cerr << "Appending signals with different time shift: "
                              << t0 << " and " << other.t0 << "\n";

  int c1 = chan.size();
  int c2 = other.chan.size();

  chan.insert(chan.end(), other.chan.begin(), other.chan.end());

  // erase first points to align t0.
  if (other.t0>t0){ // crop old channels
    int skip = (other.t0-t0)/dt;
    for (int c = 0; c<c1; c++)
      chan[c].erase(chan[c].begin(), chan[c].begin() + skip);
    t0 = other.t0;
  }
  if (t0>other.t0){ // crop new channels
    int skip = (t0-other.t0)/other.dt;
    for (int c = c1; c<c1+c2; c++)
      chan[c].erase(chan[c].begin(), chan[c].begin() + skip);
  }

  // erase last points to align N.
  int Nmin=0;
  for (int c = 0; c<get_ch(); c++) {
    if (Nmin==0 || Nmin>chan[c].size()) Nmin = chan[c].size();
  }
  for (int c = 0; c<get_ch(); c++) {
    if (Nmin==chan[c].size()) continue;
    std::cerr << "Appending signals with different number of points, crop: "
              << chan[c].size() << " to " << Nmin << "\n";
    chan[c].resize(Nmin);
  }

}

/***********************************************************/
Signal read_signal(istream & ff){
  const int hsize=4;
  char head[hsize];
  if (ff.fail()) throw Err() << "Can't read file";

  // ff can be a non-seakable stream (see gzip format below).
  // let's try to read 4 characters and then push them back:
  ff.read(head, hsize);
  ff.putback(head[3]);
  ff.putback(head[2]);
  ff.putback(head[1]);
  ff.putback(head[0]);
  if (ff.fail()) throw Err() << "Can't read file: putback error";

  // sig and sigf formats
  if (strncmp(head, "*SIG", hsize)==0)
    return read_sig(ff);

  // wav format
  if (strncmp(head, "RIFF", hsize)==0)
    return read_wav(ff);

  // flac format
  if (strncmp(head, "fLaC", hsize)==0) {
    IFilter flt(ff, "flac -d -s -c -");
    return read_wav(flt.stream());
  }

  // gzip format
  if (head[0] == (char)0x1f && head[1] == (char)0x8b) {
    IFilter flt(ff, "gunzip");
    return read_signal(flt.stream());
  }
  throw Err() << "unknown format (not SIG, WAV, FLAC or GZ)";
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
      sig.t0abs_str = valw[0];
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
  if (ff.fail()) throw Err() << "Can't read file: seek error";
  int length = ff.tellg() - start_pos;
  ff.seekg(start_pos, ios::beg);
  if (ff.fail()) throw Err() << "Can't read file: seek error";

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
      for (int i = 0; i<=N/2; i++){
        if (i>=i1f && i<i2f){
          FTYPE data[2];
          ff.read((char *)data, 2*sizeof(FTYPE));
          fft.set(i,   data[0],  data[1]);
          if (i!=0) fft.set(N-i, data[0], -data[1]);
        }
        else {
          fft.set(i, 0.0, 0.0);
          if (i!=0) fft.set(N-i, 0.0, 0.0);
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
    //cerr << "chunk: " << id << ", length: " << ch_len << "\n";

    // fmt chunk
    if (strncmp(id, "fmt ", id_size)==0) {

      // check if the fmt chunk containt enough information
      if (ch_len < sizeof(fmt_t))
        throw Err() << "Bad WAV file, FMT chunk is too short";

      // read fmt structure
      ff.read((char *)&fmt, sizeof(fmt_t));
      if (fmt.wBitsPerSample!=16) throw Err() << "Only 16bits per sample supported";

      // skip rest of the chunk
      // note: ff.seekg does not work with streams from iofilter!
      for (int i=sizeof(fmt_t); i<ch_len; i++) ff.get();
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
    // (ff.seekg does not work with streams from iofilter)
    for (int i=0; i<ch_len; i++) ff.get();
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


