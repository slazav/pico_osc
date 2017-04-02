#include <string>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <cstdlib> // atof
#include "../pico_rec/err.h"
#include "signal.h"

using namespace std;

Signal read_signal(const char *fname){
  Signal sig;

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

void write_signal(const char *fname, const Signal & sig){
  // write data to the file
  ofstream ff(fname);

  // number of points (0 for empty signal, same for all channels)
  int N =0;
  for (int i=0; i<sig.chan.size(); i++){
    if (i==0) N = sig.chan[0].size();
    else if (N!=sig.chan[0].size())
      throw Err() << "Broken signal: data arrays have different sizes";
  }

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

