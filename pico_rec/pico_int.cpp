#include <cstring> // strcasecmp
#include <cstdlib> // atoi
#include <unistd.h> // usleep
#include <fstream>
#include <iomanip>
#include <cmath>
#include <time.h>
#include <sys/time.h> // gettimeofday
#include <chrono>
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
  "   ch  -- select channel: A,B,AB etc.\n"
  "   en  -- enable channel: 1,0\n"
  "   cpl -- coupling: AC, DC\n"
  "   rng -- input range, volts (see ranges command)\n"
  "chan_get <ch>  -- get channel parameters\n"
  "   ch  -- select channel: A,B,AB etc.\n"
  "   Returns a line with four words: <ch> <en> <cpl> <rng> for each channel.\n"
  "   If the channel is not set returns channel name with 'undef' word.\n"
  "trig_set <src> <lvl> <dir> <del> -- set channel parameters\n"
  "   src -- source: A,B,EXT,NONE\n"
  "   lvl -- threshold level, ratio of full range, -1..1\n"
  "   dir -- direction: RISING,FALLING,ABOVE,BELOW,RISING_OR_FALLING\n"
  "   del -- delay, samples\n"
  "trig_get  -- get trigger parameters\n"
  "   Returns a line with four words: <src> <lvl> <dir> <del>.\n"
  "   If the trigger is not set returns 'undef' word.\n"
  "block <ch> <npre> <npost> <dt> <file> -- record signal (block mode)\n"
  "   ch    -- channels to record: A,B,AB,BA, etc.\n"
  "   npre  -- number of pretrigger samples\n"
  "   npost -- number of posttrigger samples\n"
  "   dt    -- time step, seconds\n"
  "   file  -- output file (do not write anything if file is -)\n"
  "   The block command returns #OK when recording is set up and\n"
  "   trigger can be fired (if needed). When wait command should be used\n"
  "   to wait until the recording will be completed and get its status.\n"
  "wait  -- wait until osc is ready and return status of last block command.\n"
  "   Should be used after the block command\n"
  "filter <file> <args> -- run sig_filter program\n"
  "*idn? -- write id string: \"pico_rec " VERSION "\"\n"
  "get_time -- print current time (unix seconds with ms precision)\n";
  ;
}

//save signal to a file
void PicoInt::save_signal(const std::string &fname) {
  if (fname=="" || fname=="-") return;

  // check that trigger is configured
  if (trconf.size()!=1) throw Err() << "Trigger is not configured";
  TrConf T = trconf[0];

  // check that block is configured
  if (blconf.size()!=1) throw Err() << "Block reader is not configured";
  BlConf B = blconf[0];

  ofstream ff(fname);
  if (ff.fail()) throw Err() << "Can't open output file: " << B.fname;

  ff << "*SIG001\n";
  ff << "# " << ctime(&(B.sec)) << "\n";
  ff << "\n# Oscilloscope settings:\n";
  ff << scientific;
  map<char,ChConf>::iterator ic;
  for (ic = chconf.begin(); ic!= chconf.end(); ic++){
    char ch = ic->first;
    ChConf C = ic->second;
    ff << "  chan_set: " << ch << " " << C.en << " " << C.cpl << " " << C.rng << "\n";
  }
  ff << "  trig_set: " << T.src << " " << T.lvl << " " << T.dir << " " << T.del << "\n";
  ff << "  block:    " << B.chans << " " << B.npre << " " << B.npost << " "
                       << B.dtset << " " << B.fname << "\n";

  ff << "\n# Signal parameters:\n";
  ff << "  points:   " << B.N  << "  # number of points\n"
     << "  dt:       " << B.dt << "  # time step\n"
     << "  t0:       " << B.t0 << "  # relative time of the first sample\n"
     << "  t0abs:    " << B.sec << "."
                       << setw(9) << setfill('0') << B.nsec
                       << "  # system time of trigger position\n";
  if (navr>=0) ff << "  navr:     " << navr << "  # numbel of averaged signals";

  ff << "\n# Data channels (osc channel, scale factor, overload):\n";
  for (int j=0; j<B.chans.size(); j++){
    char ch = B.chans[j];
    ic = chconf.find(ch);
    double sc = ic->second.rng/get_max_val();
    bool o = false;
    if (ch=='a' || ch=='A') o = (bool)(B.ov&1);
    if (ch=='b' || ch=='B') o = (bool)(B.ov&2);
    if (ch=='c' || ch=='C') o = (bool)(B.ov&4);
    if (ch=='d' || ch=='D') o = (bool)(B.ov&8);
    ff << "  chan: " << " " << ch << " " << sc << " " << o << "\n";
  }

  ff << "\n*\n";
  for (int i = 0; i<B.N; i++){
    for (int j=0; j<B.chans.size(); j++){
      ic = chconf.find(B.chans[j]);
      if (ic == chconf.end() || ic->second.buf.size()<B.N)
        throw Err() << "Buffer error for channel " << B.chans[j];
      ff.write((const char*)(ic->second.buf.data()+i), sizeof(int16_t));
    }
  }
}


