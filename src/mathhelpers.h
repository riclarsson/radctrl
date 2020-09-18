#ifndef mathhelpers_h
#define mathhelpers_h

#include <numeric>
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

template <class T>
T sum(const std::vector<T>& v) {
  return std::reduce(v.cbegin(), v.cend(), T{0});
}

template <class T>
T mean(const std::vector<T>& v) {
  T out{0};
  const T invdiv = 1.0 / v.size();
  for (auto& x : v) out += x * invdiv;
  return out;
}

template <class T>
T max(const std::vector<T>& v) {
  if (v.size() == 0) {
    return T{};
  } else if (v.size() == 1) {
    return v[0];
  } else {
    T out = v[0];
    for (auto& x : v)
      if (x > out) out = x;
    return out;
  }
}


double BesselJ1(double x) noexcept;

#endif  // mathhelpers_h
