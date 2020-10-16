#ifndef mathhelpers_h
#define mathhelpers_h

#include <array>
#include <numeric>
#include <ostream>
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

namespace Interp {
template <size_t N>
constexpr double sum(const std::array<double, N>& vals) {
  return std::accumulate(vals.cbegin(), vals.cend(), 0);
}

constexpr double weight(double x, double x0, double x1) {
  return x0 == x1 ? 1.0 : (x - x1) / (x0 - x1);
}

template <typename... Xs>
constexpr std::array<double, sizeof...(Xs) - 1> weight(double x, Xs... xs) {
  const std::array<double, sizeof...(Xs)> gx = {xs...};
  std::array<double, sizeof...(Xs) - 1> w;
  w.fill(0);
  for (size_t i = 0; i < sizeof...(Xs) - 1; i++) {
    for (size_t j = 0; j < sizeof...(Xs); j++) {
      if (i not_eq j) {
        w[i] += (x - gx[j]) / (gx[i] - gx[j]);
      }
    }
  }
  return w;
}
}  // namespace Interp

class LinearInterpPoint {
  double w;       // Lower weight
  std::size_t i;  // Lower index
 public:
  constexpr LinearInterpPoint(double W = 1, std::size_t I = 0) noexcept
      : w(W), i(I) {}
  constexpr LinearInterpPoint(const LinearInterpPoint& lip) noexcept
      : w(lip.w), i(lip.i) {}
  constexpr LinearInterpPoint& operator=(
      const LinearInterpPoint& lip) noexcept {
    w = lip.w;
    i = lip.i;
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os,
                                  const LinearInterpPoint& lip) {
    return os << lip.w << ' ' << lip.i;
  }
  constexpr double weight() const noexcept { return w; }
  constexpr std::size_t index() const noexcept { return i; }
};

#endif  // mathhelpers_h
