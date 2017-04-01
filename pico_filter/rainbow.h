#ifndef RAINBOW_H
#define RAINBOW_H

// Rainbow class from mapsoft project

/* Example:
      struct rainbow_data RD[]={ // user-defined rainbow data
        {0.1, 0x000000},
        {0.5, 0xFF0000}, // 0.1 - 0.5 black -> blue
        {0.5, 0xFF00FF}, // - color step
        {0.9, 0x000000}, // 0.5 - 0.9 magenta -> black
      };
      int RDS = sizeof(RD)/sizeof(rainbow_data);

      int c1=get_rainbow(v, RD, RDS); // get color for v!

  Simple example:

    simple_rainbow sr(0.1, 0.9, type=RAINBOW_NORMAL);
    c=sr.get(v); // get data

  Shade function:
    int c=color_shade(c, 0.2);

*/
///@{

/// rainbow data type
struct rainbow_data{
  double v; ///< value
  int c;    ///< color
};

int get_rainbow(double val, const rainbow_data RD[], int rd_size);

int color_shade(int c, double k);

/// predefined gradient types for simple_rainbow class
enum rainbow_type{
  RAINBOW_NORMAL,   ///< usual rainbow, b-c-g-y-r-m
  RAINBOW_BURNING,  ///< white-y-r-m-b-dark blue gradient
  RAINBOW_BURNING1  ///< k-r-y-w
};

/*******************/

/** Class for simple gradients
*/
class simple_rainbow{
  static const int max_rd_size=6;
  rainbow_data RD[max_rd_size];
  int rd_size;

public:
  simple_rainbow(double min, double max, rainbow_type type=RAINBOW_NORMAL);
  simple_rainbow(double min, double max, int cmin, int cmax);

  void set_range(double min, double max); ///<reset range
  double get_min() const;
  double get_max() const;

  int get(double val) const;
  int get_bnd(double val, int low_c=0, int high_c=0) const;

  const rainbow_data * get_data() const; ///< get pointer to the color table
  int get_size() const; ///< get color table size
};

#endif