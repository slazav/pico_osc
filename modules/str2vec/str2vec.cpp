#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std;

// convert a string with comma/space separated integer numbers into an integer vector
vector<int> str2ivec(const char *str){
  char *s, *t, *saveptr;
  const char * delim="\n\t ,";
  vector<int> ret;
  for (s = (char*)str; ;s = NULL) {
    t = strtok_r(s, delim, &saveptr);
    if (t == NULL) break;
    ret.push_back(atoi(t));
  }
  return ret;
}

// same for doubles
vector<double> str2dvec(const char *str){
  char *s, *t, *saveptr;
  const char * delim="\n\t ,";
  vector<double> ret;
  for (s = (char*)str; ;s = NULL) {
    t = strtok_r(s, delim, &saveptr);
    if (t == NULL) break;
    ret.push_back(atof(t));
  }
  return ret;
}
