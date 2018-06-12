#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include <iomanip>
#include <cmath>
#include <time.h>
#include "pico_int.h"
#include "err.h"

#define VERSION "2.1"

using namespace std;

bool
is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0; }

const char *
PicoInt::cmd_help() const {
  return
  "help -- show command list\n"
  "ranges <ch> -- show possible ranges\n"
  "   ch  -- select channel: A,B\n"
  "chan_set <ch> <en> <cpl> <rng> -- set channel parameters\n"
  "   ch  -- select channel: A,B\n"
  "   en  -- enable channel: 1,0\n"
  "   cpl -- coupling: AC, DC\n"
  "   rng -- input range, volts (see ranges command)\n"
  "trig_set <src> <lvl> <dir> <del> -- set channel parameters\n"
  "   src -- source: A,B,EXT,NONE\n"
  "   lvl -- threshold level, ratio of full range, -1..1\n"
  "   dir -- direction: RISING,FALLING,ABOVE,BELOW,RISING_OR_FALLING\n"
  "   del -- delay, samples\n"
  "block <ch> <npre> <npost> <dt> <file> -- record signal (block mode)\n"
  "   ch    -- channels to record: A,B,AB,BA, etc."
  "   npre  -- number of pretrigger samples\n"
  "   npost -- number of posttrigger samples\n"
  "   dt    -- time step, seconds\n"
  "   file  -- output file\n"
  "wait  -- wait until osc is ready (use after block command)\n"
  "filter <file> <args> -- run sig_filter program"
  "*idn? -- write id string: \"pico_rec " VERSION "\"\n";
  ;
}

