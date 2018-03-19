#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex>
#include <vector>
#include <fftw3.h>
#include "../signal.h"

using namespace std;

main(int argc, char *argv[]){
  try {
    if (argc!=3) throw Err() << "Usage: test_signal <infile> <outfile>";
    Signal s = read_signal(argv[1]);
    s.crop_t(0, 1e-4);
    write_sig(argv[2], s);
  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }

}
