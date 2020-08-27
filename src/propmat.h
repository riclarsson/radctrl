#ifndef propmat_h
#define propmat_h

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

#include "complex.h"
#include "constants.h"

namespace Absorption {
constexpr std::array<double, 7> ZeemanPropMat4x4(
    Complex c, const std::array<double, 7>& v) {
  return {v[0] * c.real(), v[1] * c.real(), v[2] * c.real(), v[3] * c.real(),
          v[4] * c.imag(), v[5] * c.imag(), v[6] * c.imag()};
}

template <size_t n>
class PropMat {
 public:
  static constexpr size_t N = n;
  static constexpr size_t SIZE = ((N == 4) ? 7 : ((N == 3) ? 4 : N));

 private:
  static_assert(N < 5 and N > 0);

  std::array<double, SIZE> p;

 public:
  constexpr PropMat(double a, double b, double c, double d, double u, double v,
                    double w) noexcept
      : p({a, b, c, d, u, v, w}) {
    static_assert(N == 4);
  }
  constexpr PropMat(double a, double b, double c, double u) noexcept
      : p({a, b, c, u}) {
    static_assert(N == 3);
  }
  constexpr PropMat(double a, double b) noexcept : p({a, b}) {
    static_assert(N == 2);
  }
  constexpr PropMat(double a) noexcept : p({a}) { static_assert(N == 1); }
  constexpr PropMat(const std::array<double, SIZE>& a) noexcept : p(a) {}
  constexpr PropMat() noexcept {
    for (size_t i = 0; i < SIZE; i++) p[i] = 0;
  }

  friend std::ostream& operator<<(std::ostream& os, PropMat pm) {
    if constexpr (N == 4)
      return os << pm[0] << ' ' << pm[1] << ' ' << pm[2] << ' ' << pm[3] << ' '
                << pm[4] << ' ' << pm[5] << ' ' << pm[6];
    else if constexpr (N == 3)
      return os << pm[0] << ' ' << pm[1] << ' ' << pm[2] << ' ' << pm[3];
    else if constexpr (N == 2)
      return os << pm[0] << ' ' << pm[1];
    else if constexpr (N == 1)
      return os << pm[0];
  }

  friend std::istream& operator>>(std::istream& is, PropMat& pm) {
    if constexpr (N == 4)
      return is >> pm[0] >> pm[1] >> pm[2] >> pm[3] >> pm[4] >> pm[5] >> pm[6];
    else if constexpr (N == 3)
      return is >> pm[0] >> pm[1] >> pm[2] >> pm[3];
    else if constexpr (N == 2)
      return is >> pm[0] >> pm[1];
    else if constexpr (N == 1)
      return is >> pm[0];
  }

  constexpr PropMat operator+(double x) const noexcept {
    auto newp = PropMat(p);
    newp.p[0] += x;
    return newp;
  }

  constexpr PropMat operator*(double x) const noexcept {
    auto newp = PropMat(p);
    std::transform(newp.p.cbegin(), newp.p.cend(), newp.p.begin(),
                   [x](auto& c) { return x * c; });
    return newp;
  }

  constexpr PropMat operator+(const std::array<double, SIZE> x) const noexcept {
    PropMat newp(p);
    std::transform(x.cbegin(), x.cend(), p.cbegin(), newp.p.begin(),
                   [](auto& a, auto& b) { return a + b; });
    return newp;
  }

  constexpr double operator[](unsigned i) const noexcept { return p[i]; }

  constexpr double inverse_denominator() const noexcept {
    using Constant::pow2;
    using Constant::pow4;
    if constexpr (N == 4)
      return pow4(p[0]) - pow2(p[0]) * pow2(p[1]) - pow2(p[0]) * pow2(p[2]) -
             pow2(p[0]) * pow2(p[3]) + pow2(p[0]) * pow2(p[N]) +
             pow2(p[0]) * pow2(p[5]) + pow2(p[0]) * pow2(p[6]) -
             pow2(p[1]) * pow2(p[6]) + 2 * p[1] * p[2] * p[5] * p[6] -
             2 * p[1] * p[3] * p[N] * p[6] - pow2(p[2]) * pow2(p[5]) +
             2 * p[2] * p[3] * p[N] * p[5] - pow2(p[3]) * pow2(p[N]);
    else if constexpr (N == 3)
      return p[0] * (pow2(p[0]) - pow2(p[1]) - pow2(p[2]) + pow2(p[N]));
    else if constexpr (N == 2)
      return pow2(p[0]) - pow2(p[1]);
    else if constexpr (N == 1)
      return p[0];
  }

  constexpr bool can_be_inversed() const noexcept {
    return inverse_denominator() not_eq 0;
  }
};
}  // namespace Absorption

#endif  // propmat_h