bool
PicoInt::cmd(const vector<string> & args){
  if (args.size()<1) return false;

  // print time
  if (is_cmd(args, "get_time")){
    if (args.size()!=1) throw Err() << "Usage: get_time";
    struct timeval tv;
    gettimeofday(&tv, NULL);
    cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec << "\n";
    return true;
  }

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

  // get channel parameters
  if (is_cmd(args, "chan_get")) {
    if (args.size()!=2) throw Err()
      << "Usage: chan_get <ch>";
    for (int i=0; i<args[1].length(); i++){
      string ch; ch+=args[1][i]; // channel as a string
      char chc = args[1][i];  // channel as a single char
      if (chconf.find(chc)==chconf.end()){
        cout << chc << " undef\n";
        continue;
      }
      cout << chc << " "
           << chconf[chc].en << " "
           << chconf[chc].cpl << " "
           << chconf[chc].rng << "\n";
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

  // get trigger parameters
  if (is_cmd(args, "trig_get")) {
    if (args.size()!=1) throw Err()
      << "Usage: trig_get";
    if (trconf.size()<1){
      cout << "undef\n";
      return true;
    }
    cout << trconf[0].src << " "
         << trconf[0].lvl << " "
         << trconf[0].dir << " "
         << trconf[0].del << "\n";
    return true;
  }

  // wait command (should be run after block command)
  if (is_cmd(args, "wait")){
    if (args.size()!=1) throw Err() << "Usage: wait";
    if (block_err != "") throw Err() << block_err;
    return true;
  }

  if (is_cmd(args, "block")) {
    if (args.size()!=6) throw Err()
      << "Usage: block <ch> <npre> <npost> <dt> <file>";
    BlConf B;
    B.chans = args[1];

    // use atof to parse values like 1e6, then convert to int.
    B.npre  = atof(args[2].c_str());
    B.npost = atof(args[3].c_str());
    B.dtset = atof(args[4].c_str());
    B.fname = args[5];
    B.N     = B.npre + B.npost;
    B.dt    = B.dtset;

    // check that trigger is configured
    if (trconf.size()!=1) throw Err() << "Trigger is not configured";
    TrConf T = trconf[0];

    // reset all channel buffers
    map<char,ChConf>::iterator ic;
    for (ic = chconf.begin(); ic!= chconf.end(); ic++) ic->second.buf.clear();

    // averaging start/stop: clear buffers
    if (navr <= 0) avrbuf.clear();

    // set up and register all needed channel buffers
    for (int i=0;i<B.chans.size();i++){
      char chc = B.chans[i];
      string ch = string() + chc;
      ic = chconf.find(chc);
      if (ic == chconf.end()) throw Err() << "Channel " << ch << " is not configured";
      if (ic->second.buf.size()>0) continue; // already configured
      ic->second.buf.resize(B.N);
      set_buf(ch.c_str(), ic->second.buf.data(), ic->second.buf.size());
      // averaging start: make buffer for each channel, fill with zeros
      if (navr == 0) avrbuf[chc].buf = std::vector<int32_t>(B.N, 0);
    }


    // start collecting data
    run_block(B.npre, B.npost, &(B.dt));
    usleep(B.npre*B.dt*1e6);
    cout << "#OK\n" << flush; waiting = true; block_err.clear();
    usleep(B.npost*B.dt*1e6);
    while (!is_ready()) usleep(1000);
    struct timespec t0abs;
    if (clock_gettime(CLOCK_REALTIME, &t0abs)!=0)
      throw Err() << "Can't get system time";

    get_block(0, &(B.N), &(B.ov));

    B.t0 = get_trig() - B.npre*B.dt + T.del*B.dt; // time of the first sample from the trigger (usually negative)
    double tlen = B.dt*B.npost; // signal length from trigger to the end in seconds
    long ds = floor(tlen); // second correction
    long dns = floor(1e9*(tlen-ds)); // nanosecond correction
    t0abs.tv_sec -= ds;
    t0abs.tv_nsec -= dns;
    while (t0abs.tv_nsec<0){ t0abs.tv_sec-=1; t0abs.tv_nsec+=1e9; }
    B.sec = t0abs.tv_sec;
    B.nsec = t0abs.tv_nsec;

    // averaging: add recorded data to averaging buffers
    if (navr >= 0) {
      for (int c=0;c<B.chans.size();c++){
        char ch = B.chans[c];
        map<char,ChConf>::iterator ic = chconf.find(ch);
        if (ic == chconf.end())
          throw Err() << "No buffer for channel: " << ch;
        map<char,AvrBuf>::iterator ia = avrbuf.find(ch);
        if (ia == avrbuf.end())
          throw Err() << "No averaging buffer for channel: " << ch;
        if (ic->second.buf.size() != ia->second.buf.size())
          throw Err() << "Number of points changed during averaging";
        for (int i = 0; i<ic->second.buf.size(); i++)
          ia->second.buf[i] += ic->second.buf[i];
      }
      navr++;
      if (navr > (1<<16)) throw Err() << "Too many averages (buffer overflow is possible)";
    }

    blconf.clear();
    blconf.push_back(B);
    if (B.fname != "-") save_signal(B.fname);
    waiting = false;
    return false;
  }

  // start averaging
  if (is_cmd(args, "avr_start")) {
    if (args.size()!=1) throw Err() << "Usage: avr_start";
    navr=0;
    return true;
  }

  // stop averaging
  if (is_cmd(args, "avr_stop")) {
    if (args.size()!=1) throw Err() << "Usage: avr_stop";
    navr=-1;
    return true;
  }

  // save average signal
  if (is_cmd(args, "avr_save")) {
    if (args.size()!=2) throw Err() << "Usage: avr_save <file>";
    string fname = args[1];
    if (navr<1) throw Err() << "No averaging have been done yet";

    // do averaging and save the result in channel buffers
    map<char,AvrBuf>::iterator ia;
    for (ia = avrbuf.begin(); ia!=avrbuf.end(); ia++){
      map<char,ChConf>::iterator ic = chconf.find(ia->first);
      if (ic == chconf.end())
        throw Err() << "No buffer for channel: " << ia->first;
      int N = ia->second.buf.size();
      ic->second.buf.resize(N);
      for (int i = 0; i<N; i++) ic->second.buf[i] = ia->second.buf[i]/navr;
    }
    // save the signal
    save_signal(fname);
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
