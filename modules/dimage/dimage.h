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
  int w,h,vs;
  double amin,amax;

  dImage(int w_, int h_, double v,
         double amin_=-HUGE_VAL, double amax_=+HUGE_VAL):
         w(w_),h(h_),amin(amin_),amax(amax_),vector(w_*h_,v){}

  double get(int x, int y) const {return (*this)[w*y+x];}
  void   set(int x, int y, const double v) { (*this)[w*y+x] = v;}
  void   add(int x, int y, const double v) { (*this)[w*y+x] += v;}
  void   set_vs(const double vs_) {vs=vs_;}

  void print_pnm(std::ostream & ff,
        bool uselog = false, const char *grad = "KRYW") const{
    // find data range (for logscale ignore values <=0)
    double cmin=+HUGE_VAL;
    double cmax=-HUGE_VAL;
    for (int y=0; y<h; y++){
      for (int x=0; x<w; x++){
        double v = get(x,y);
        if (uselog && v<=0) continue;
        if (uselog) v = log(v);
        if (v > cmax) cmax = v;
        if (v < cmin) cmin = v;
      }
    }
    std::cerr << "picture min: " << cmin << " max: " << cmax << "\n";
    if (amin!=-HUGE_VAL) cmin=amin;
    if (amax!=+HUGE_VAL) cmax=amax;
    Rainbow R(cmin, cmax, grad);

    // print data
    ff << "P6\n" << w+vs << " " << h << "\n255\n";
    for (int y=0; y<h; y++){
      for (int x=0; x<w+vs; x++){
        double v;
        if (x<w) {
          v = get(x,y);
          if (uselog) v = (v<=0)? cmin : log(v);
        }
        else {
          v = cmin + (cmax-cmin)*(h-y-1)/(double)(h-1);
        }
        int32_t col = R.get(v);
        char *cc = (char *)&col;
        ff << cc[2] << cc[1] << cc[0];
      }
    }
  }
};

#endif