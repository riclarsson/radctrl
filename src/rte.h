#ifndef rte_h
#define rte_h

#include <array>
#include <iostream>
#include <vector>

#include "complex.h"
#include "constants.h"
#include "enums.h"
#include "grids.h"
#include "propmat.h"
#include "units.h"

namespace RTE {
template <size_t n>
class RadVec {
  static constexpr size_t N = n;
  static_assert(N < 5 and N > 0);
  std::array<double, N> b;

 public:
  constexpr RadVec(double a1, double a2, double a3, double a4) noexcept
      : b({a1, a2, a3, a4}) {
    static_assert(N == 4);
  }
  constexpr RadVec(double a1, double a2, double a3) noexcept : b({a1, a2, a3}) {
    static_assert(N == 3);
  }
  constexpr RadVec(double a1, double a2) noexcept : b({a1, a2}) {
    static_assert(N == 2);
  }
  constexpr RadVec(double a1) noexcept : b({a1}) { static_assert(N == 1); }

  constexpr RadVec(Absorption::PropMat<n> pm) noexcept {
    for (size_t i = 0; i < N; i++) b[i] = pm[i];
  }

  constexpr RadVec() noexcept {
    for (size_t i = 0; i < N; i++) b[i] = 0;
  }

  friend std::ostream &operator<<(std::ostream &os, RadVec a) {
    if constexpr (RadVec::N == 4) {
      return os << a[0] << ' ' << a[1] << ' ' << a[2] << ' ' << a[3];
    } else if constexpr (RadVec::N == 3) {
      return os << a[0] << ' ' << a[1] << ' ' << a[2];
    } else if constexpr (RadVec::N == 2) {
      return os << a[0] << ' ' << a[1];
    } else if constexpr (RadVec::N == 1) {
      return os << a[0];
    }
  }

  constexpr double operator[](size_t i) const noexcept { return b[i]; }

  constexpr RadVec operator+(RadVec rv) const noexcept {
    if constexpr (N == 4) {
      return RadVec{b[0] + rv[0], b[1] + rv[1], b[2] + rv[2], b[3] + rv[3]};
    } else if constexpr (RadVec::N == 3) {
      return RadVec{b[0] + rv[0], b[1] + rv[1], b[2] + rv[2]};
    } else if constexpr (RadVec::N == 2) {
      return RadVec{b[0] + rv[0], b[1] + rv[1]};
    } else if constexpr (RadVec::N == 1) {
      return RadVec{b[0] + rv[0]};
    }
  }

  constexpr RadVec operator*(double x) const noexcept {
    if constexpr (N == 4) {
      return RadVec{b[0] * x, b[1] * x, b[2] * x, b[3] * x};
    } else if constexpr (RadVec::N == 3) {
      return RadVec{b[0] * x, b[1] * x, b[2] * x};
    } else if constexpr (RadVec::N == 2) {
      return RadVec{b[0] * x, b[1] * x};
    } else if constexpr (RadVec::N == 1) {
      return RadVec{b[0] * x};
    }
  }

  friend constexpr RadVec operator*(double x, RadVec rv) noexcept {
    return rv * x;
  }

  constexpr RadVec operator-(RadVec rv) const noexcept {
    if constexpr (N == 4) {
      return RadVec{b[0] - rv[0], b[1] - rv[1], b[2] - rv[2], b[3] - rv[3]};
    } else if constexpr (RadVec::N == 3) {
      return RadVec{b[0] - rv[0], b[1] - rv[1], b[2] - rv[2]};
    } else if constexpr (RadVec::N == 2) {
      return RadVec{b[0] - rv[0], b[1] - rv[1]};
    } else if constexpr (RadVec::N == 1) {
      return RadVec{b[0] - rv[0]};
    }
  }

  constexpr RadVec operator/(double x) const noexcept {
    if constexpr (N == 4) {
      return RadVec{b[0] / x, b[1] / x, b[2] / x, b[3] / x};
    } else if constexpr (RadVec::N == 3) {
      return RadVec{b[0] / x, b[1] / x, b[2] / x};
    } else if constexpr (RadVec::N == 2) {
      return RadVec{b[0] / x, b[1] / x};
    } else if constexpr (RadVec::N == 1) {
      return RadVec{b[0] / x};
    }
  }
};  // RadVec

template <size_t n>
class TraMat {
  static constexpr size_t N = n;
  static_assert(N < 5 and N > 0);
  std::array<double, N * N> a;

 public:
  constexpr TraMat(double a00, double a01, double a02, double a03, double a10,
                   double a11, double a12, double a13, double a20, double a21,
                   double a22, double a23, double a30, double a31, double a32,
                   double a33) noexcept
      : a({a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23, a30, a31,
           a32, a33}) {
    static_assert(N == 4);
  }
  constexpr TraMat(double a00, double a01, double a02, double a10, double a11,
                   double a12, double a20, double a21, double a22) noexcept
      : a({a00, a01, a02, a10, a11, a12, a20, a21, a22}) {
    static_assert(N == 3);
  }
  constexpr TraMat(double a00, double a01, double a10, double a11) noexcept
      : a({a00, a01, a10, a11}) {
    static_assert(N == 2);
  }
  constexpr TraMat(double a00) noexcept : a({a00}) { static_assert(N == 1); }
  constexpr TraMat(Absorption::PropMat<4> p) noexcept
      : a({p[0], p[1], p[2], p[3], p[1], p[0], p[N], p[5], p[2], -p[N], p[0],
           p[6], p[3], -p[5], -p[6], p[0]}) {
    static_assert(N == 4);
  }
  constexpr TraMat(Absorption::PropMat<3> p) noexcept
      : a({p[0], p[1], p[2], p[1], p[0], p[N], p[2], -p[N], p[0]}) {
    static_assert(N == 3);
  }
  constexpr TraMat(Absorption::PropMat<2> p) noexcept
      : a({p[0], p[1], p[1], p[0]}) {
    static_assert(N == 2);
  }
  constexpr TraMat(Absorption::PropMat<1> p) noexcept : a({p[0]}) {
    static_assert(N == 1);
  }

  constexpr TraMat() noexcept {
    for (size_t i = 0; i < N * N; i++) a[i] = 0;
  }

  friend std::ostream &operator<<(std::ostream &os, TraMat a) {
    for (size_t i = 0; i < TraMat::N; i++) {
      for (size_t j = 0; j < TraMat::N; j++) {
        if (j < N - 1)
          os << a(i, j) << ' ';
        else if (i < N - 1)
          os << a(i, j) << '\n';
        else
          os << a(i, j);
      }
    }
    return os;
  }

  constexpr double operator()(size_t i, size_t j) const noexcept {
    return a[i * N + j];
  }

  double &operator()(size_t i, size_t j) noexcept { return a[i * N + j]; }

  constexpr RadVec<N> operator()(TraMat t) const noexcept {
    if constexpr (N == 4)
      return RadVec<N>{t(0, 0), t(0, 1), t(0, 2), t(0, 3)};
    else if constexpr (N == 3)
      return RadVec<N>{t(0, 0), t(0, 1), t(0, 2)};
    else if constexpr (N == 2)
      return RadVec<N>{t(0, 0), t(0, 1)};
    else if constexpr (N == 1)
      return RadVec<N>{t(0, 0)};
  }

  constexpr TraMat operator/(double x) const noexcept {
    TraMat out;
    for (size_t i = 0; i < N * N; i++) out.a[i] = a[i] / x;
    return out;
  }

  friend constexpr TraMat operator-(TraMat T1, TraMat T2) noexcept {
    if constexpr (TraMat::N == 1) return {T1(0, 0) - T2(0, 0)};
    if constexpr (TraMat::N == 2)
      return {T1(0, 0) - T2(0, 0), T1(0, 1) - T2(0, 1), T1(1, 0) - T2(1, 0),
              T1(1, 1) - T2(1, 1)};
    if constexpr (TraMat::N == 3)
      return {T1(0, 0) - T2(0, 0), T1(0, 1) - T2(0, 1), T1(0, 2) - T2(0, 2),
              T1(1, 0) - T2(1, 0), T1(1, 1) - T2(1, 1), T1(1, 2) - T2(1, 2),
              T1(2, 0) - T2(2, 0), T1(2, 1) - T2(2, 1), T1(2, 2) - T2(2, 2)};
    if constexpr (TraMat::N == 4)
      return {T1(0, 0) - T2(0, 0), T1(0, 1) - T2(0, 1), T1(0, 2) - T2(0, 2),
              T1(0, 3) - T2(0, 3), T1(1, 0) - T2(1, 0), T1(1, 1) - T2(1, 1),
              T1(1, 2) - T2(1, 2), T1(1, 3) - T2(1, 3), T1(2, 0) - T2(2, 0),
              T1(2, 1) - T2(2, 1), T1(2, 2) - T2(2, 2), T1(2, 3) - T2(2, 3),
              T1(3, 0) - T2(3, 0), T1(3, 1) - T2(3, 1), T1(3, 2) - T2(3, 2),
              T1(3, 3) - T2(3, 3)};
  }