bool
PicoInt::cmd(const vector<string> & args){
  if (args.size()<1) return false;


  // print id
  if (is_cmd(args, "*idn?")){
    if (args.size()!=1) throw Err() << "Usage: *idn?";
    cout << "pico_rec " VERSION "\n";
    return true;
  }

  // print help
  if (is_cmd(args, "help")){
    if (args.size()!=1) throw Err() << "Usage: help";
    cout << cmd_help();
    return true;
  }

  // show range settings
  if (is_cmd(args, "ranges")){
    if (args.size()!=2) throw Err() << "Usage: ranges <ch>";
    cout << chan_get_ranges(args[1].c_str()) << "\n";
    return true;
  }

  // set channel parameters
  if (is_cmd(args, "chan_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: chan_set <ch> <en> <cpl> <rng>";
    for (int i=0; i<args[1].length(); i++){
      string ch; ch+=args[1][i]; // channel as a string
      char chc = args[1][i];  // channel as a single char
      ChConf C;
      C.en  = atoi(args[2].c_str());
      C.cpl = args[3];
      C.rng = atof(args[4].c_str());
      chan_set(ch.c_str(), C.en, C.cpl.c_str(), C.rng);
      chconf[chc] = C; // save channel configuration
    }
    return true;
  }

  // set trigger parameters
  if (is_cmd(args, "trig_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: trig_set <src> <lvl> <dir> <del>";
    TrConf T;
    T.src = args[1];
    T.lvl = atof(args[2].c_str());
    T.dir = args[3];
    T.del = atoi(args[4].c_str());
    trig_set(T.src.c_str(), get_max_val()*T.lvl, T.dir.c_str(), T.del, 0);
    // save trigger configuration
    trconf.clear();
    trconf.push_back(T);
    return true;
  }

  if (is_cmd(args, "block")) {
    if (args.size()!=6) throw Err()
      << "Usage: block <ch> <npre> <npost> <dt> <file>";
    string chans = args[1];
    uint32_t  npre  = atof(args[2].c_str());
    uint32_t  npost = atof(args[3].c_str());
    float     dt    = atof(args[4].c_str());
    const char *fname = args[5].c_str();
    uint32_t  N     = npre+npost;

    // check that trigger is configured
    if (trconf.size()!=1) throw Err() << "Trigger is not configured";
    TrConf T = trconf[0];

    // reset all channel buffers
    map<char,ChConf>::iterator ic;
    for (ic = chconf.begin(); ic!= chconf.end(); ic++) ic->second.buf.clear();

    // set up and register all needed channel buffers
    for (int i=0;i<chans.size();i++){
      char chc = chans[i];
      string ch = string() + chc;
      ic = chconf.find(chc);
      if (ic == chconf.end()) throw Err() << "Channel " << ch << " is not configured";
      if (ic->second.buf.size()>0) continue; // already configured
      ic->second.buf.resize(N);
      set_buf(ch.c_str(), ic->second.buf.data(), ic->second.buf.size());
    }

    ofstream ff(fname);
    if (ff.fail()) throw Err() << "Can't open output file: " << fname;

    // start collecting data
    run_block(npre, npost, &dt);
    usleep(npre*dt*1e6);
    cout << "#OK\n" << flush;
    usleep(npost*dt*1e6);
    while (!is_ready()) usleep(1000);
    struct timespec t0abs;
    if (clock_gettime(CLOCK_REALTIME, &t0abs)!=0)
      throw Err() << "Can't get system time";

    int16_t ov;
    get_block(0, &N, &ov);

    double t0 = get_trig() - npre*dt + T.del*dt; // time of the first sample from the trigger (usually negative)
    double tlen = dt*N; // signal length from trigger to the end in seconds
    long ds = round(tlen); // second correction
    long dns = round(1e9*(tlen-ds)); // nanosecond correction
    t0abs.tv_sec -= ds;
    t0abs.tv_nsec -= dns;
    while (t0abs.tv_nsec<0){ t0abs.tv_sec+=1; t0abs.tv_nsec+=1e9; }


    ff << "*SIG001\n";
    ff << "# " << ctime(&(t0abs.tv_sec)) << "\n";
    ff << "\n# Oscilloscope settings:\n";
    ff << scientific;
    for (ic = chconf.begin(); ic!= chconf.end(); ic++){
      char ch = ic->first;
      ChConf C = ic->second;
      ff << "  chan_set: " << ch << " " << C.en << " " << C.cpl << " " << C.rng << "\n";
    }
    ff << "  trig_set: " << T.src << " " << T.lvl << " " << T.dir << " " << T.del << "\n";
    ff << "  block:    " << args[1] << " " << args[2] << " " << args[3] << " "
                       << args[4] << " " << args[5] << "\n";

    ff << "\n# Signal parameters:\n";
    ff << "  points:   " << N  << "  # number of points\n"
       << "  dt:       " << dt << "  # time step\n"
       << "  t0:       " << t0 << "  # relative time of the first sample\n"
       << "  t0abs:    " << t0abs.tv_sec << "."
                         << setw(9) << setfill('0') << t0abs.tv_nsec
                         << "  # system time of trigger position\n";

    ff << "\n# Data channels (osc channel, scale factor, overload):\n";
    for (int j=0; j<chans.size(); j++){
      char ch = chans[j];
      ic = chconf.find(ch);
      double sc = ic->second.rng/get_max_val();
      bool o = false;
      if (ch=='a' || ch=='A') o = (bool)(ov&1);
      if (ch=='b' || ch=='B') o = (bool)(ov&2);
      if (ch=='c' || ch=='C') o = (bool)(ov&4);
      if (ch=='d' || ch=='D') o = (bool)(ov&8);
      ff << "  chan: " << " " << ch << " " << sc << " " << o << "\n";
    }

    ff << "\n*\n";
    for (int i = 0; i<N; i++){
      for (int j=0; j<chans.size(); j++){
        ic = chconf.find(chans[j]);
        if (ic == chconf.end() || ic->second.buf.size()<N)
          throw Err() << "Buffer error for channel " << chans[j];
        ff.write((const char*)(ic->second.buf.data()+i), sizeof(int16_t));
      }
    }
    return false;
  }

  // wait until osc is ready
  if (is_cmd(args, "wait")) {
    if (args.size()!=1) throw Err() << "Usage: wait";
    return true;
  }

  // run filter
  // note: stderr can not be transferred to Device
  if (is_cmd(args, "filter")) {
    if (args.size()<2) throw Err()
      << "Usage: filter <file> <args> ...";
    string cmd = "sig_filter " + args[1];
    for (int i=2; i<args.size(); i++) cmd += " " + args[i];
    system(cmd.c_str());
    return true;
  }

  throw Err() << "Unknown command: " << args[0];
}
