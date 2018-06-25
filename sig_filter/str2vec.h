#ifndef STR2VEC_H
#define STR2VEC_H

#include <vector>

// convert a string with comma/space separated integer
// numbers into an integer vector
std::vector<int> str2ivec(const char *str);

// same for doubles
std::vector<double> str2dvec(const char *str);

#endif
