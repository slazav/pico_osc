#include <fstream>
#include "data.h"
#include "signal.h"


using namespace std;

// print help
void help(){
  cout << "wav2sig -- convert WAV-file to SIG format.\n"
          "Usage: wav2sig <in file> <out file>\n"
  ;
}

int
main(int argc, char *argv[]){
  try {
    if (argc!=3) { help(); return 0; }
    std::ifstream fi(argv[1]);
    std::ofstream fo(argv[2]);
    write_sig(fo, read_wav(fi));
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
