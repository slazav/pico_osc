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
  float tbuf;    // length of buffer in seconds

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
    tbuf  = 0.1;
  }
};


/********************************************************************/
// high-level record function
void record_block(PicoInt & osc, const InPars & pi){

  Buf<int16_t> bufa,bufb;

  // set chan A
  if (pi.use_a){
    osc.chan_set("A", pi.cpl_a.c_str(), pi.rng_a);
    bufa = Buf<int16_t>(pi.nrec);
    osc.set_buf("A", bufa);
  }
  else osc.chan_disable("A");

  // set chan B
  if (pi.use_b){
    osc.chan_set("B", pi.cpl_b.c_str(), pi.rng_b);
    bufb = Buf<int16_t>(pi.nrec);
    osc.set_buf("B", bufb);
  }
  else osc.chan_disable("B");

  // calculate actual dt to find correct trigger delay
  float dt = osc.tbase2dt( osc.dt2tbase(pi.dt) );
  uint32_t ndel = round(pi.trig_del/dt); // trig delay (samples)

  // set trigger
  if (pi.trig_src=="A" || pi.trig_src=="B"){
    int32_t trig_lvl=0;
    if (pi.trig_src=="A")
      trig_lvl = pi.trig_lvl/pi.rng_a*PS4000_MAX_VALUE;
    if (pi.trig_src=="B")
      trig_lvl = pi.trig_lvl/pi.rng_b*PS4000_MAX_VALUE;
    osc.trig_set(pi.trig_src.c_str(), trig_lvl,
                 pi.trig_dir.c_str(), ndel, 0);
  }
  else osc.trig_disable();

  std::cerr << "Start collecting data\n";
  osc.run_block(pi.nrec, pi.npre, &dt);
  usleep(pi.nrec*dt*1e6);
  while (!osc.is_ready()) usleep(pi.nrec*dt*1e6/100);

  int16_t o;
  uint32_t nrec = pi.nrec;
  osc.get_block(0, &nrec, &o);
  bool overflow_a = (bool)(o&1);
  bool overflow_b = (bool)(o&2);

  double t0 = osc.get_trig() - pi.npre*dt + ndel*dt;
  double sc_a = pi.rng_a/osc.get_max_val();
  double sc_b = pi.rng_b/osc.get_max_val();

  // dump results
  std::cout << std::scientific
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
       << "nchan: " << int(pi.use_a) + int(pi.use_b) << "\n";

  std::cout << std::scientific
       << "t0: "   << t0 << "\n"
       << "dt: "   << dt << "\n"
       << "sc_a: " << sc_a << "\n"
       << "sc_b: " << sc_b << "\n"
       << "overflow_a: " << overflow_a << "\n"
       << "overflow_b: " << overflow_b << "\n";
  std::cout << "\n";

  int n = std::max(bufa.size, bufb.size);
  for (int i = 0; i<n; i++){
    if (bufa.size == n){
      int16_t *d = bufa.data + i;
      std::cout.write((const char*)d, sizeof(int16_t));
    }
    if (bufb.size == n){
      int16_t *d = bufb.data + i;
      std::cout.write((const char*)d, sizeof(int16_t));
    }
  }
}

/********************************************************************/
// parameter structure for the callback
struct CBPar{
  bool use_a, use_b;
  Buf<int16_t> *bufa;
  Buf<int16_t> *bufb;
};

/********************************************************************/
// calback
void stream_cb(int16_t h, int32_t num, uint32_t start,
                               int16_t over, uint32_t trigpos, int16_t trig,
                               int16_t autostop, void *par){
  CBPar *pars = (CBPar *)par;

  int n = std::max(pars->bufa->size, pars->bufb->size);
//std::cerr << "stream_cb " << num << "\n";
//if (trig) std::cerr << "trig_pos " << trigpos << "\n";
  if (n<num+start) return;
  for (int i=0; i<num; i++){
    if (pars->use_a){
      int16_t *d1 = pars->bufa->data + start + i;
      std::cout.write((const char*)d1, sizeof(int16_t));
    }
    if (pars->use_b){
      int16_t *d2 = pars->bufb->data + start + i;
      std::cout.write((const char*)d2, sizeof(int16_t));
    }
  }
}


// high-level stream function
void stream(PicoInt & osc, const InPars & pi){

  int len = ceil(2*pi.tbuf/pi.dt);
  Buf<int16_t> bufa, bufb;
  // set chan A
  if (pi.use_a){
    osc.chan_set("A", pi.cpl_a.c_str(), pi.rng_a);
    bufa = Buf<int16_t>(len);
    osc.set_buf("A", bufa);
  }
  else osc.chan_disable("A");

  // set chan B
  if (pi.use_b){
    osc.chan_set("B", pi.cpl_b.c_str(), pi.rng_b);
    bufb = Buf<int16_t>(len);
    osc.set_buf("B", bufb);
  }
  else osc.chan_disable("B");

  // calculate actual dt to find correct trigger delay
  float dt = osc.tbase2dt( osc.dt2tbase(pi.dt) );
  uint32_t ndel = round(pi.trig_del/dt); // trig delay (samples)

  // set trigger
  if (pi.trig_src=="A" || pi.trig_src=="B"){
    int32_t trig_lvl=0;
    if (pi.trig_src=="A")
      trig_lvl = pi.trig_lvl/pi.rng_a*PS4000_MAX_VALUE;
    if (pi.trig_src=="B")
      trig_lvl = pi.trig_lvl/pi.rng_b*PS4000_MAX_VALUE;
    osc.trig_set(pi.trig_src.c_str(), trig_lvl,
                 pi.trig_dir.c_str(), ndel, 0);
  }
  else osc.trig_disable();

  // parameter structure for the callback
  CBPar pars;
  pars.use_a = pi.use_a;
  pars.use_b = pi.use_b;
  pars.bufa = &bufa;
  pars.bufb = &bufb;

  double sc_a = pi.rng_a/osc.get_max_val();
  double sc_b = pi.rng_b/osc.get_max_val();

  // write header
  std::cout << std::scientific
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
       << "tbuf:  " << pi.tbuf << "\n"
       << "nchan: " << int(pi.use_a) + int(pi.use_b) << "\n";

  std::cout << std::scientific
       << "dt: "   << dt << "\n"
       << "sc_a: " << sc_a << "\n"
       << "sc_b: " << sc_b << "\n";
  std::cout << "\n";

  // run streaming
  std::cerr << "Start collecting data\n";
  osc.run_stream(pi.nrec,pi.npre, &dt, len);

  while(1){
    usleep(pi.tbuf*1e6);
    osc.get_stream(stream_cb, (void *)&pars);
  }
  osc.stop();
}


#endif


