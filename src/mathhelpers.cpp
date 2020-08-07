#include "mathhelpers.h"

std::vector<float> linspace(float s, float e, int count) {
  std::vector<float> ls(count);
  
  if (count == 0) {
    return ls;
  } else if (count == 1) {
    ls.front() = (e + s) / 2;
    return ls;
  } else {
    const float step = (e - s) / (count - 1);
    ls.front() = s;
    ls.back() = e;
    for(int i=1; i < count-1; ++i)
      ls[i] = s + step * i;
    return ls;
  }
}
