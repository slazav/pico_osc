#include "oscsig.h"

int
main(){

  double f0 = 32288.0;
  double tau0 = 0.5;
  double n = 1;
//  for (tau0=0.1; tau0<=10; tau0+=0.1){
    for (double n=0; n<3; n+=0.05){
      OscSig s(100000, 1e-5);
      s.mk_test_func(1.0, f0, 0, tau0, n);
      s.fft();
      Lor l1 = s.find_peak(25000,45000);
      std::cout << n << " " << (l1.fre-f0)/f0 << " "  << (l1.tau-tau0)/tau0 << "\n";
//  s.text_dump();
//    }
//    std::cout << "\n";
  }
}