#include <cmath>
#include "rainbow/rainbow.h"
#include "dimage.h"

void
dImage::print_pnm(std::ostream & ff,
     bool uselog, const char *grad, Opt opts) const{

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
  ff << "P6\n";

  // print comments
  opts.put("width",    w);
  opts.put("height",   h);
  opts.put("sc_width", vs);
  opts.put("colors",   grad);
  opts.put("logscale", uselog);
  opts.put("cmin",     cmin);
  opts.put("cmax",     cmax);
  for (Opt::const_iterator i=opts.begin(); i!=opts.end(); i++)
    ff << "#" << i->first << ": " << i->second << "\n";

  ff << w+vs << " " << h << "\n255\n";
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
