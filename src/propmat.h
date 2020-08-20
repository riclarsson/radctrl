#ifndef propmat_h
#define propmat_h

#include <array>
#include <iostream>
#include <vector>

#include "constants.h"

namespace Absorption {
template <unsigned int n>
class PropMat {
  static constexpr unsigned int N = n;
  static constexpr unsigned int SIZE = ((N == 4) ? 7 : ((N == 3) ? 4 : N));
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

  friend std::ostream& operator<<(std::ostream& os, PropMat pm) {
    if constexpr (N == 4)
      std::cout << pm[0] << ' ' << pm[1] << ' ' << pm[2] << ' ' << pm[3] << ' '
                << pm[4] << ' ' << pm[5] << ' ' << pm[6];
    else if constexpr (N == 3)
      std::cout << pm[0] << ' ' << pm[1] << ' ' << pm[2] << ' ' << pm[3];
    else if constexpr (N == 2)
      std::cout << pm[0] << ' ' << pm[1];
    else if constexpr (N == 1)
      std::cout << pm[0];
    return os;
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
