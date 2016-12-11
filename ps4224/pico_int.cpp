#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include "pico_int.h"
#include "err.h"

using namespace std;

bool
is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0; }

const char *
PicoInt::cmd_help() const{
  return
  "===\n"
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
  "===\n"
  ;
}

void
PicoInt::cmd(const vector<string> & args){
  if (args.size()<1) return;

  // print help
  if (is_cmd(args, "help")){
    cout << cmd_help();
    return;
  }

  // show range settings
  if (is_cmd(args, "ranges")){
    if (args.size()!=2) throw Err() << "Usage: ranges <ch>";
    cout << chan_get_ranges(args[1].c_str()) << "\n";
    return;
  }

  // set channel parameters
  if (is_cmd(args, "chan_set")) {
    if (args.size()!=5) throw Err()
      << "Usage: chan_set <ch> <en> <cpl> <rng>";
    if (args[1].size()!=1) throw Err() << "Bad channel: " << args[1];
    const char *ch = args[1].c_str();
    char chc = args[1][0]; // channel as a single char
    ChConf C;
    C.en  = atoi(args[2].c_str());
    C.cpl = args[3];
    C.rng = atof(args[4].c_str());
    chan_set(ch, C.en, C.cpl.c_str(), C.rng);
    chconf[chc] = C; // save channel configuration
    cout << "OK\n" << flush;
    return;
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
    cout << "OK\n" << flush;
    return;
  }

  if (is_cmd(args, "block")) {
    if (args.size()!=6) throw Err()
      << "Usage: block <ch> <npre> <npost> <dt> <file>";
    string chans = args[1];
    uint32_t  npre  = atoi(args[2].c_str());
    uint32_t  npost = atoi(args[3].c_str());
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

    // start collecting data
    run_block(npre, npost, &dt);
    usleep(npre*dt*1e6);
    cout << "Ready\n" << flush;
    usleep(npost*dt*1e6);
    while (!is_ready()) usleep(1000);
    time_t t0abs = time(NULL); // system time of last record

    int16_t ov;
    get_block(0, &N, &ov);

    double t0 = get_trig() - npre*dt + T.del*dt;
    t0abs += (time_t)(-N*dt+t0); // system time of trigger position

    // write data to the file
    ofstream ff(fname);

    // channel and trigger settings
    ff << "# " << ctime(&t0abs) << "\n";
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
       << "  t0abs:    " << t0abs << "  # system time of trigger position\n";

    ff << "\n# Data channels (number, osc channel, scale factor, overload):\n";
    ff << "  data_num: " << chans.size() << "\n";
    for (int j=0; j<chans.size(); j++){
      char ch = chans[j];
      ic = chconf.find(ch);
      double sc = ic->second.rng/get_max_val();
      bool o = false;
      if (ch=='a' || ch=='A') o = (bool)(ov&1);
      if (ch=='b' || ch=='B') o = (bool)(ov&2);
      if (ch=='c' || ch=='C') o = (bool)(ov&4);
      if (ch=='d' || ch=='D') o = (bool)(ov&8);
      ff << "  data: " << j << " " << ch << " " << sc << " " << o << "\n";
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
    cout << "Done\n" << flush;
    return;
  }
  throw Err() << "Unknown command: " << args[0];
}
