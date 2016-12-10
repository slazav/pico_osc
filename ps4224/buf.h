#ifndef BUF_H
#define BUF_H
#include <cassert>

// Data buffer
template <typename T>
class Buf{

  public:
    T      *data;       /* main data array */
    int     size;       /* data size       */
    double  start,step; /* x-coord start and step*/

  /************************************/
  /* Refcounter and memory management */

  private:
    int * refcounter;

    void create(int size_, double step_, double start_){
      size  = size_;
      step  = step_;
      start = start_;
      data = new T[size];
      assert(data);
      refcounter   = new int;
      *refcounter  = 1;
    }
    void copy(const Buf & other){
      data  = other.data;
      size  = other.size;
      refcounter = other.refcounter;
      (*refcounter)++;
      assert(*refcounter >0);
    }
    void destroy(void){
      (*refcounter)--;
      if (*refcounter<=0){
      delete [] data;
      delete refcounter;
    }
  }

  /************************************/
  /* Copy constructor, destructor, assignment */
  public:

    Buf(const Buf & other){ copy(other); }

    Buf & operator=(const Buf & other){
      if (this != &other){ destroy(); copy(other); }
      return *this;
    }

    ~Buf(){ destroy(); }

  /************************************/
  /* Main constructor -- create uninitialized array */
  Buf(int size=0, double step=1, double start=0){ create(size,step,start); }


  // get_index_range
  void get_index_range(double v1, double v2, int *i1, int *i2) const{
    *i1 = floor((v1-start)/step);
    *i2 = ceil((v2-start)/step);
    if (*i1<0)    *i1=0;
    if (*i2>size) *i2=size;
    if (*i2<=*i1) *i2=*i1;
    return;
  }
  // x-value at a given index
  double get_x(int i) const { return start+step*i; }


};

#endif
