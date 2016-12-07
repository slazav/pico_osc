#include "ps4224.h"
#include "err.h"
#include <unistd.h>

int
main(){
  try {
  std::cerr << "list available devices:\n";
  std::cerr << PS4224::dev_list() << "\n";


  PS4224 osc;

  std::cerr << "channel ranges:\n";
  std::cerr << " A: " << osc.chan_get_ranges("A") << "\n";
  std::cerr << " B: " << osc.chan_get_ranges("B") << "\n";


  std::cerr << "set channels and trigger:\n";
  osc.chan_set("A", "AC", 0.05);
  osc.chan_set("B", "AC", 0.05);
//  osc.chan_disable("B");
//  osc.chan_disable("A");
  osc.trig_set("B", 5000, "rising", 0,0);
//  osc.trig_disable();

  std::cerr << "timebase -> time -> timebase conversions\n";
  uint32_t tb;
  tb = 0;
  std::cerr << " " << tb << " -> " << osc.tbase2dt(tb) << 
                            " -> " << osc.dt2tbase(osc.tbase2dt(tb)) << "\n";
  tb = 1;
  std::cerr << " " << tb << " -> " << osc.tbase2dt(tb) << 
                            " -> " << osc.dt2tbase(osc.tbase2dt(tb)) << "\n";
  tb = 5;
  std::cerr << " " << tb << " -> " << osc.tbase2dt(tb) << 
                            " -> " << osc.dt2tbase(osc.tbase2dt(tb)) << "\n";
  tb = 1024;
  std::cerr << " " << tb << " -> " << osc.tbase2dt(tb) << 
                            " -> " << osc.dt2tbase(osc.tbase2dt(tb)) << "\n";


  std::cerr << "run block\n";
  float dt = 1e-6;
  uint32_t nrec = 256, npre = 16;

  Buf<int16_t> b1(nrec), b2(nrec);
  osc.set_buf("A", b1);
  osc.set_buf("B", b2);

  osc.run_block(nrec,npre, &dt);
  while (!osc.is_ready()){
    usleep(nrec*dt*1e6);
  }

  int16_t o;
  osc.get_data(0, &nrec, &o);
  std::cerr << "  get " << nrec << " points, overflow: " << o << "\n";

  std::cerr << "  trigger: " << osc.get_trig() << "\n";

  for (int i = 0; i<b1.size; i++){
    double t = dt*i;
    std::cout << t << "\t" << b1.data[i] << "\t" << b2.data[i] << "\n";
  }


  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
