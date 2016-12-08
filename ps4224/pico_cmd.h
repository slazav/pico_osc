#ifndef PICO_CMD_H
#define PICO_CMD_H

// high-level commands for work with pico-devices

#include <string>
#include <stdint.h>
#include <unistd.h> // usleep
#include <cmath>
#include <cstdlib>
#include "err.h"
#include "buf.h"
#include "pico_int.h"

// input parameters for high-level record function
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
};

// output parameters for high-level record function
struct OutPars{
  Buf<int16_t> bufa, bufb;    // data buffers
  float t0, dt;     // tstart, tstep
  float sc_a, sc_b; // data scale
  bool overflow_a, overflow_b;
  // defaults
  OutPars(){
    t0=dt=0;
    sc_a=sc_b=1;
    overflow_a=overflow_b=false;
  }
};

// high-level record function
OutPars record_block(PicoInt & osc, const InPars & pi){
  OutPars po;

  // set chan A
  if (pi.use_a){
    osc.chan_set("A", pi.cpl_a.c_str(), pi.rng_a);
    po.bufa = Buf<int16_t>(pi.nrec);
    osc.set_buf("A", po.bufa);
  }
  else osc.chan_disable("A");

  // set chan B
  if (pi.use_b){
    osc.chan_set("B", pi.cpl_b.c_str(), pi.rng_b);
    po.bufb = Buf<int16_t>(pi.nrec);
    osc.set_buf("B", po.bufb);
  }
  else osc.chan_disable("B");

  // calculate actual dt to find correct trigger delay
  po.dt = osc.tbase2dt( osc.dt2tbase(pi.dt) );
  uint32_t ndel = round(pi.trig_del/po.dt); // trig delay (samples)

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

  po.dt = pi.dt;
  osc.run_block(pi.nrec, pi.npre, &po.dt);
  usleep(pi.nrec*po.dt*1e6);
  while (!osc.is_ready()) usleep(pi.nrec*po.dt*1e6/100);

  int16_t o;
  uint32_t n = pi.nrec;
  osc.get_data(0, &n, &o);
  po.overflow_a = (bool)(o&1);
  po.overflow_b = (bool)(o&2);

  po.t0 = osc.get_trig() - pi.npre*po.dt + ndel*po.dt;
  po.sc_a = pi.rng_a/osc.get_max_val();
  po.sc_b = pi.rng_b/osc.get_max_val();

  return po;
}

#endif


