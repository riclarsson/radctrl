#ifndef testhelpers_h
#define testhelpers_h

#include <vector>

template <class T, class X>
std::vector<T> add_modify_vector(std::vector<T> x, X mod, size_t pos) {
  std::vector<T> out(x);
  out[pos] += mod;
  return out;
}

#endif  // testhelpers_h