  friend constexpr TraMat operator*(TraMat T1, TraMat T2) noexcept {
    if constexpr (TraMat::N == 1) return {T1(0, 0) * T2(0, 0)};
    if constexpr (TraMat::N == 2)
      return {T1(0, 0) * T2(0, 0) + T1(0, 1) * T2(1, 0),
              T1(0, 0) * T2(0, 1) + T1(0, 1) * T2(1, 1),
              T1(1, 0) * T2(0, 0) + T1(1, 1) * T2(1, 0),
              T1(1, 0) * T2(0, 1) + T1(1, 1) * T2(1, 1)};
    if constexpr (TraMat::N == 3)
      return {T1(0, 0) * T2(0, 0) + T1(0, 1) * T2(1, 0) + T1(0, 2) * T2(2, 0),
              T1(0, 0) * T2(0, 1) + T1(0, 1) * T2(1, 1) + T1(0, 2) * T2(2, 1),
              T1(0, 0) * T2(0, 2) + T1(0, 1) * T2(1, 2) + T1(0, 2) * T2(2, 2),
              T1(1, 0) * T2(0, 0) + T1(1, 1) * T2(1, 0) + T1(1, 2) * T2(2, 0),
              T1(1, 0) * T2(0, 1) + T1(1, 1) * T2(1, 1) + T1(1, 2) * T2(2, 1),
              T1(1, 0) * T2(0, 2) + T1(1, 1) * T2(1, 2) + T1(1, 2) * T2(2, 2),
              T1(2, 0) * T2(0, 0) + T1(2, 1) * T2(1, 0) + T1(2, 2) * T2(2, 0),
              T1(2, 0) * T2(0, 1) + T1(2, 1) * T2(1, 1) + T1(2, 2) * T2(2, 1),
              T1(2, 0) * T2(0, 2) + T1(2, 1) * T2(1, 2) + T1(2, 2) * T2(2, 2)};
    if constexpr (TraMat::N == 4)
      return {T1(0, 0) * T2(0, 0) + T1(0, 1) * T2(1, 0) + T1(0, 2) * T2(2, 0) +
                  T1(0, 3) * T2(3, 0),
              T1(0, 0) * T2(0, 1) + T1(0, 1) * T2(1, 1) + T1(0, 2) * T2(2, 1) +
                  T1(0, 3) * T2(3, 1),
              T1(0, 0) * T2(0, 2) + T1(0, 1) * T2(1, 2) + T1(0, 2) * T2(2, 2) +
                  T1(0, 3) * T2(3, 2),
              T1(0, 0) * T2(0, 3) + T1(0, 1) * T2(1, 3) + T1(0, 2) * T2(2, 3) +
                  T1(0, 3) * T2(3, 3),
              T1(1, 0) * T2(0, 0) + T1(1, 1) * T2(1, 0) + T1(1, 2) * T2(2, 0) +
                  T1(1, 3) * T2(3, 0),
              T1(1, 0) * T2(0, 1) + T1(1, 1) * T2(1, 1) + T1(1, 2) * T2(2, 1) +
                  T1(1, 3) * T2(3, 1),
              T1(1, 0) * T2(0, 2) + T1(1, 1) * T2(1, 2) + T1(1, 2) * T2(2, 2) +
                  T1(1, 3) * T2(3, 2),
              T1(1, 0) * T2(0, 3) + T1(1, 1) * T2(1, 3) + T1(1, 2) * T2(2, 3) +
                  T1(1, 3) * T2(3, 3),
              T1(2, 0) * T2(0, 0) + T1(2, 1) * T2(1, 0) + T1(2, 2) * T2(2, 0) +
                  T1(2, 3) * T2(3, 0),
              T1(2, 0) * T2(0, 1) + T1(2, 1) * T2(1, 1) + T1(2, 2) * T2(2, 1) +
                  T1(2, 3) * T2(3, 1),
              T1(2, 0) * T2(0, 2) + T1(2, 1) * T2(1, 2) + T1(2, 2) * T2(2, 2) +
                  T1(2, 3) * T2(3, 2),
              T1(2, 0) * T2(0, 3) + T1(2, 1) * T2(1, 3) + T1(2, 2) * T2(2, 3) +
                  T1(2, 3) * T2(3, 3),
              T1(3, 0) * T2(0, 0) + T1(3, 1) * T2(1, 0) + T1(3, 2) * T2(2, 0) +
                  T1(3, 3) * T2(3, 0),
              T1(3, 0) * T2(0, 1) + T1(3, 1) * T2(1, 1) + T1(3, 2) * T2(2, 1) +
                  T1(3, 3) * T2(3, 1),
              T1(3, 0) * T2(0, 2) + T1(3, 1) * T2(1, 2) + T1(3, 2) * T2(2, 2) +
                  T1(3, 3) * T2(3, 2),
              T1(3, 0) * T2(0, 3) + T1(3, 1) * T2(1, 3) + T1(3, 2) * T2(2, 3) +
                  T1(3, 3) * T2(3, 3)};
  }

  static constexpr TraMat Identity() {
    if constexpr (TraMat::N == 4) {
      return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    } else if constexpr (TraMat::N == 3) {
      return {1, 0, 0, 0, 1, 0, 0, 0, 1};
    } else if constexpr (TraMat::N == 2) {
      return {1, 0, 0, 1};
    } else if constexpr (TraMat::N == 1) {
      return {1};
    }
  }
};  // TraMat

constexpr RadVec<1> operator*(const TraMat<1> &T, const RadVec<1> &R) {
  return RadVec<1>{R[0] * T(0, 0)};
}

constexpr RadVec<2> operator*(const TraMat<2> &T, const RadVec<2> &R) {
  return RadVec<2>{R[0] * T(0, 0) + R[1] * T(0, 1),
                   R[0] * T(1, 0) + R[1] * T(1, 1)};
}

constexpr RadVec<3> operator*(const TraMat<3> &T, const RadVec<3> &R) {
  return RadVec<3>{R[0] * T(0, 0) + R[1] * T(0, 1) + R[2] * T(0, 2),
                   R[0] * T(1, 0) + R[1] * T(1, 1) + R[2] * T(1, 2),
                   R[0] * T(2, 0) + R[1] * T(2, 1) + R[2] * T(2, 2)};
}

constexpr RadVec<4> operator*(const TraMat<4> &T, const RadVec<4> &R) {
  return RadVec<4>{
      R[0] * T(0, 0) + R[1] * T(0, 1) + R[2] * T(0, 2) + R[3] * T(0, 3),
      R[0] * T(1, 0) + R[1] * T(1, 1) + R[2] * T(1, 2) + R[3] * T(1, 3),
      R[0] * T(2, 0) + R[1] * T(2, 1) + R[2] * T(2, 2) + R[3] * T(2, 3),
      R[0] * T(3, 0) + R[1] * T(3, 1) + R[2] * T(3, 2) + R[3] * T(3, 3)};
}

ENUMCLASS(PolarizationType, unsigned char, I, Q, U, V, IpQ, ImQ, IpU, ImU, IpV,
          ImV)

template <size_t N>
struct Polarization {
  PolarizationType pol;
  TraMat<N> rotation;

  constexpr Polarization(
      PolarizationType p,
      [[maybe_unused]] Angle<AngleType::deg> circular_rotation = 0) noexcept
      : pol(p) {
    using Conversion::cosd;
    using Conversion::sind;
    if constexpr (N == 4) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = cosd(2 * circular_rotation);
      rotation(1, 2) = -sind(2 * circular_rotation);
      rotation(2, 1) = sind(2 * circular_rotation);
      rotation(3, 3) = 1;
    } else if constexpr (N == 3) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = cosd(2 * circular_rotation);
      rotation(1, 2) = -sind(2 * circular_rotation);
      rotation(2, 1) = sind(2 * circular_rotation);
    } else if constexpr (N == 2) {
      rotation(0, 0) = 1;
      rotation(1, 1) = cosd(2 * circular_rotation);  // Destructive...
    } else if constexpr (N == 2) {
      rotation(0, 0) = 1;
    }
  }
};

/** Returns T (I - J) + J */
template <size_t N>
constexpr RadVec<N> update(const RadVec<N> &I, const TraMat<N> &T,
                           const RadVec<N> &J) noexcept {
  if constexpr (N == 1) return (I[0] - J[0]) * T(0, 0) + J[0];
  if constexpr (N == 2)
    return {(I[0] - J[0]) * T(0, 0) + (I[1] - J[1]) * T(0, 1) + J[0],
            (I[0] - J[0]) * T(1, 0) + (I[1] - J[1]) * T(1, 1) + J[1]};
  if constexpr (N == 3)
    return {(I[0] - J[0]) * T(0, 0) + (I[1] - J[1]) * T(0, 1) +
                (I[2] - J[2]) * T(0, 2) + J[0],
            (I[0] - J[0]) * T(1, 0) + (I[1] - J[1]) * T(1, 1) +
                (I[2] - J[2]) * T(1, 2) + J[1],
            (I[0] - J[0]) * T(2, 0) + (I[1] - J[1]) * T(2, 1) +
                (I[2] - J[2]) * T(2, 2) + J[2]};
  if constexpr (N == 4)
    return {(I[0] - J[0]) * T(0, 0) + (I[1] - J[1]) * T(0, 1) +
                (I[2] - J[2]) * T(0, 2) + (I[3] - J[3]) * T(0, 3) + J[0],
            (I[0] - J[0]) * T(1, 0) + (I[1] - J[1]) * T(1, 1) +
                (I[2] - J[2]) * T(1, 2) + (I[3] - J[3]) * T(1, 3) + J[1],
            (I[0] - J[0]) * T(2, 0) + (I[1] - J[1]) * T(2, 1) +
                (I[2] - J[2]) * T(2, 2) + (I[3] - J[3]) * T(2, 3) + J[2],
            (I[0] - J[0]) * T(3, 0) + (I[1] - J[1]) * T(3, 1) +
                (I[2] - J[2]) * T(3, 2) + (I[3] - J[3]) * T(3, 3) + J[3]};
}

/** Returns dT (I - J) + dJ - T dJ */
template <size_t N>
constexpr RadVec<N> dupdate(const RadVec<N> &I, const TraMat<N> &T,
                            const TraMat<N> &dT, const RadVec<N> &J,
                            const RadVec<N> &dJ) noexcept {
  if constexpr (N == 1)
    return {
        (I[0] - J[0]) * dT(0, 0) - T(0, 0) * dJ[0] + dJ[0],
    };
  if constexpr (N == 2)
    return {
        (I[0] - J[0]) * dT(0, 0) + (I[1] - J[1]) * dT(0, 1) - T(0, 0) * dJ[0] -
            T(0, 1) * dJ[1] + dJ[0],
        (I[0] - J[0]) * dT(1, 0) + (I[1] - J[1]) * dT(1, 1) - T(1, 0) * dJ[0] -
            T(1, 1) * dJ[1] + dJ[1],
    };
  if constexpr (N == 3)
    return {
        (I[0] - J[0]) * dT(0, 0) + (I[1] - J[1]) * dT(0, 1) +
            (I[2] - J[2]) * dT(0, 2) - T(0, 0) * dJ[0] - T(0, 1) * dJ[1] -
            T(0, 2) * dJ[2] + dJ[0],
        (I[0] - J[0]) * dT(1, 0) + (I[1] - J[1]) * dT(1, 1) +
            (I[2] - J[2]) * dT(1, 2) - T(1, 0) * dJ[0] - T(1, 1) * dJ[1] -
            T(1, 2) * dJ[2] + dJ[1],
        (I[0] - J[0]) * dT(2, 0) + (I[1] - J[1]) * dT(2, 1) +
            (I[2] - J[2]) * dT(2, 2) - T(2, 0) * dJ[0] - T(2, 1) * dJ[1] -
            T(2, 2) * dJ[2] + dJ[2],
    };
  if constexpr (N == 4)
    return {
        (I[0] - J[0]) * dT(0, 0) + (I[1] - J[1]) * dT(0, 1) +
            (I[2] - J[2]) * dT(0, 2) + (I[3] - J[3]) * dT(0, 3) -
            T(0, 0) * dJ[0] - T(0, 1) * dJ[1] - T(0, 2) * dJ[2] -
            T(0, 3) * dJ[3] + dJ[0],
        (I[0] - J[0]) * dT(1, 0) + (I[1] - J[1]) * dT(1, 1) +
            (I[2] - J[2]) * dT(1, 2) + (I[3] - J[3]) * dT(1, 3) -
            T(1, 0) * dJ[0] - T(1, 1) * dJ[1] - T(1, 2) * dJ[2] -
            T(1, 3) * dJ[3] + dJ[1],
        (I[0] - J[0]) * dT(2, 0) + (I[1] - J[1]) * dT(2, 1) +
            (I[2] - J[2]) * dT(2, 2) + (I[3] - J[3]) * dT(2, 3) -
            T(2, 0) * dJ[0] - T(2, 1) * dJ[1] - T(2, 2) * dJ[2] -
            T(2, 3) * dJ[3] + dJ[2],
        (I[0] - J[0]) * dT(3, 0) + (I[1] - J[1]) * dT(3, 1) +
            (I[2] - J[2]) * dT(3, 2) + (I[3] - J[3]) * dT(3, 3) -
            T(3, 0) * dJ[0] - T(3, 1) * dJ[1] - T(3, 2) * dJ[2] -
            T(3, 3) * dJ[3] + dJ[3],
    };
}

