#ifndef PICO_CMD_H
#define PICO_CMD_H

// high-level commands for work with pico-devices

#include <string>
#include <sstream>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cmath>
#include <cstdlib>
#include "err.h"
#include "buf.h"
#include "pico_int.h"

// input parameters for high-level record functions
struct InPars{
  bool  use_a, use_b;
  float rng_a, rng_b;       // V
  std::string cpl_a, cpl_b; // "DC", "AC"
  std::string trig_src;         // "", "A", "B"
  float trig_del;               // s
  float trig_lvl;             // V
  std::string trig_dir;         // "RISING", "FALLING"
  float dt;      // time step, s
  uint32_t nrec; // total number of samples
  uint32_t npre; // pretrigger samples

  // defaults
  InPars(){
    use_a = use_b = true;
    rng_a = rng_b = 1;
    cpl_a = cpl_b = "DC";
    trig_src = ""; // none
    trig_lvl = 0;
    trig_dir = "RISING";
    dt    = 1e-3;
    nrec  = 1024;
    npre  = 0;
  }

  void print(std::ostream & ss){
    ss << std::scientific
       << "# Oscilloscope settings:\n"
       << "use_a: " << pi.use_a << "\n"
       << "rng_a: " << pi.rng_a << "\n"
       << "cpl_a: " << pi.cpl_a << "\n"
       << "use_b: " << pi.use_b << "\n"
       << "rng_b: " << pi.rng_b << "\n"
       << "cpl_b: " << pi.cpl_b << "\n"
       << "trig_src: " << pi.trig_src << "\n"
       << "trig_lvl: " << pi.trig_lvl << "\n"
       << "trig_del: " << pi.trig_del << "\n"
       << "trig_dir: " << pi.trig_dir << "\n"
       << "in_dt: " << pi.dt << "\n"
       << "nrec: "  << pi.nrec << "\n"
       << "npre: "  << pi.npre << "\n"
  }

};

/********************************************************************/
// parameter structure for the callback
struct CBPars{
  int nch; // number of active channels
  Buf<int16_t> bufs[MAXCH]; // array of buffers (only nch are used)
  double sc[MAXCH];         // array of channel scales (only nch are used)
  bool   ov[MAXCH];         // array of overflow flags (only nch are used)
  double dt,t0;             // time scale and relative time of the first point
  double tabs;              // absolute time of t=0
  int cnt, trg;             // sample count, trigger sample
  InPars pi;                // osc settings
};

/********************************************************************/
// Set osc parameters (trigger, channels)
// and prepare CBPars structure
CBPars set_osc(PicoInt & osc, const InPars & pi, uint32_t buflen){

  CBPars ret;
  ret.nch = 0;
  ret.t0 = ret.tabs = 0;
  ret.cnt = ret.trg = 0;
  ret.pi = pi;

  // set chan A
  if (pi.use_a){
    osc.chan_set("A", pi.cpl_a.c_str(), pi.rng_a);
    ret.bufs[ret.nch] = Buf<int16_t>(buflen);
    ret.sc[ret.nch]   = pi.rng_a/osc.get_max_val();
    ret.ov[ret.nch]   = false;
    osc.set_buf("A", bufa.data, bufa.size);
    ret.nch++;
  }
  else osc.chan_disable("A");

  // set chan B
  if (pi.use_b){
    osc.chan_set("B", pi.cpl_b.c_str(), pi.rng_b);
    ret.bufs[ret.nch] = Buf<int16_t>(buflen);
    ret.sc[ret.nch]   = pi.rng_b/osc.get_max_val();
    ret.ov[ret.nch]   = false;
    osc.set_buf("B", bufb.data, bufb.size);
    ret.nch++;
  }
  else osc.chan_disable("B");

  // calculate actual dt to find correct trigger delay
  ret.dt = osc.tbase2dt( osc.dt2tbase(pi.dt) );
  uint32_t ndel = round(pi.trig_del/ret.dt); // trig delay (samples)

  // set trigger
  if (pi.trig_src=="A"){
    int32_t trig_lvl = pi.trig_lvl/pi.rng_a*osc.get_max_val();
    osc.trig_set("A", trig_lvl, pi.trig_dir.c_str(), ndel, 0);
  }
  else if (pi.trig_src=="B"){
    int32_t trig_lvl = pi.trig_lvl/pi.rng_b*osc.get_max_val();
    osc.trig_set("B", trig_lvl, pi.trig_dir.c_str(), ndel, 0);
  }
  else if (pi.trig_src=="" || pi.trig_src=="NONE" || pi.trig_src=="OFF"){
    osc.trig_disable();
  }
  else throw Err() << "unknown trigger setting: " << pi.trig_src;
}

