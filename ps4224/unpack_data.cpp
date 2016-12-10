#include "err.h"
#include <iostream>
#include <string>
#include <cstdlib>

// unpack data written by pico4224block/pico4224stream

using namespace std;
int
main(){
  try {

  double sc_a=1, sc_b=1, t0=0, dt=1;
  bool use_a=true, use_b=true;

  // read metadata <name>:<value>
  while (!cin.eof()){
    string l;
    getline(cin, l);

    if (l=="") break;
    cout << "# " << l << "\n";
    // extract values for conversion
    size_t i = l.find(":");
    if (i!=string::npos){
      string key = l.substr(0,i);
      string val = l.substr(i+1);
      if (key=="sc_a") sc_a = atof(val.c_str());
      if (key=="sc_b") sc_b = atof(val.c_str());
      if (key=="t0") t0 = atof(val.c_str());
      if (key=="dt") dt = atof(val.c_str());
      if (key=="use_a") use_a = atoi(val.c_str());
      if (key=="use_b") use_b = atoi(val.c_str());
    }
  }

  // convert data
  const int len=1024;
  long int cnt=0;
  int16_t buf[len];
  while (!cin.eof()){
    cin.read((char *)buf, len*sizeof(int16_t));
    size_t n = cin.gcount()/sizeof(int16_t);
    if (use_a && use_b){
      for (int i=0; i<n; i+=2){
       double x = t0 + (cnt+i/2)*dt;
       double y = sc_a * buf[i];
       double z = sc_b * buf[i+1];
       cout << scientific << x << "\t" << y << "\t" << z << "\n";
      }
      cnt+=n/2;
    }
    else if (use_a || use_b){
      double sc = use_a? sc_a:sc_b;
      for (int i=0; i<n; i+=1){
        double x = t0 + (cnt+i)*dt;
        double y = sc * buf[i];
        cout << scientific << x << "\t" << y << "\n";
      }
      cnt+=n;
    }
  }

  }
  catch (Err E){
    std::cerr << E.str() << "\n";
  }
}
