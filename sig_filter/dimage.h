#ifndef DIMAGE_H
#define DIMAGE_H

#include <iostream>
#include <vector>
#include "rainbow.h"

/******************************************************************/
// Strange double image
// You fill it with doubles and then you dump in to PNM
class dImage:std::vector<double> {
  public:
  int w,h;
  dImage(int w_, int h_, double v):w(w_),h(h_),vector(w_*h_,v){}

  double get(int x, int y) const {return (*this)[w*y+x];}
  void   set(int x, int y, double v) { (*this)[w*y+x] = v;}

  void print_pnm(std::ostream & ff) const{
    // find data range
    double cmax=0;
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        if (get(x,y) > cmax) cmax = get(x,y);
      }
    }
    simple_rainbow sr(0, cmax, RAINBOW_BURNING1);
    // print data
    ff << "P6\n" << w << " " << h << "\n255\n";
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        int32_t c = sr.get(get(x,y));
        ff.write((char *)&c, 3);
      }
    }
  }
};

#endif