/** Planck's law
 */
SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
B(Temperature<TemperatureType::K> T, Frequency<FrequencyType::Freq> f) noexcept;

/** Inverse of Planck's law
 */
SpectralRadiance<PowerType::T, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
invB(SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                      FrequencyType::Freq>
         I,
     Frequency<FrequencyType::Freq> f) noexcept;

/** Derivative of inverse of Planck's law wrt intensity */
SpectralRadiance<PowerType::T, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dinvBdI(SpectralRadiance<PowerType::W, SphericalAngleType::Steradian,
                         AreaType::m2, FrequencyType::Freq>
            I,
        Frequency<FrequencyType::Freq> f) noexcept;

/** Planck's law temperature derivative
 */
SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dBdT(Temperature<TemperatureType::K> T,
     Frequency<FrequencyType::Freq> f) noexcept;

/** Planck's law frequency derivative
 */
SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dBdf(Temperature<TemperatureType::K> T,
     Frequency<FrequencyType::Freq> f) noexcept;

/** Returns K^-1 (A B + S) + B
 */
template <size_t N>
constexpr RadVec<N> source(
    const Absorption::PropMat<N> &K, const RadVec<N> &S, const RadVec<N> &A,
    const SpectralRadiance<PowerType::W, SphericalAngleType::Steradian,
                           AreaType::m2, FrequencyType::Freq> &B) noexcept {
  using Constant::pow2;
  using Constant::pow4;
  if constexpr (N == 1) {
    auto invdenom = 1 / K[0];
    if (invdenom not_eq 0) {
      return RadVec<1>{
          invdenom * (A[0] * B + S[0]) + B,
      };
    } else {
      return RadVec<1>{B};
    }
  } else if constexpr (N == 2) {
    auto invdenom = 1 / (pow2(K[0]) - pow2(K[1]));
    if (invdenom not_eq 0) {
      return RadVec<2>{
          invdenom * ((A[0] * B + S[0]) * K[0] - (A[1] * B + S[1]) * K[1]) + B,
          invdenom * (-(A[0] * B + S[0]) * K[1] + (A[1] * B + S[1]) * K[0]),
      };
    } else {
      return RadVec<2>{B, 0};
    }
  } else if constexpr (N == 3) {
    auto invdenom =
        1 / ((pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[3])) * K[0]);
    if (invdenom not_eq 0) {
      return RadVec<3>{
          invdenom * ((A[0] * B + S[0]) * (pow2(K[0]) + pow2(K[3])) -
                      (A[1] * B + S[1]) * (K[0] * K[1] + K[2] * K[3]) -
                      (A[2] * B + S[2]) * (K[0] * K[2] - K[1] * K[3])) +
              B,
          invdenom * (-(A[0] * B + S[0]) * (K[0] * K[1] - K[2] * K[3]) +
                      (A[1] * B + S[1]) * (pow2(K[0]) - pow2(K[2])) -
                      (A[2] * B + S[2]) * (K[0] * K[3] - K[1] * K[2])),
          invdenom * (-(A[0] * B + S[0]) * (K[0] * K[2] + K[1] * K[3]) +
                      (A[1] * B + S[1]) * (K[0] * K[3] + K[1] * K[2]) +
                      (A[2] * B + S[2]) * (pow2(K[0]) - pow2(K[1]))),
      };
    } else {
      return RadVec<3>{B, 0, 0};
    }
  } else if constexpr (N == 4) {
    auto invdenom =
        1 / (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
             pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[4]) +
             pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
             pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
             2 * K[1] * K[3] * K[4] * K[6] - pow2(K[2]) * pow2(K[5]) +
             2 * K[2] * K[3] * K[4] * K[5] - pow2(K[3]) * pow2(K[4]));
    if (invdenom not_eq 0) {
      return RadVec<4>{
          invdenom * ((A[0] * B + S[0]) *
                          (pow2(K[0]) + pow2(K[4]) + pow2(K[5]) + pow2(K[6])) *
                          K[0] -
                      (A[1] * B + S[1]) *
                          (pow2(K[0]) * K[1] + K[0] * K[2] * K[4] +
                           K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                           K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) -
                      (A[2] * B + S[2]) *
                          (pow2(K[0]) * K[2] - K[0] * K[1] * K[4] +
                           K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                           K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) -
                      (A[3] * B + S[3]) *
                          (pow2(K[0]) * K[3] - K[0] * K[1] * K[5] -
                           K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                           K[2] * K[4] * K[5] + K[3] * pow2(K[4]))) +
              B,
          invdenom *
              (-(A[0] * B + S[0]) * (pow2(K[0]) * K[1] - K[0] * K[2] * K[4] -
                                     K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                                     K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) +
               (A[1] * B + S[1]) *
                   (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) * K[0] -
               (A[2] * B + S[2]) * (pow2(K[0]) * K[4] - K[0] * K[1] * K[2] +
                                    K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                                    K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) -
               (A[3] * B + S[3]) * (pow2(K[0]) * K[5] - K[0] * K[1] * K[3] -
                                    K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                                    pow2(K[2]) * K[5] + K[2] * K[3] * K[4])),
          invdenom *
              (-(A[0] * B + S[0]) * (pow2(K[0]) * K[2] + K[0] * K[1] * K[4] -
                                     K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                                     K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) +
               (A[1] * B + S[1]) * (pow2(K[0]) * K[4] + K[0] * K[1] * K[2] -
                                    K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                                    K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) +
               (A[2] * B + S[2]) *
                   (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) * K[0] -
               (A[3] * B + S[3]) * (pow2(K[0]) * K[6] - K[0] * K[2] * K[3] +
                                    K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                                    K[1] * K[2] * K[5] - K[1] * K[3] * K[4])),
          invdenom *
              (-(A[0] * B + S[0]) * (pow2(K[0]) * K[3] + K[0] * K[1] * K[5] +
                                     K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                                     K[2] * K[4] * K[5] + K[3] * pow2(K[4])) +
               (A[1] * B + S[1]) * (pow2(K[0]) * K[5] + K[0] * K[1] * K[3] +
                                    K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                                    pow2(K[2]) * K[5] + K[2] * K[3] * K[4]) +
               (A[2] * B + S[2]) * (pow2(K[0]) * K[6] + K[0] * K[2] * K[3] -
                                    K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                                    K[1] * K[2] * K[5] - K[1] * K[3] * K[4]) +
               (A[3] * B + S[3]) *
                   (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[4])) * K[0]),
      };
    } else {
      return RadVec<4>{B, 0, 0, 0};
    }
  }
}

/** Returns K^-1((dA B + A dB + dS) - K^-1 dK J)
 *
 * where J is K^-1 (A B + S) [e.g., source(K, S, A, B)]
 */
