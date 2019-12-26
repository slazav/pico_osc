#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

void
process_data(const std::vector<double> & data){
  for (int i=0; i<data.size(); i++)
    std::cout << data[i] << " ";
  std::cout << "\n";
}


int
main (void) {

  std::vector<double> data;

  double fprev=0, fdiff=0;
  int nall=0, nswp=0;

  while (!std::cin.eof()){
    std::string l;
    getline(std::cin, l);

    std::istringstream ss(l);
    double f;
    ss >> f;

    // reset sweep
    if (nswp>0 && (f-fprev)*fdiff <= 0){

      nswp = 0;
      fdiff = f-fprev;
      if (data.size()>2) process_data(data);
      data.clear();
      data.push_back(fprev);
    }
    else {
      fdiff = f-fprev;
    }


    data.push_back(f);
    nall++;
    nswp++;
    fprev=f;
  }
  if (data.size()>2) process_data(data);
  data.clear();

  std::cout << "\n";
  return 0;
}
