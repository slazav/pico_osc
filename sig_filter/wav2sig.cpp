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
    const char *i_file = argv[1];
    const char *o_file = argv[2];

    write_signal(o_file, read_wav(i_file));
  }

  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