template <size_t N>
constexpr RadVec<N> dsource(
    const Absorption::PropMat<N> &K, const Absorption::PropMat<N> &dK,
    const RadVec<N> &S, const RadVec<N> &dS, const RadVec<N> &A,
    const RadVec<N> &dA,
    const SpectralRadiance<PowerType::W, SphericalAngleType::Steradian,
                           AreaType::m2, FrequencyType::Freq> &B,
    const SpectralRadiance<PowerType::W, SphericalAngleType::Steradian,
                           AreaType::m2, FrequencyType::Freq> &dB) {
  using Constant::pow2;
  using Constant::pow3;
  using Constant::pow4;

  if constexpr (N == 1) {
    auto invdenom = 1 / K[0];
    if (invdenom not_eq 0) {
      auto ddenom = dK[0];
      return RadVec<1>{invdenom * (A[0] * dB + B * dA[0] + dS[0]) -
                       (A[0] * B + S[0]) * ddenom * pow2(invdenom) + dB};
    } else {
      return RadVec<1>{dB};
    }
  } else if constexpr (N == 2) {
    auto invdenom = 1 / (pow2(K[0]) - pow2(K[1]));
    if (invdenom not_eq 0) {
      auto ddenom = 2 * K[0] * dK[0] - 2 * K[1] * dK[1];
      return RadVec<2>{
          invdenom * ((A[0] * B + S[0]) * dK[0] - (A[1] * B + S[1]) * dK[1] +
                      (A[0] * dB + B * dA[0] + dS[0]) * K[0] -
                      (A[1] * dB + B * dA[1] + dS[1]) * K[1]) -
              ((A[0] * B + S[0]) * K[0] - (A[1] * B + S[1]) * K[1]) * ddenom *
                  pow2(invdenom) +
              dB,
          -invdenom * ((A[0] * B + S[0]) * dK[1] - (A[1] * B + S[1]) * dK[0] +
                       (A[0] * dB + B * dA[0] + dS[0]) * K[1] -
                       (A[1] * dB + B * dA[1] + dS[1]) * K[0]) +
              ((A[0] * B + S[0]) * K[1] - (A[1] * B + S[1]) * K[0]) * ddenom *
                  pow2(invdenom)};
    } else {
      return RadVec<2>{dB, 0};
    }
  } else if constexpr (N == 3) {
    auto invdenom =
        1 / ((pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[3])) * K[0]);
    if (invdenom not_eq 0) {
      auto ddenom =
          2 * (K[0] * dK[0] - K[1] * dK[1] - K[2] * dK[2] + K[3] * dK[3]) *
              K[0] +
          (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[3])) * dK[0];
      return RadVec<3>{
          -invdenom * (-2 * (A[0] * B + S[0]) * (K[0] * dK[0] + K[3] * dK[3]) +
                       (A[1] * B + S[1]) * (K[0] * dK[1] + K[1] * dK[0] +
                                            K[2] * dK[3] + K[3] * dK[2]) +
                       (A[2] * B + S[2]) * (K[0] * dK[2] - K[1] * dK[3] +
                                            K[2] * dK[0] - K[3] * dK[1]) +
                       (K[0] * K[1] + K[2] * K[3]) *
                           (A[1] * dB + B * dA[1] + dS[1]) +
                       (K[0] * K[2] - K[1] * K[3]) *
                           (A[2] * dB + B * dA[2] + dS[2]) -
                       (pow2(K[0]) + pow2(K[3])) *
                           (A[0] * dB + B * dA[0] + dS[0])) +
              (-(A[0] * B + S[0]) * (pow2(K[0]) + pow2(K[3])) +
               (A[1] * B + S[1]) * (K[0] * K[1] + K[2] * K[3]) +
               (A[2] * B + S[2]) * (K[0] * K[2] - K[1] * K[3])) *
                  ddenom * pow2(invdenom) +
              dB,
          -invdenom * ((A[0] * B + S[0]) * (K[0] * dK[1] + K[1] * dK[0] -
                                            K[2] * dK[3] - K[3] * dK[2]) -
                       2 * (A[1] * B + S[1]) * (K[0] * dK[0] - K[2] * dK[2]) +
                       (A[2] * B + S[2]) * (K[0] * dK[3] - K[1] * dK[2] -
                                            K[2] * dK[1] + K[3] * dK[0]) +
                       (K[0] * K[1] - K[2] * K[3]) *
                           (A[0] * dB + B * dA[0] + dS[0]) +
                       (K[0] * K[3] - K[1] * K[2]) *
                           (A[2] * dB + B * dA[2] + dS[2]) -
                       (pow2(K[0]) - pow2(K[2])) *
                           (A[1] * dB + B * dA[1] + dS[1])) +
              ((A[0] * B + S[0]) * (K[0] * K[1] - K[2] * K[3]) -
               (A[1] * B + S[1]) * (pow2(K[0]) - pow2(K[2])) +
               (A[2] * B + S[2]) * (K[0] * K[3] - K[1] * K[2])) *
                  ddenom * pow2(invdenom),
          invdenom * (-(A[0] * B + S[0]) * (K[0] * dK[2] + K[1] * dK[3] +
                                            K[2] * dK[0] + K[3] * dK[1]) +
                      (A[1] * B + S[1]) * (K[0] * dK[3] + K[1] * dK[2] +
                                           K[2] * dK[1] + K[3] * dK[0]) +
                      2 * (A[2] * B + S[2]) * (K[0] * dK[0] - K[1] * dK[1]) -
                      (K[0] * K[2] + K[1] * K[3]) *
                          (A[0] * dB + B * dA[0] + dS[0]) +
                      (K[0] * K[3] + K[1] * K[2]) *
                          (A[1] * dB + B * dA[1] + dS[1]) +
                      (pow2(K[0]) - pow2(K[1])) *
                          (A[2] * dB + B * dA[2] + dS[2])) -
              (-(A[0] * B + S[0]) * (K[0] * K[2] + K[1] * K[3]) +
               (A[1] * B + S[1]) * (K[0] * K[3] + K[1] * K[2]) +
               (A[2] * B + S[2]) * (pow2(K[0]) - pow2(K[1]))) *
                  ddenom * pow2(invdenom)};
    } else {
      return RadVec<3>{dB, 0, 0};
    }
  } else if constexpr (N == 4) {
    auto invdenom =
        1 / (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
             pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[4]) +
             pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
             pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
             2 * K[1] * K[3] * K[4] * K[6] - pow2(K[2]) * pow2(K[5]) +
             2 * K[2] * K[3] * K[4] * K[5] - pow2(K[3]) * pow2(K[4]));
    if (invdenom not_eq 0) {
      auto ddenom =
          4 * pow3(K[0]) * dK[0] - 2 * pow2(K[0]) * K[1] * dK[1] -
          2 * pow2(K[0]) * K[2] * dK[2] - 2 * pow2(K[0]) * K[3] * dK[3] +
          2 * pow2(K[0]) * K[4] * dK[4] + 2 * pow2(K[0]) * K[5] * dK[5] +
          2 * pow2(K[0]) * K[6] * dK[6] - 2 * K[0] * pow2(K[1]) * dK[0] -
          2 * K[0] * pow2(K[2]) * dK[0] - 2 * K[0] * pow2(K[3]) * dK[0] +
          2 * K[0] * pow2(K[4]) * dK[0] + 2 * K[0] * pow2(K[5]) * dK[0] +
          2 * K[0] * pow2(K[6]) * dK[0] - 2 * pow2(K[1]) * K[6] * dK[6] +
          2 * K[1] * K[2] * K[5] * dK[6] + 2 * K[1] * K[2] * K[6] * dK[5] -
          2 * K[1] * K[3] * K[4] * dK[6] - 2 * K[1] * K[3] * K[6] * dK[4] -
          2 * K[1] * K[4] * K[6] * dK[3] + 2 * K[1] * K[5] * K[6] * dK[2] -
          2 * K[1] * pow2(K[6]) * dK[1] - 2 * pow2(K[2]) * K[5] * dK[5] +
          2 * K[2] * K[3] * K[4] * dK[5] + 2 * K[2] * K[3] * K[5] * dK[4] +
          2 * K[2] * K[4] * K[5] * dK[3] - 2 * K[2] * pow2(K[5]) * dK[2] +
          2 * K[2] * K[5] * K[6] * dK[1] - 2 * pow2(K[3]) * K[4] * dK[4] -
          2 * K[3] * pow2(K[4]) * dK[3] + 2 * K[3] * K[4] * K[5] * dK[2] -
          2 * K[3] * K[4] * K[6] * dK[1];
      return RadVec<4>{
          invdenom * (2 * (A[0] * B + S[0]) *
                          (K[0] * dK[0] + K[4] * dK[4] + K[5] * dK[5] +
                           K[6] * dK[6]) *
                          K[0] +
                      (A[0] * B + S[0]) *
                          (pow2(K[0]) + pow2(K[4]) + pow2(K[5]) + pow2(K[6])) *
                          dK[0] -
                      (A[1] * B + S[1]) *
                          (pow2(K[0]) * dK[1] + 2 * K[0] * K[1] * dK[0] +
                           K[0] * K[2] * dK[4] + K[0] * K[3] * dK[5] +
                           K[0] * K[4] * dK[2] + K[0] * K[5] * dK[3] +
                           2 * K[1] * K[6] * dK[6] + K[2] * K[4] * dK[0] -
                           K[2] * K[5] * dK[6] - K[2] * K[6] * dK[5] +
                           K[3] * K[4] * dK[6] + K[3] * K[5] * dK[0] +
                           K[3] * K[6] * dK[4] + K[4] * K[6] * dK[3] -
                           K[5] * K[6] * dK[2] + pow2(K[6]) * dK[1]) +
                      (A[2] * B + S[2]) *
                          (-pow2(K[0]) * dK[2] + K[0] * K[1] * dK[4] -
                           2 * K[0] * K[2] * dK[0] - K[0] * K[3] * dK[6] +
                           K[0] * K[4] * dK[1] - K[0] * K[6] * dK[3] +
                           K[1] * K[4] * dK[0] + K[1] * K[5] * dK[6] +
                           K[1] * K[6] * dK[5] - 2 * K[2] * K[5] * dK[5] +
                           K[3] * K[4] * dK[5] + K[3] * K[5] * dK[4] -
                           K[3] * K[6] * dK[0] + K[4] * K[5] * dK[3] -
                           pow2(K[5]) * dK[2] + K[5] * K[6] * dK[1]) +
                      (A[3] * B + S[3]) *
                          (-pow2(K[0]) * dK[3] + K[0] * K[1] * dK[5] +
                           K[0] * K[2] * dK[6] - 2 * K[0] * K[3] * dK[0] +
                           K[0] * K[5] * dK[1] + K[0] * K[6] * dK[2] -
                           K[1] * K[4] * dK[6] + K[1] * K[5] * dK[0] -
                           K[1] * K[6] * dK[4] + K[2] * K[4] * dK[5] +
                           K[2] * K[5] * dK[4] + K[2] * K[6] * dK[0] -
                           2 * K[3] * K[4] * dK[4] - pow2(K[4]) * dK[3] +
                           K[4] * K[5] * dK[2] - K[4] * K[6] * dK[1]) +
                      (A[0] * dB + B * dA[0] + dS[0]) *
                          (pow2(K[0]) + pow2(K[4]) + pow2(K[5]) + pow2(K[6])) *
                          K[0] -
                      (A[1] * dB + B * dA[1] + dS[1]) *
                          (pow2(K[0]) * K[1] + K[0] * K[2] * K[4] +
                           K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                           K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) -
                      (A[2] * dB + B * dA[2] + dS[2]) *
                          (pow2(K[0]) * K[2] - K[0] * K[1] * K[4] +
                           K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                           K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) -
                      (A[3] * dB + B * dA[3] + dS[3]) *
                          (pow2(K[0]) * K[3] - K[0] * K[1] * K[5] -
                           K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                           K[2] * K[4] * K[5] + K[3] * pow2(K[4]))) +
              (-(A[0] * B + S[0]) *
                   (pow2(K[0]) + pow2(K[4]) + pow2(K[5]) + pow2(K[6])) * K[0] +
               (A[1] * B + S[1]) * (pow2(K[0]) * K[1] + K[0] * K[2] * K[4] +
                                    K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                                    K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) +
               (A[2] * B + S[2]) * (pow2(K[0]) * K[2] - K[0] * K[1] * K[4] +
                                    K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                                    K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) +
               (A[3] * B + S[3]) * (pow2(K[0]) * K[3] - K[0] * K[1] * K[5] -
                                    K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                                    K[2] * K[4] * K[5] + K[3] * pow2(K[4]))) *
                  ddenom * pow2(invdenom) +
              dB,
          -invdenom * (-(A[0] * B + S[0]) *
                           (-pow2(K[0]) * dK[1] - 2 * K[0] * K[1] * dK[0] +
                            K[0] * K[2] * dK[4] + K[0] * K[3] * dK[5] +
                            K[0] * K[4] * dK[2] + K[0] * K[5] * dK[3] -
                            2 * K[1] * K[6] * dK[6] + K[2] * K[4] * dK[0] +
                            K[2] * K[5] * dK[6] + K[2] * K[6] * dK[5] -
                            K[3] * K[4] * dK[6] + K[3] * K[5] * dK[0] -
                            K[3] * K[6] * dK[4] - K[4] * K[6] * dK[3] +
                            K[5] * K[6] * dK[2] - pow2(K[6]) * dK[1]) -
                       2 * (A[1] * B + S[1]) *
                           (K[0] * dK[0] - K[2] * dK[2] - K[3] * dK[3] +
                            K[6] * dK[6]) *
                           K[0] -
                       (A[1] * B + S[1]) *
                           (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) *
                           dK[0] +
                       (A[2] * B + S[2]) *
                           (pow2(K[0]) * dK[4] - K[0] * K[1] * dK[2] -
                            K[0] * K[2] * dK[1] + 2 * K[0] * K[4] * dK[0] +
                            K[0] * K[5] * dK[6] + K[0] * K[6] * dK[5] -
                            K[1] * K[2] * dK[0] - K[1] * K[3] * dK[6] -
                            K[1] * K[6] * dK[3] + K[2] * K[3] * dK[5] +
                            K[2] * K[5] * dK[3] - pow2(K[3]) * dK[4] -
                            2 * K[3] * K[4] * dK[3] + K[3] * K[5] * dK[2] -
                            K[3] * K[6] * dK[1] + K[5] * K[6] * dK[0]) +
                       (A[3] * B + S[3]) *
                           (pow2(K[0]) * dK[5] - K[0] * K[1] * dK[3] -
                            K[0] * K[3] * dK[1] - K[0] * K[4] * dK[6] +
                            2 * K[0] * K[5] * dK[0] - K[0] * K[6] * dK[4] +
                            K[1] * K[2] * dK[6] - K[1] * K[3] * dK[0] +
                            K[1] * K[6] * dK[2] - pow2(K[2]) * dK[5] +
                            K[2] * K[3] * dK[4] + K[2] * K[4] * dK[3] -
                            2 * K[2] * K[5] * dK[2] + K[2] * K[6] * dK[1] +
                            K[3] * K[4] * dK[2] - K[4] * K[6] * dK[0]) +
                       (A[0] * dB + B * dA[0] + dS[0]) *
                           (pow2(K[0]) * K[1] - K[0] * K[2] * K[4] -
                            K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                            K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) -
                       (A[1] * dB + B * dA[1] + dS[1]) *
                           (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) *
                           K[0] +
                       (A[2] * dB + B * dA[2] + dS[2]) *
                           (pow2(K[0]) * K[4] - K[0] * K[1] * K[2] +
                            K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                            K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) +
                       (A[3] * dB + B * dA[3] + dS[3]) *
                           (pow2(K[0]) * K[5] - K[0] * K[1] * K[3] -
                            K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                            pow2(K[2]) * K[5] + K[2] * K[3] * K[4])) +
              ((A[0] * B + S[0]) * (pow2(K[0]) * K[1] - K[0] * K[2] * K[4] -
                                    K[0] * K[3] * K[5] + K[1] * pow2(K[6]) -
                                    K[2] * K[5] * K[6] + K[3] * K[4] * K[6]) -
               (A[1] * B + S[1]) *
                   (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) * K[0] +
               (A[2] * B + S[2]) * (pow2(K[0]) * K[4] - K[0] * K[1] * K[2] +
                                    K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                                    K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) +
               (A[3] * B + S[3]) * (pow2(K[0]) * K[5] - K[0] * K[1] * K[3] -
                                    K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                                    pow2(K[2]) * K[5] + K[2] * K[3] * K[4])) *
                  ddenom * pow2(invdenom),
          invdenom * ((A[0] * B + S[0]) *
                          (-pow2(K[0]) * dK[2] - K[0] * K[1] * dK[4] -
                           2 * K[0] * K[2] * dK[0] + K[0] * K[3] * dK[6] -
                           K[0] * K[4] * dK[1] + K[0] * K[6] * dK[3] -
                           K[1] * K[4] * dK[0] + K[1] * K[5] * dK[6] +
                           K[1] * K[6] * dK[5] - 2 * K[2] * K[5] * dK[5] +
                           K[3] * K[4] * dK[5] + K[3] * K[5] * dK[4] +
                           K[3] * K[6] * dK[0] + K[4] * K[5] * dK[3] -
                           pow2(K[5]) * dK[2] + K[5] * K[6] * dK[1]) +
                      (A[1] * B + S[1]) *
                          (pow2(K[0]) * dK[4] + K[0] * K[1] * dK[2] +
                           K[0] * K[2] * dK[1] + 2 * K[0] * K[4] * dK[0] -
                           K[0] * K[5] * dK[6] - K[0] * K[6] * dK[5] +
                           K[1] * K[2] * dK[0] - K[1] * K[3] * dK[6] -
                           K[1] * K[6] * dK[3] + K[2] * K[3] * dK[5] +
                           K[2] * K[5] * dK[3] - pow2(K[3]) * dK[4] -
                           2 * K[3] * K[4] * dK[3] + K[3] * K[5] * dK[2] -
                           K[3] * K[6] * dK[1] - K[5] * K[6] * dK[0]) +
                      2 * (A[2] * B + S[2]) *
                          (K[0] * dK[0] - K[1] * dK[1] - K[3] * dK[3] +
                           K[5] * dK[5]) *
                          K[0] +
                      (A[2] * B + S[2]) *
                          (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) *
                          dK[0] -
                      (A[3] * B + S[3]) *
                          (pow2(K[0]) * dK[6] - K[0] * K[2] * dK[3] -
                           K[0] * K[3] * dK[2] + K[0] * K[4] * dK[5] +
                           K[0] * K[5] * dK[4] + 2 * K[0] * K[6] * dK[0] -
                           pow2(K[1]) * dK[6] + K[1] * K[2] * dK[5] -
                           K[1] * K[3] * dK[4] - K[1] * K[4] * dK[3] +
                           K[1] * K[5] * dK[2] - 2 * K[1] * K[6] * dK[1] -
                           K[2] * K[3] * dK[0] + K[2] * K[5] * dK[1] -
                           K[3] * K[4] * dK[1] + K[4] * K[5] * dK[0]) -
                      (A[0] * dB + B * dA[0] + dS[0]) *
                          (pow2(K[0]) * K[2] + K[0] * K[1] * K[4] -
                           K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                           K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) +
                      (A[1] * dB + B * dA[1] + dS[1]) *
                          (pow2(K[0]) * K[4] + K[0] * K[1] * K[2] -
                           K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                           K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) +
                      (A[2] * dB + B * dA[2] + dS[2]) *
                          (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) *
                          K[0] -
                      (A[3] * dB + B * dA[3] + dS[3]) *
                          (pow2(K[0]) * K[6] - K[0] * K[2] * K[3] +
                           K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                           K[1] * K[2] * K[5] - K[1] * K[3] * K[4])) +
              ((A[0] * B + S[0]) * (pow2(K[0]) * K[2] + K[0] * K[1] * K[4] -
                                    K[0] * K[3] * K[6] - K[1] * K[5] * K[6] +
                                    K[2] * pow2(K[5]) - K[3] * K[4] * K[5]) -
               (A[1] * B + S[1]) * (pow2(K[0]) * K[4] + K[0] * K[1] * K[2] -
                                    K[0] * K[5] * K[6] - K[1] * K[3] * K[6] +
                                    K[2] * K[3] * K[5] - pow2(K[3]) * K[4]) -
               (A[2] * B + S[2]) *
                   (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) * K[0] +
               (A[3] * B + S[3]) * (pow2(K[0]) * K[6] - K[0] * K[2] * K[3] +
                                    K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                                    K[1] * K[2] * K[5] - K[1] * K[3] * K[4])) *
                  ddenom * pow2(invdenom),
          invdenom * (-(A[0] * B + S[0]) *
                          (pow2(K[0]) * dK[3] + K[0] * K[1] * dK[5] +
                           K[0] * K[2] * dK[6] + 2 * K[0] * K[3] * dK[0] +
                           K[0] * K[5] * dK[1] + K[0] * K[6] * dK[2] +
                           K[1] * K[4] * dK[6] + K[1] * K[5] * dK[0] +
                           K[1] * K[6] * dK[4] - K[2] * K[4] * dK[5] -
                           K[2] * K[5] * dK[4] + K[2] * K[6] * dK[0] +
                           2 * K[3] * K[4] * dK[4] + pow2(K[4]) * dK[3] -
                           K[4] * K[5] * dK[2] + K[4] * K[6] * dK[1]) +
                      (A[1] * B + S[1]) *
                          (pow2(K[0]) * dK[5] + K[0] * K[1] * dK[3] +
                           K[0] * K[3] * dK[1] + K[0] * K[4] * dK[6] +
                           2 * K[0] * K[5] * dK[0] + K[0] * K[6] * dK[4] +
                           K[1] * K[2] * dK[6] + K[1] * K[3] * dK[0] +
                           K[1] * K[6] * dK[2] - pow2(K[2]) * dK[5] +
                           K[2] * K[3] * dK[4] + K[2] * K[4] * dK[3] -
                           2 * K[2] * K[5] * dK[2] + K[2] * K[6] * dK[1] +
                           K[3] * K[4] * dK[2] + K[4] * K[6] * dK[0]) +
                      (A[2] * B + S[2]) *
                          (pow2(K[0]) * dK[6] + K[0] * K[2] * dK[3] +
                           K[0] * K[3] * dK[2] - K[0] * K[4] * dK[5] -
                           K[0] * K[5] * dK[4] + 2 * K[0] * K[6] * dK[0] -
                           pow2(K[1]) * dK[6] + K[1] * K[2] * dK[5] -
                           K[1] * K[3] * dK[4] - K[1] * K[4] * dK[3] +
                           K[1] * K[5] * dK[2] - 2 * K[1] * K[6] * dK[1] +
                           K[2] * K[3] * dK[0] + K[2] * K[5] * dK[1] -
                           K[3] * K[4] * dK[1] - K[4] * K[5] * dK[0]) +
                      2 * (A[3] * B + S[3]) *
                          (K[0] * dK[0] - K[1] * dK[1] - K[2] * dK[2] +
                           K[4] * dK[4]) *
                          K[0] +
                      (A[3] * B + S[3]) *
                          (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[4])) *
                          dK[0] -
                      (A[0] * dB + B * dA[0] + dS[0]) *
                          (pow2(K[0]) * K[3] + K[0] * K[1] * K[5] +
                           K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                           K[2] * K[4] * K[5] + K[3] * pow2(K[4])) +
                      (A[1] * dB + B * dA[1] + dS[1]) *
                          (pow2(K[0]) * K[5] + K[0] * K[1] * K[3] +
                           K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                           pow2(K[2]) * K[5] + K[2] * K[3] * K[4]) +
                      (A[2] * dB + B * dA[2] + dS[2]) *
                          (pow2(K[0]) * K[6] + K[0] * K[2] * K[3] -
                           K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                           K[1] * K[2] * K[5] - K[1] * K[3] * K[4]) +
                      (A[3] * dB + B * dA[3] + dS[3]) *
                          (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[4])) *
                          K[0]) -
              (-(A[0] * B + S[0]) * (pow2(K[0]) * K[3] + K[0] * K[1] * K[5] +
                                     K[0] * K[2] * K[6] + K[1] * K[4] * K[6] -
                                     K[2] * K[4] * K[5] + K[3] * pow2(K[4])) +
               (A[1] * B + S[1]) * (pow2(K[0]) * K[5] + K[0] * K[1] * K[3] +
                                    K[0] * K[4] * K[6] + K[1] * K[2] * K[6] -
                                    pow2(K[2]) * K[5] + K[2] * K[3] * K[4]) +
               (A[2] * B + S[2]) * (pow2(K[0]) * K[6] + K[0] * K[2] * K[3] -
                                    K[0] * K[4] * K[5] - pow2(K[1]) * K[6] +
                                    K[1] * K[2] * K[5] - K[1] * K[3] * K[4]) +
               (A[3] * B + S[3]) *
                   (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[4])) * K[0]) *
                  ddenom * pow2(invdenom)};
    } else {
      return RadVec<4>{dB, 0, 0, 0};
    }
  }
}

