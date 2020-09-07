#ifndef mathhelpers_h
#define mathhelpers_h

#include <vector>

template <class T>
std::vector<T> linspace(T s, T e,
                        typename std::vector<T>::size_type count) noexcept {
  std::vector<T> ls(count);

  if (count == 0) {
    return ls;
  } else if (count == 1) {
    ls.front() = (e + s) / 2;
    return ls;
  } else {
    const T step = (e - s) / (count - 1);
    ls.front() = s;
    ls.back() = e;
    for (typename std::vector<T>::size_type i = 1; i < count - 1; ++i)
      ls[i] = s + step * i;
    return ls;
  }
}

template <class T>
std::vector<T> scale(const std::vector<T>& v, const double x) {
  auto out(v);
  for (auto& y : out) y *= x;
  return out;
}

#endif  // mathhelpers_h
