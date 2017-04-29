#include <string>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <cstdlib> // atof
#include "../pico_rec/err.h"
#include "signal.h"

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
Signal read_header(ifstream &ff){
  Signal sig;
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
      Channel ch;
      ch.name = valw[0][0];
      ch.sc   = atof(valw[1].c_str());
      ch.ov   = atoi(valw[2].c_str());
      sig.chan.push_back(ch);
    }
  }
  return sig;
}

/***********************************************************/
Signal read_signal(const char *fname){

  ifstream ff(fname);
  if (ff.fail()) throw Err() << "Can't read file: " << fname;
  Signal sig = read_header(ff);

  // read data array
  int bufsize = 1<<16;
  int cnt = 0;
  int num = sig.chan.size();
  if (num<1) return sig;

  vector<int16_t> buf(bufsize*num);
  while (!ff.eof()){
    ff.read((char *)buf.data(), bufsize*num*sizeof(int16_t));
    int len = ff.gcount()/num/sizeof(int16_t);
    for (int n=0; n< num; n++){
      sig.chan[n].resize(cnt+len);
      for (int i=0; i<len; i++){
        sig.chan[n][cnt+i] = buf[i*num+n];
      }
    }
    cnt+=len;
  }
  return sig;
}

/***********************************************************/
void write_signal(const char *fname, const Signal & sig){
  // write data to the file
  ofstream ff(fname);

  // number of points
  int N = sig.get_n();

  ff << scientific;
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