template <size_t N>
constexpr TraMat<N> linear_transmat(
    const Absorption::PropMat<N> &K1, const Absorption::PropMat<N> &K2,
    const Distance<DistanceType::meter> r) noexcept {
  using Constant::inv_sqrt_2;
  using Constant::pow2;
  using Constant::pow3;
  using std::cos;
  using std::cosh;
  using std::exp;
  using std::sin;
  using std::sinh;
  using std::sqrt;
  if constexpr (N == 4) {
    const double a = -r * (K1[0] + K2[0]) * 0.5, b = -r * (K1[1] + K2[1]) * 0.5,
                 c = -r * (K1[2] + K2[2]) * 0.5, d = -r * (K1[3] + K2[3]) * 0.5,
                 u = -r * (K1[N] + K2[N]) * 0.5, v = -r * (K1[5] + K2[5]) * 0.5,
                 w = -r * (K1[6] + K2[6]) * 0.5;
    const double b2 = b * b, c2 = c * c, d2 = d * d, u2 = u * u, v2 = v * v,
                 w2 = w * w;
    const double exp_a = exp(a);

    const double tmp =
        w2 * w2 + 2 * (b2 * (b2 * 0.5 + c2 + d2 - u2 - v2 + w2) +
                       c2 * (c2 * 0.5 + d2 - u2 + v2 - w2) +
                       d2 * (d2 * 0.5 + u2 - v2 - w2) +
                       u2 * (u2 * 0.5 + v2 + w2) + v2 * (v2 * 0.5 + w2) +
                       4 * (b * d * u * w - b * c * v * w - c * d * u * v));

    const Complex Const1 = sqrt(Complex{tmp});
    const double Const2 = b2 + c2 + d2 - u2 - v2 - w2;

    const Complex x = sqrt(Const2 + Const1) * inv_sqrt_2;
    const Complex y = sqrt(Const2 - Const1) * Complex{0, inv_sqrt_2};
    const Complex x2 = x * x;
    const Complex y2 = y * y;
    const Complex cy = cos(y);
    const Complex sy = sin(y);
    const Complex cx = cosh(x);
    const Complex sx = sinh(x);

    const bool x_zero = std::abs(x) < 1e-4;
    const bool y_zero = std::abs(y) < 1e-4;
    const bool both_zero = y_zero and x_zero;
    const bool either_zero = y_zero or x_zero;

    /* Using:
     *    lim x→0 [({cosh(x),cos(x)} - 1) / x^2] → 1/2
     *    lim x→0 [{sinh(x),sin(x)} / x]  → 1
     *    inv_x2 := 1 for x == 0,
     *    -i sin(ix) → sinh(x)
     *    cos(ix) → cosh(x)
     *    C0, C1, C2 ∝ [1/x^2]
     */
    const Complex ix = x_zero ? 0.0 : 1.0 / x;
    const Complex iy = y_zero ? 0.0 : 1.0 / y;
    const Complex inv_x2y2 =
        both_zero
            ? 1.0
            : 1.0 / (x2 + y2);  // The first "1.0" is the trick for above limits

    const double C0 =
        either_zero ? 1.0 : ((cy * x2 + cx * y2) * inv_x2y2).real();
    const double C1 =
        either_zero ? 1.0 : ((sy * x2 * iy + sx * y2 * ix) * inv_x2y2).real();
    const double C2 = both_zero ? 0.5 : ((cx - cy) * inv_x2y2).real();
    const double C3 =
        both_zero ? 1.0 / 6.0
                  : ((x_zero ? 1.0 - sy * iy
                             : y_zero ? sx * ix - 1.0 : sx * ix - sy * iy) *
                     inv_x2y2)
                        .real();

    return TraMat<N>{exp_a * (C0 + C2 * (b2 + c2 + d2)),
                     exp_a * (C1 * b + C2 * (-c * u - d * v) +
                              C3 * (b * (b2 + c2 + d2) - u * (b * u - d * w) -
                                    v * (b * v + c * w))),
                     exp_a * (C1 * c + C2 * (b * u - d * w) +
                              C3 * (c * (b2 + c2 + d2) - u * (c * u + d * v) -
                                    w * (b * v + c * w))),
                     exp_a * (C1 * d + C2 * (b * v + c * w) +
                              C3 * (d * (b2 + c2 + d2) - v * (c * u + d * v) +
                                    w * (b * u - d * w))),

                     exp_a * (C1 * b + C2 * (c * u + d * v) +
                              C3 * (-b * (-b2 + u2 + v2) + c * (b * c - v * w) +
                                    d * (b * d + u * w))),
                     exp_a * (C0 + C2 * (b2 - u2 - v2)),
                     exp_a * (C2 * (b * c - v * w) + C1 * u +
                              C3 * (c * (c * u + d * v) - u * (-b2 + u2 + v2) -
                                    w * (b * d + u * w))),
                     exp_a * (C2 * (b * d + u * w) + C1 * v +
                              C3 * (d * (c * u + d * v) - v * (-b2 + u2 + v2) +
                                    w * (b * c - v * w))),

                     exp_a * (C1 * c + C2 * (-b * u + d * w) +
                              C3 * (b * (b * c - v * w) - c * (-c2 + u2 + w2) +
                                    d * (c * d - u * v))),
                     exp_a * (C2 * (b * c - v * w) - C1 * u +
                              C3 * (-b * (b * u - d * w) + u * (-c2 + u2 + w2) -
                                    v * (c * d - u * v))),
                     exp_a * (C0 + C2 * (c2 - u2 - w2)),
                     exp_a * (C2 * (c * d - u * v) + C1 * w +
                              C3 * (-d * (b * u - d * w) + v * (b * c - v * w) -
                                    w * (-c2 + u2 + w2))),

                     exp_a * (C1 * d + C2 * (-b * v - c * w) +
                              C3 * (b * (b * d + u * w) + c * (c * d - u * v) -
                                    d * (-d2 + v2 + w2))),
                     exp_a * (C2 * (b * d + u * w) - C1 * v +
                              C3 * (-b * (b * v + c * w) - u * (c * d - u * v) +
                                    v * (-d2 + v2 + w2))),
                     exp_a * (C2 * (c * d - u * v) - C1 * w +
                              C3 * (-c * (b * v + c * w) + u * (b * d + u * w) +
                                    w * (-d2 + v2 + w2))),
                     exp_a * (C0 + C2 * (d2 - v2 - w2))};
  } else if constexpr (N == 3) {
    const double a = -r * (K1[0] + K2[0]) * 0.5, b = -r * (K1[1] + K2[1]) * 0.5,
                 c = -r * (K1[2] + K2[2]) * 0.5, u = -r * (K1[N] + K2[N]) * 0.5;
    const double exp_a = exp(a);
    const double a2 = a * a, b2 = b * b, c2 = c * c, u2 = u * u;
    const double Const = b2 + c2 - u2;

    // Type of calculations
    const bool real = Const > 0;
    const bool imag = Const < 0;
    const bool either = real or imag;

    const double x =
        sqrt(imag ? -Const : Const);  // test to just use real values
    const double x2 =
        (real ? 1 : -1) * x * x;  // test to change sign if imaginary
    const double inv_x2 =
        either ? 1.0 / x2
               : 1.0;  // test so further calculations are replaced as x→0

    const double sx = real ? sinh(x) : sin(-x);  // -i sin(ix) → sinh(x)
    const double cx = real ? cosh(x) : cos(+x);  //    cos(ix) → cosh(x)

    /* Using:
     *    lim x→0 [(cosh(x) - 1) / x^2] → 1/2
     *    lim x→0 [sinh(x) / x]  → 1
     *    inv_x2 := 1 for x == 0,
     *    C0, C1, C2 ∝ [1/x^2]
     */
    const double C0 =
        either ? a2 * (cx - 1.0) - a * x * sx + x2 : 1.0 + 0.5 * a2 - a;
    const double C1 = either ? 2.0 * a * (1.0 - cx) + x * sx : 1.0 - a;
    const double C2 = either ? cx - 1.0 : 0.5;

    return TraMat<N>{(C0 + C1 * a + C2 * (a2 + b2 + c2)) * exp_a * inv_x2,
                     (C1 * b + C2 * (2 * a * b - c * u)) * exp_a * inv_x2,
                     (C1 * c + C2 * (2 * a * c + b * u)) * exp_a * inv_x2,
                     (C1 * b + C2 * (2 * a * b + c * u)) * exp_a * inv_x2,
                     (C0 + C1 * a + C2 * (a2 + b2 - u2)) * exp_a * inv_x2,
                     (C1 * u + C2 * (2 * a * u + b * c)) * exp_a * inv_x2,
                     (C1 * c + C2 * (2 * a * c - b * u)) * exp_a * inv_x2,
                     (-C1 * u - C2 * (2 * a * u - b * c)) * exp_a * inv_x2,
                     (C0 + C1 * a + C2 * (a2 + c2 - u2)) * exp_a * inv_x2};
  } else if constexpr (N == 2) {
    const double A =
        exp(-r * (0.5 * (K1[0] + K2[0]) - 0.5 * (K1[1] + K2[1]))) / 2;
    const double B =
        exp(-r * (0.5 * (K1[0] + K2[0]) + 0.5 * (K1[1] + K2[1]))) / 2;
    return TraMat<N>{A + B, B - A, B - A, A + B};
  } else if constexpr (N == 1) {
    return TraMat<N>{exp(-(K1[0] + K2[0]) * 0.5 * r)};
  }
}

