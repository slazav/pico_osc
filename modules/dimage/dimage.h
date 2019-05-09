#ifndef DIMAGE_H
#define DIMAGE_H

#include <iostream>
#include <vector>
#include "opt/opt.h"

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
        bool uselog = false, const char *grad = "KRYW", Opt opts = Opt()) const;
};

#endif