#include <fstream>
#include "signal/signal.h"


using namespace std;

// print help
void help(){
  cout << "sig2wav -- convert SIG-file to WAV.\n"
          "Usage: sig2wav <in file> <out file>\n"
  ;
}

int
main(int argc, char *argv[]){
  try {
    if (argc!=3) { help(); return 0; }
    std::ifstream fi(argv[1]);
    std::ofstream fo(argv[2]);
    write_wav(fo, read_signal(fi));
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