template <size_t N>
constexpr TraMat<N> single_transmat(
    const Absorption::PropMat<N> &K,
    const Distance<DistanceType::meter> r) noexcept {
  return linear_transmat(K, K, r);
}

template <size_t N>
constexpr TraMat<N> dlinear_transmat(
    const TraMat<N> &T, const Absorption::PropMat<N> &K1,
    const Absorption::PropMat<N> &K2, const Absorption::PropMat<N> &dK,
    const Distance<DistanceType::meter> r,
    const Distance<DistanceType::meter> dr) noexcept {
  using Constant::inv_sqrt_2;
  using Constant::pow2;
  using Constant::pow3;
  using std::cos;
  using std::cosh;
  using std::exp;
  using std::sin;
  using std::sinh;
  using std::sqrt;
  if constexpr (N == 4) {
    const double a = -r * 0.5 * (K1[0] + K2[0]), b = -r * 0.5 * (K1[1] + K2[1]),
                 c = -r * 0.5 * (K1[2] + K2[2]), d = -r * 0.5 * (K1[3] + K2[3]),
                 u = -r * 0.5 * (K1[N] + K2[N]), v = -r * 0.5 * (K1[5] + K2[5]),
                 w = -r * 0.5 * (K1[6] + K2[6]);
    const double da = (-r * 0.5 * dK[0] - dr * 0.5 * (K1[0] + K2[0])),
                 db = (-r * 0.5 * dK[1] - dr * 0.5 * (K1[1] + K2[1])),
                 dc = (-r * 0.5 * dK[2] - dr * 0.5 * (K1[2] + K2[2])),
                 dd = (-r * 0.5 * dK[3] - dr * 0.5 * (K1[3] + K2[3])),
                 du = (-r * 0.5 * dK[N] - dr * 0.5 * (K1[N] + K2[N])),
                 dv = (-r * 0.5 * dK[5] - dr * 0.5 * (K1[5] + K2[5])),
                 dw = (-r * 0.5 * dK[6] - dr * 0.5 * (K1[6] + K2[6]));
    const double b2 = b * b, c2 = c * c, d2 = d * d, u2 = u * u, v2 = v * v,
                 w2 = w * w;
    const double db2 = 2 * db * b, dc2 = 2 * dc * c, dd2 = 2 * dd * d,
                 du2 = 2 * du * u, dv2 = 2 * dv * v, dw2 = 2 * dw * w;
    const double exp_a = exp(a);

    const double tmp =
        w2 * w2 + 2 * (b2 * (b2 * 0.5 + c2 + d2 - u2 - v2 + w2) +
                       c2 * (c2 * 0.5 + d2 - u2 + v2 - w2) +
                       d2 * (d2 * 0.5 + u2 - v2 - w2) +
                       u2 * (u2 * 0.5 + v2 + w2) + v2 * (v2 * 0.5 + w2) +
                       4 * (b * d * u * w - b * c * v * w - c * d * u * v));
    const double dtmp =
        2 * w2 * dw2 +
        2 * (db2 * (b2 * 0.5 + c2 + d2 - u2 - v2 + w2) +
             b2 * (db2 * 0.5 + dc2 + dd2 - du2 - dv2 + dw2) +
             dc2 * (c2 * 0.5 + d2 - u2 + v2 - w2) +
             c2 * (dc2 * 0.5 + dd2 - du2 + dv2 - dw2) +
             dd2 * (d2 * 0.5 + u2 - v2 - w2) +
             d2 * (dd2 * 0.5 + du2 - dv2 - dw2) + du2 * (u2 * 0.5 + v2 + w2) +
             u2 * (du2 * 0.5 + dv2 + dw2) + dv2 * (v2 * 0.5 + w2) +
             v2 * (dv2 * 0.5 + dw2) +
             4 * (db * d * u * w - db * c * v * w - dc * d * u * v +
                  b * dd * u * w - b * dc * v * w - c * dd * u * v +
                  b * d * du * w - b * c * dv * w - c * d * du * v +
                  b * d * u * dw - b * c * v * dw - c * d * u * dv));

    const Complex Const1 = sqrt(Complex{tmp});
    const double Const2 = b2 + c2 + d2 - u2 - v2 - w2;
    const Complex dConst1 = 0.5 * dtmp / Const1;
    const double dConst2 = db2 + dc2 + dd2 - du2 - dv2 - dw2;

    const Complex tmp_x_sqrt = std::sqrt(Const2 + Const1);
    const Complex tmp_y_sqrt = std::sqrt(Const2 - Const1);
    const Complex x = tmp_x_sqrt * inv_sqrt_2;
    const Complex y = tmp_y_sqrt * Complex{0, inv_sqrt_2};
    const Complex x2 = x * x;
    const Complex y2 = y * y;
    const Complex cy = cos(y);
    const Complex sy = sin(y);
    const Complex cx = cosh(x);
    const Complex sx = sinh(x);

    const bool x_zero = std::abs(x) < 1e-4;
    const bool y_zero = std::abs(y) < 1e-4;
    const bool both_zero = y_zero and x_zero;
    const bool either_zero = y_zero or x_zero;

    const Complex dx =
        x_zero ? 0 : (0.5 * (dConst2 + dConst1) / tmp_x_sqrt) * inv_sqrt_2;
    const Complex dy = y_zero ? 0
                              : (0.5 * (dConst2 - dConst1) / tmp_y_sqrt) *
                                    Complex(0, inv_sqrt_2);
    const Complex dx2 = 2.0 * x * dx;
    const Complex dy2 = 2.0 * y * dy;
    const Complex dcy = -sy * dy;
    const Complex dsy = cy * dy;
    const Complex dcx = sx * dx;
    const Complex dsx = cx * dx;

    /* Using:
     *    lim x→0 [({cosh(x),cos(x)} - 1) / x^2] → 1/2
     *    lim x→0 [{sinh(x),sin(x)} / x]  → 1
     *    inv_x2 := 1 for x == 0,
     *    -i sin(ix) → sinh(x)
     *    cos(ix) → cosh(x)
     *    C0, C1, C2 ∝ [1/x^2]
     */
    const Complex ix = x_zero ? 0.0 : 1.0 / x;
    const Complex iy = y_zero ? 0.0 : 1.0 / y;
    const Complex inv_x2y2 =
        both_zero
            ? 1.0
            : 1.0 / (x2 + y2);  // The first "1.0" is the trick for above limits
    const Complex dix = -dx * ix * ix;
    const Complex diy = -dy * iy * iy;
    const Complex dx2dy2 = dx2 + dy2;

    const Complex C0c = either_zero ? 1.0 : (cy * x2 + cx * y2) * inv_x2y2;
    const Complex C1c =
        either_zero ? 1.0 : (sy * x2 * iy + sx * y2 * ix) * inv_x2y2;
    const Complex C2c = both_zero ? 0.5 : (cx - cy) * inv_x2y2;
    const Complex C3c =
        both_zero ? 1.0 / 6.0
                  : (x_zero ? 1.0 - sy * iy
                            : y_zero ? sx * ix - 1.0 : sx * ix - sy * iy) *
                        inv_x2y2;
    const Complex dC0c =
        either_zero
            ? 0.0
            : (dcy * x2 + cy * dx2 + dcx * y2 + cx * dy2 - C0c * dx2dy2) *
                  inv_x2y2;
    const Complex dC1c =
        either_zero
            ? 0.0
            : (dsy * x2 * iy + sy * dx2 * iy + sy * x2 * diy + dsx * y2 * ix +
               sx * dy2 * ix + sx * y2 * dix - C1c * dx2dy2) *
                  inv_x2y2;
    const Complex dC2c =
        both_zero ? 0.0 : (dcx - dcy - C2c * dx2dy2) * inv_x2y2;
    const Complex dC3c =
        both_zero
            ? 0.0
            : ((x_zero ? -dsy * iy - sy * diy
                       : y_zero ? dsx * ix + sx * dix
                                : dsx * ix + sx * dix - dsy * iy - sy * diy) -
               C3c * dx2dy2) *
                  inv_x2y2;

    //     const double &C0 = reinterpret_cast<const
    //     double(&)[2]>(C0c)[0];
    const double &C1 = reinterpret_cast<const double(&)[2]>(C1c)[0];
    const double &C2 = reinterpret_cast<const double(&)[2]>(C2c)[0];
    const double &C3 = reinterpret_cast<const double(&)[2]>(C3c)[0];
    const double &dC0 = reinterpret_cast<const double(&)[2]>(dC0c)[0];
    const double &dC1 = reinterpret_cast<const double(&)[2]>(dC1c)[0];
    const double &dC2 = reinterpret_cast<const double(&)[2]>(dC2c)[0];
    const double &dC3 = reinterpret_cast<const double(&)[2]>(dC3c)[0];

    return TraMat<N>{
        T(0, 0) * da +
            exp_a * (dC0 + dC2 * (b2 + c2 + d2) + C2 * (db2 + dc2 + dd2)),

        T(0, 1) * da +
            exp_a * (db * C1 + b * dC1 + dC2 * (-c * u - d * v) +
                     C2 * (-dc * u - dd * v - c * du - d * dv) +
                     dC3 * (b * (b2 + c2 + d2) - u * (b * u - d * w) -
                            v * (b * v + c * w)) +
                     C3 * (db * (b2 + c2 + d2) - du * (b * u - d * w) -
                           dv * (b * v + c * w) + b * (db2 + dc2 + dd2) -
                           u * (db * u - dd * w) - v * (db * v + dc * w) -
                           u * (b * du - d * dw) - v * (b * dv + c * dw))),

        T(0, 2) * da +
            exp_a * (dC1 * c + C1 * dc + dC2 * (b * u - d * w) +
                     C2 * (db * u - dd * w + b * du - d * dw) +
                     dC3 * (c * (b2 + c2 + d2) - u * (c * u + d * v) -
                            w * (b * v + c * w)) +
                     C3 * (dc * (b2 + c2 + d2) - du * (c * u + d * v) -
                           dw * (b * v + c * w) + c * (db2 + dc2 + dd2) -
                           u * (dc * u + dd * v) - w * (db * v + dc * w) -
                           u * (c * du + d * dv) - w * (b * dv + c * dw))),

        T(0, 3) * da +
            exp_a * (dC1 * d + C1 * dd + dC2 * (b * v + c * w) +
                     C2 * (db * v + dc * w + b * dv + c * dw) +
                     dC3 * (d * (b2 + c2 + d2) - v * (c * u + d * v) +
                            w * (b * u - d * w)) +
                     C3 * (dd * (b2 + c2 + d2) - dv * (c * u + d * v) +
                           dw * (b * u - d * w) + d * (db2 + dc2 + dd2) -
                           v * (dc * u + dd * v) + w * (db * u - dd * w) -
                           v * (c * du + d * dv) + w * (b * du - d * dw))),

        T(1, 0) * da +
            exp_a * (db * C1 + b * dC1 + dC2 * (c * u + d * v) +
                     C2 * (dc * u + dd * v + c * du + d * dv) +
                     dC3 * (-b * (-b2 + u2 + v2) + c * (b * c - v * w) +
                            d * (b * d + u * w)) +
                     C3 * (-db * (-b2 + u2 + v2) + dc * (b * c - v * w) +
                           dd * (b * d + u * w) - b * (-db2 + du2 + dv2) +
                           c * (db * c - dv * w) + d * (db * d + du * w) +
                           c * (b * dc - v * dw) + d * (b * dd + u * dw))),

        T(1, 1) * da +
            exp_a * (dC0 + dC2 * (b2 - u2 - v2) + C2 * (db2 - du2 - dv2)),

        T(1, 2) * da +
            exp_a *
                (dC2 * (b * c - v * w) +
                 C2 * (db * c + b * dc - dv * w - v * dw) + dC1 * u + C1 * du +
                 dC3 * (c * (c * u + d * v) - u * (-b2 + u2 + v2) -
                        w * (b * d + u * w)) +
                 C3 * (dc * (c * u + d * v) - du * (-b2 + u2 + v2) -
                       dw * (b * d + u * w) + c * (dc * u + dd * v) -
                       u * (-db2 + du2 + dv2) - w * (db * d + du * w) +
                       c * (c * du + d * dv) - w * (b * dd + u * dw))),

        T(1, 3) * da +
            exp_a *
                (dC2 * (b * d + u * w) +
                 C2 * (db * d + b * dd + du * w + u * dw) + dC1 * v + C1 * dv +
                 dC3 * (d * (c * u + d * v) - v * (-b2 + u2 + v2) +
                        w * (b * c - v * w)) +
                 C3 * (dd * (c * u + d * v) - dv * (-b2 + u2 + v2) +
                       dw * (b * c - v * w) + d * (dc * u + dd * v) -
                       v * (-db2 + du2 + dv2) + w * (db * c - dv * w) +
                       d * (c * du + d * dv) + w * (b * dc - v * dw))),

        T(2, 0) * da +
            exp_a * (dC1 * c + C1 * dc + dC2 * (-b * u + d * w) +
                     C2 * (-db * u + dd * w - b * du + d * dw) +
                     dC3 * (b * (b * c - v * w) - c * (-c2 + u2 + w2) +
                            d * (c * d - u * v)) +
                     C3 * (db * (b * c - v * w) - dc * (-c2 + u2 + w2) +
                           dd * (c * d - u * v) + b * (db * c - dv * w) -
                           c * (-dc2 + du2 + dw2) + d * (dc * d - du * v) +
                           b * (b * dc - v * dw) + d * (c * dd - u * dv))),

        T(2, 1) * da +
            exp_a *
                (dC2 * (b * c - v * w) +
                 C2 * (db * c + b * dc - dv * w - v * dw) - dC1 * u - C1 * du +
                 dC3 * (-b * (b * u - d * w) + u * (-c2 + u2 + w2) -
                        v * (c * d - u * v)) +
                 C3 * (-db * (b * u - d * w) + du * (-c2 + u2 + w2) -
                       dv * (c * d - u * v) - b * (db * u - dd * w) +
                       u * (-dc2 + du2 + dw2) - v * (dc * d - du * v) -
                       b * (b * du - d * dw) - v * (c * dd - u * dv))),

        T(2, 2) * da +
            exp_a * (dC0 + dC2 * (c2 - u2 - w2) + C2 * (dc2 - du2 - dw2)),

        T(2, 3) * da +
            exp_a *
                (dC2 * (c * d - u * v) +
                 C2 * (dc * d + c * dd - du * v - u * dv) + dC1 * w + C1 * dw +
                 dC3 * (-d * (b * u - d * w) + v * (b * c - v * w) -
                        w * (-c2 + u2 + w2)) +
                 C3 * (-dd * (b * u - d * w) + dv * (b * c - v * w) -
                       dw * (-c2 + u2 + w2) - d * (db * u - dd * w) +
                       v * (db * c - dv * w) - w * (-dc2 + du2 + dw2) -
                       d * (b * du - d * dw) + v * (b * dc - v * dw))),

        T(3, 0) * da +
            exp_a * (dC1 * d + C1 * dd + dC2 * (-b * v - c * w) +
                     C2 * (-db * v - dc * w - b * dv - c * dw) +
                     dC3 * (b * (b * d + u * w) + c * (c * d - u * v) -
                            d * (-d2 + v2 + w2)) +
                     C3 * (db * (b * d + u * w) + dc * (c * d - u * v) -
                           dd * (-d2 + v2 + w2) + b * (db * d + du * w) +
                           c * (dc * d - du * v) - d * (-dd2 + dv2 + dw2) +
                           b * (b * dd + u * dw) + c * (c * dd - u * dv))),

        T(3, 1) * da +
            exp_a *
                (dC2 * (b * d + u * w) +
                 C2 * (db * d + b * dd + du * w + u * dw) - dC1 * v - C1 * dv +
                 dC3 * (-b * (b * v + c * w) - u * (c * d - u * v) +
                        v * (-d2 + v2 + w2)) +
                 C3 * (-db * (b * v + c * w) - du * (c * d - u * v) +
                       dv * (-d2 + v2 + w2) - b * (db * v + dc * w) -
                       u * (dc * d - du * v) + v * (-dd2 + dv2 + dw2) -
                       b * (b * dv + c * dw) - u * (c * dd - u * dv))),

        T(3, 2) * da +
            exp_a *
                (dC2 * (c * d - u * v) +
                 C2 * (dc * d + c * dd - du * v - u * dv) - dC1 * w - C1 * dw +
                 dC3 * (-c * (b * v + c * w) + u * (b * d + u * w) +
                        w * (-d2 + v2 + w2)) +
                 C3 * (-dc * (b * v + c * w) + du * (b * d + u * w) +
                       dw * (-d2 + v2 + w2) - c * (db * v + dc * w) +
                       u * (db * d + du * w) + w * (-dd2 + dv2 + dw2) -
                       c * (b * dv + c * dw) + u * (b * dd + u * dw))),

        T(3, 3) * da +
            exp_a * (dC0 + dC2 * (d2 - v2 - w2) + C2 * (dd2 - dv2 - dw2))};
  } else if constexpr (N == 3) {
    const double a = -r * 0.5 * (K1[0] + K2[0]), b = -r * 0.5 * (K1[1] + K2[1]),
                 c = -r * 0.5 * (K1[2] + K2[2]), u = -r * 0.5 * (K1[N] + K2[N]);
    const double da = -r * 0.5 * dK[0] - dr * 0.5 * (K1[0] + K2[0]),
                 db = -r * 0.5 * dK[1] - dr * 0.5 * (K1[1] + K2[1]),
                 dc = -r * 0.5 * dK[2] - dr * 0.5 * (K1[2] + K2[2]),
                 du = -r * 0.5 * dK[N] - dr * 0.5 * (K1[N] + K2[N]);
    const double a2 = a * a, b2 = b * b, c2 = c * c, u2 = u * u;
    const double Const = b2 + c2 - u2;

    const bool real = Const > 0;
    const bool imag = Const < 0;
    const bool either = real or imag;
    const double x =
        sqrt(imag ? -Const : Const);  // test to just use real values
    const double x2 = real ? x * x : -x * x;
    const double inv_x =
        either ? 1.0 / x
               : 1.0;  // test so further calculations are replaced as x→0
    const double da2 = 2 * a * da, db2 = 2 * b * db, dc2 = 2 * c * dc,
                 du2 = 2 * u * du;
    const double dx = either ? ((db2 + dc2 - du2) * inv_x * 0.5) : 0,
                 dx2 = 2 * x * dx;
    const double cx = real ? cosh(x) : cos(+x);  //    cos(ix) → cosh(x)
    const double dsx = (real ? 1 : -1) * cx * dx;
    const double sx =
        real ? std::sinh(x) : std::sin(-x);  // -i sin(ix) → sinh(x)
    const double dcx = sx * dx;
    const double C0 =
        either ? a2 * (cx - 1.0) - a * x * sx + x2 : 1.0 + 0.5 * a2 - a;
    const double dC0 = either
                           ? da2 * (cx - 1.0) + da2 * (dcx - 1.0) -
                                 da * x * sx - a * dx * sx - a * x * dsx + dx2
                           : 0.5 * da2 - da;
    const double dC1 =
        either ? 2.0 * (da * (1.0 - cx) - a * dcx) + dx * sx + x * dsx : -da;
    const double dC2 = either ? dcx : 0;
    const double inv_x2 = inv_x * inv_x;
    const double exp_a = exp(a);
    const double dexp_a = exp_a * da;
    const double C1 = either ? 2.0 * a * (1.0 - cx) + x * sx : 1.0 - a;
    const double C2 = either ? cx - 1.0 : 0.5;

    return {((((a2 + b2 + c2) * C2 + C0 + C1 * a) * dexp_a +
              ((a2 + b2 + c2) * dC2 + (da2 + db2 + dc2) * C2 + C1 * da +
               a * dC1 + dC0) *
                  exp_a) *
                 x2 -
             ((a2 + b2 + c2) * C2 + C0 + C1 * a) * exp_a * dx2) *
                inv_x2,
            ((((2 * a * b - c * u) * C2 + C1 * b) * dexp_a +
              ((2 * a * b - c * u) * dC2 +
               (2 * a * db + 2 * b * da - c * du - u * dc) * C2 + C1 * db +
               b * dC1) *
                  exp_a) *
                 x2 -
             ((2 * a * b - c * u) * C2 + C1 * b) * exp_a * dx2) *
                inv_x2,
            ((((2 * a * c + b * u) * C2 + C1 * c) * dexp_a +
              ((2 * a * c + b * u) * dC2 +
               (2 * a * dc + b * du + 2 * c * da + u * db) * C2 + C1 * dc +
               c * dC1) *
                  exp_a) *
                 x2 -
             ((2 * a * c + b * u) * C2 + C1 * c) * exp_a * dx2) *
                inv_x2,
            ((((2 * a * b + c * u) * C2 + C1 * b) * dexp_a +
              ((2 * a * b + c * u) * dC2 +
               (2 * a * db + 2 * b * da + c * du + u * dc) * C2 + C1 * db +
               b * dC1) *
                  exp_a) *
                 x2 -
             ((2 * a * b + c * u) * C2 + C1 * b) * exp_a * dx2) *
                inv_x2,
            ((((a2 + b2 - u2) * C2 + C0 + C1 * a) * dexp_a +
              ((a2 + b2 - u2) * dC2 + (da2 + db2 - du2) * C2 + C1 * da +
               a * dC1 + dC0) *
                  exp_a) *
                 x2 -
             ((a2 + b2 - u2) * C2 + C0 + C1 * a) * exp_a * dx2) *
                inv_x2,
            ((((2 * a * u + b * c) * C2 + C1 * u) * dexp_a +
              ((2 * a * u + b * c) * dC2 +
               (2 * a * du + b * dc + c * db + 2 * u * da) * C2 + C1 * du +
               u * dC1) *
                  exp_a) *
                 x2 -
             ((2 * a * u + b * c) * C2 + C1 * u) * exp_a * dx2) *
                inv_x2,
            ((((2 * a * c - b * u) * C2 + C1 * c) * dexp_a +
              ((2 * a * c - b * u) * dC2 +
               (2 * a * dc - b * du + 2 * c * da - u * db) * C2 + C1 * dc +
               c * dC1) *
                  exp_a) *
                 x2 -
             ((2 * a * c - b * u) * C2 + C1 * c) * exp_a * dx2) *
                inv_x2,
            (-(((2 * a * u - b * c) * C2 + C1 * u) * dexp_a +
               ((2 * a * u - b * c) * dC2 +
                (2 * a * du - b * dc - c * db + 2 * u * da) * C2 + C1 * du +
                u * dC1) *
                   exp_a) *
                 x2 +
             ((2 * a * u - b * c) * C2 + C1 * u) * exp_a * dx2) *
                inv_x2,
            ((((a2 + c2 - u2) * C2 + C0 + C1 * a) * dexp_a +
              ((a2 + c2 - u2) * dC2 + (da2 + dc2 - du2) * C2 + C1 * da +
               a * dC1 + dC0) *
                  exp_a) *
                 x2 -
             ((a2 + c2 - u2) * C2 + C0 + C1 * a) * exp_a * dx2) *
                inv_x2};
  } else if constexpr (N == 2) {
    const double dA = (-r * (0.5 * dK[0] - 0.5 * dK[1]) -
                       dr * (0.5 * (K1[0] + K2[0]) - 0.5 * (K1[1] + K2[1]))) *
                      (T(0, 0) - T(0, 1)) / 2;
    const double dB = (-r * (0.5 * dK[0] + 0.5 * dK[1]) -
                       dr * (0.5 * (K1[0] + K2[0]) + 0.5 * (K1[1] + K2[1]))) *
                      (T(0, 0) + T(0, 1)) / 2;
    return TraMat<N>{dA + dB, dB - dA, dB - dA, dA + dB};
    return TraMat<N>{};
  } else if constexpr (N == 1) {
    const double a = 0.5 * (K1[0] + K2[0]), da = 0.5 * dK[0];
    return TraMat<N>{-(r * da + dr * a) * T(0, 0)};
  }
}
}  // namespace RTE

#endif
