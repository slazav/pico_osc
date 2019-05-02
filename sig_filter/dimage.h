#ifndef DIMAGE_H
#define DIMAGE_H

#include <iostream>
#include <vector>
#include "rainbow/rainbow.h"

/******************************************************************/
// Strange double image
// You fill it with doubles and then you dump in to PNM
class dImage:std::vector<double> {
  public:
  int w,h;
  double amin,amax;
  dImage(int w_, int h_, double v,
         double amin_=-HUGE_VAL, double amax_=+HUGE_VAL):
         w(w_),h(h_),amin(amin_),amax(amax_),vector(w_*h_,v){}

  double get(int x, int y) const {return (*this)[w*y+x];}
  void   set(int x, int y, double v) { (*this)[w*y+x] = v;}

  void print_pnm(std::ostream & ff) const{
    // find data range
    double cmin=get(0,0);
    double cmax=cmin;
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        if (get(x,y) > cmax) cmax = get(x,y);
        if (get(x,y) < cmin) cmin = get(x,y);
      }
    }
    std::cerr << "picture min: " << cmin << " max: " << cmax << "\n";
    if (amin!=-HUGE_VAL) cmin=amin;
    if (amax!=+HUGE_VAL) cmax=amax;
    Rainbow R(cmin, cmax, "KRYW");
    // print data
    ff << "P6\n" << w << " " << h << "\n255\n";
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        int32_t col = R.get(get(x,y));
        char *cc = (char *)&col;
        ff << cc[2] << cc[1] << cc[0];
      }
    }
  }
};

#endif