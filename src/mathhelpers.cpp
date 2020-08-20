#include "mathhelpers.h"


std::vector<double> linspace(double s, double e, int count) {
  std::vector<double> ls(count);
  
  if (count == 0) {
    return ls;
  } else if (count == 1) {
    ls.front() = (e + s) / 2;
    return ls;
  } else {
    const double step = (e - s) / (count - 1);
    ls.front() = s;
    ls.back() = e;
    for(int i=1; i < count-1; ++i)
      ls[i] = s + step * i;
    return ls;
  }
}
