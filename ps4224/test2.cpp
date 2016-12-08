#include "ps4224.h"
#include "err.h"
#include <unistd.h>


int
main(){
  try {

    PS4224 osc;
    PS4224::InPars pi;

    pi.rng_a = 2;
    pi.rng_b = 0.05;
    pi.cpl_a = "DC";
    pi.cpl_b = "AC";
    pi.dt = 1e-5;
    pi.nrec = 300*1024;
    pi.npre = 0;
    pi.trig_src = "A";
    pi.trig_lvl = 0.005;
    pi.trig_del = 0.0032;

    PS4224::OutPars po = osc.record(pi);

    for (int i = 0; i<po.bufa.size; i++){
      double x = po.t0 + po.dt*i;
      double y = po.sc_a * po.bufa.data[i];
      double z = po.sc_b * po.bufb.data[i];
      std::cout << x << "\t" << y << "\t" << z << "\n";
    }
    if (po.overflow_a) std::cerr << "Overload A\n";
    if (po.overflow_b) std::cerr << "Overload B\n";

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
