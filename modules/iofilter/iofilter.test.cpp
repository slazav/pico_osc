/* Test program! */
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>

#include "err/err.h"
#include "iofilter.h"



int
main(){

  {
    std::ifstream ff("iofilter.test.cpp");
    IFilter flt(ff, "cat");

    std::string l;
    std::getline(flt.stream(), l);
    assert(l == "/* Test program! */");
  }

  {
    IFilter flt("echo abc");
    std::string l;
    std::getline(flt.stream(), l);
    assert(l == "abc");
  }


//  std::cerr << "[" << flt.stream().rdbuf() << "]\n";
  return 0;
}