/********************************************************************/
// high-level record function
void record_block(PicoInt & osc, const InPars & pi){

  CBPars pars = set_osc(osc, pi, pi.nrec);

  std::cerr << "Block mode: start collecting data\n";
  osc.run_block(pi.nrec, pi.npre, &pars.dt);
  usleep(pi.nrec*pars.dt*1e6);
  while (!osc.is_ready()) usleep(pi.nrec*pars.dt*1e6/100);

  int16_t o;
  uint32_t nrec = pi.nrec;
  osc.get_block(0, &nrec, &o);

  int c = 0;
  if (use_a) pars.ov[c++] = (bool)(o&1);
  if (use_b) pars.ov[c++] = (bool)(o&2);
  pars.t0 = osc.get_trig() - pi.npre*dt + pi.trig_del;

  // print header
  pars.print(std::cout);

  std::cout << std::scientific
       << "# Signal parameters:"
       << "nchan: " << pars.nch   << "\n";
       << "t0:    " << pars.t0   << "\n"
       << "dt:    " << pars.dt   << "\n";
       << "tabs:  " << pars.tabs << "\n";
  for (int c=0;c<pars.nch){
    std::cout << "sc" << c << ": " << pars.sc[c] << "\n";
              << "ov" << c << ": " << pars.ov[c] << "\n";
  }
  std::cout << "\n";

  for (int i = 0; i<pars.bufs[0].size; i++){
    for (int ch = 0; ch < pars.nch; ch++){
      int16_t *d = pars.bufs[ch].data + i;
      std::cout.write((const char*)d, sizeof(int16_t));
    }
  }
}

/********************************************************************/
// calback
void stream_cb(int16_t h, int32_t num, uint32_t start,
                               int16_t over, uint32_t trigpos, int16_t trig,
                               int16_t autostop, void *par){

  CBPars *pars = (CBPars *)par;
  if (trig) pars->trg = pars->cnt + trigpos;
  pars->cnt += num;


std::cerr << "stream_cb " << num << "\n";
if (trig) std::cerr << "trig_pos " << trigpos << "\n";

  if (pars.nch<1 || pars->bufs[nch-1].size < start+num) return;
  for (int i=0; i<num; i++){
    for (int c = 0; c<pars.nch; c++){
      int16_t *d1 = pars->bufs[c]->data + start + i;
      std::cout.write((const char*)d1, sizeof(int16_t));
    }
  }
}


// high-level stream function
void stream(PicoInt & osc, const InPars & pi){

  double tbuf = min(100*dt, 0.1);
  int len = ceil(2*tbuf/pi.dt);

  CBPars pars = set_osc(osc, pi, len);

  // write header
  pars.print(std::cout);

  // run streaming
  std::cerr << "Streaming mode: start collecting data\n";
  osc.run_stream(pi.nrec, pi.npre, &pars.dt, len);

  std::cout << std::scientific
       << "nchan: " << pars.nch << "\n";
       << "t0: "   << pars.t0 << "\n"
       << "dt: "   << pars.dt << "\n";
  for (int c=0;c<pars.nch){
    std::cout << "sc" << c << ": " << pars.sc[c] << "\n";
              << "ov" << c << ": " << pars.ov[c] << "\n";
  }
  std::cout << "\n";


  while(1){
    usleep(pi.tbuf*1e6);
    osc.get_stream(stream_cb, (void *)&pars);
  }
  osc.stop();
}


#endif


