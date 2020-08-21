#ifndef rte_h
#define rte_h

#include <array>
#include <iostream>

#include "complex.h"
#include "constants.h"
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

  friend std::ostream& operator<<(std::ostream& os, RadVec a) {
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

  friend std::ostream& operator<<(std::ostream& os, TraMat a) {
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
};  // TraMat

/** Returns T (I - J) + J */
template <size_t N>
constexpr RadVec<N> update(const RadVec<N> I, const TraMat<N> T,
                           const RadVec<N> J) noexcept {
  if constexpr (N == 4)
    return RadVec<N>{
        T(0, 0) * (I[0] - J[0]) + T(0, 1) * (I[1] - J[1]) +
            T(0, 2) * (I[2] - J[2]) + T(0, 3) * (I[3] - J[3]) + J[0],
        T(1, 0) * (I[0] - J[0]) + T(1, 1) * (I[1] - J[1]) +
            T(1, 2) * (I[2] - J[2]) + T(1, 3) * (I[3] - J[3]) + J[1],
        T(2, 0) * (I[0] - J[0]) + T(2, 1) * (I[1] - J[1]) +
            T(2, 2) * (I[2] - J[2]) + T(2, 3) * (I[3] - J[3]) + J[2],
        T(3, 0) * (I[0] - J[0]) + T(3, 1) * (I[1] - J[1]) +
            T(3, 2) * (I[2] - J[2]) + T(3, 3) * (I[3] - J[3]) + J[3]};
  else if constexpr (N == 3)
    return RadVec<N>{T(0, 0) * (I[0] - J[0]) + T(0, 1) * (I[1] - J[1]) +
                         T(0, 2) * (I[2] - J[2]) + J[0],
                     T(1, 0) * (I[0] - J[0]) + T(1, 1) * (I[1] - J[1]) +
                         T(1, 2) * (I[2] - J[2]) + J[1],
                     T(2, 0) * (I[0] - J[0]) + T(2, 1) * (I[1] - J[1]) +
                         T(2, 2) * (I[2] - J[2]) + J[2]};
  else if constexpr (N == 2)
    return RadVec<N>{T(0, 0) * (I[0] - J[0]) + T(0, 1) * (I[1] - J[1]) + J[0],
                     T(1, 0) * (I[0] - J[0]) + T(1, 1) * (I[1] - J[1]) + J[1]};
  else if constexpr (N == 1)
    return RadVec<N>{T(0, 0) * (I[0] - J[0]) + J[0]};
}

/** Returns dT (I - J) + dJ - T dJ */
template <size_t N>
constexpr RadVec<N> dupdate(const RadVec<N> I, const TraMat<N> T,
                            const TraMat<N> dT, const RadVec<N> J,
                            const RadVec<N> dJ) noexcept {
  if constexpr (N == 4)
    return RadVec<N>{
        -T(0, 0) * dJ[0] - T(0, 1) * dJ[1] - T(0, 2) * dJ[2] - T(0, 3) * dJ[3] +
            dT(0, 0) * (I[0] - J[0]) + dT(0, 1) * (I[1] - J[1]) +
            dT(0, 2) * (I[2] - J[2]) + dT(0, 3) * (I[3] - J[3]) + dJ[0],
        -T(1, 0) * dJ[0] - T(1, 1) * dJ[1] - T(1, 2) * dJ[2] - T(1, 3) * dJ[3] +
            dT(1, 0) * (I[0] - J[0]) + dT(1, 1) * (I[1] - J[1]) +
            dT(1, 2) * (I[2] - J[2]) + dT(1, 3) * (I[3] - J[3]) + dJ[1],
        -T(2, 0) * dJ[0] - T(2, 1) * dJ[1] - T(2, 2) * dJ[2] - T(2, 3) * dJ[3] +
            dT(2, 0) * (I[0] - J[0]) + dT(2, 1) * (I[1] - J[1]) +
            dT(2, 2) * (I[2] - J[2]) + dT(2, 3) * (I[3] - J[3]) + dJ[2],
        -T(3, 0) * dJ[0] - T(3, 1) * dJ[1] - T(3, 2) * dJ[2] - T(3, 3) * dJ[3] +
            dT(3, 0) * (I[0] - J[0]) + dT(3, 1) * (I[1] - J[1]) +
            dT(3, 2) * (I[2] - J[2]) + dT(3, 3) * (I[3] - J[3]) + dJ[3]};
  else if constexpr (N == 3)
    return RadVec<N>{-T(0, 0) * dJ[0] - T(0, 1) * dJ[1] - T(0, 2) * dJ[2] +
                         dT(0, 0) * (I[0] - J[0]) + dT(0, 1) * (I[1] - J[1]) +
                         dT(0, 2) * (I[2] - J[2]) + dJ[0],
                     -T(1, 0) * dJ[0] - T(1, 1) * dJ[1] - T(1, 2) * dJ[2] +
                         dT(1, 0) * (I[0] - J[0]) + dT(1, 1) * (I[1] - J[1]) +
                         dT(1, 2) * (I[2] - J[2]) + dJ[1],
                     -T(2, 0) * dJ[0] - T(2, 1) * dJ[1] - T(2, 2) * dJ[2] +
                         dT(2, 0) * (I[0] - J[0]) + dT(2, 1) * (I[1] - J[1]) +
                         dT(2, 2) * (I[2] - J[2]) + dJ[2]};
  else if constexpr (N == 2)
    return RadVec<N>{
        -T(0, 0) * dJ[0] - T(0, 1) * dJ[1] + dT(0, 0) * (I[0] - J[0]) +
            dT(0, 1) * (I[1] - J[1]) + dJ[0],
        -T(1, 0) * dJ[0] - T(1, 1) * dJ[1] + dT(1, 0) * (I[0] - J[0]) +
            dT(1, 1) * (I[1] - J[1]) + dJ[1]};
  else if constexpr (N == 1)
    return RadVec<N>{-T(0, 0) * dJ[0] + dT(0, 0) * (I[0] - J[0]) + dJ[0]};
}

/** Planck's law
 *
 * FIXME: should not return double but a proper unit
 */
double B(Temperature<TemperatureType::K> T,
         Frequency<FrequencyType::Freq> f) noexcept;

/** Planck's law temperature derivative
 *
 * FIXME: should not return double but a proper unit
 */
double dBdT(Temperature<TemperatureType::K> T,
            Frequency<FrequencyType::Freq> f) noexcept;

/** Planck's law frequency derivative
 *
 * FIXME: should not return double but a proper unit
 */
double dBdf(Temperature<TemperatureType::K> T,
            Frequency<FrequencyType::Freq> f) noexcept;

/** Returns K^-1 (A B + S)
 *
 * FIXME:  B should not be a double...
 */
template <size_t N>
constexpr RadVec<N> source(const Absorption::PropMat<N> K, const RadVec<N> S,
                           const RadVec<N> A, const double B) noexcept {
  using Constant::pow2;
  if constexpr (N == 4) {
    const double invden = 1.0 / K.inverse_denominator();
    return {(K[0] * (A[0] * B + S[0]) *
                 (pow2(K[0]) + pow2(K[N]) + pow2(K[5]) + pow2(K[6])) +
             (A[1] * B + S[1]) *
                 (-pow2(K[0]) * K[1] - K[0] * K[2] * K[N] - K[0] * K[3] * K[5] -
                  K[1] * pow2(K[6]) + K[2] * K[5] * K[6] - K[3] * K[N] * K[6]) +
             (A[2] * B + S[2]) *
                 (-pow2(K[0]) * K[2] + K[0] * K[1] * K[N] - K[0] * K[3] * K[6] +
                  K[1] * K[5] * K[6] - K[2] * pow2(K[5]) + K[3] * K[N] * K[5]) +
             (A[3] * B + S[3]) * (-pow2(K[0]) * K[3] + K[0] * K[1] * K[5] +
                                  K[0] * K[2] * K[6] - K[1] * K[N] * K[6] +
                                  K[2] * K[N] * K[5] - K[3] * pow2(K[N]))) *
                invden,
            (K[0] * (A[1] * B + S[1]) *
                 (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) +
             (A[0] * B + S[0]) *
                 (-pow2(K[0]) * K[1] + K[0] * K[2] * K[N] + K[0] * K[3] * K[5] -
                  K[1] * pow2(K[6]) + K[2] * K[5] * K[6] - K[3] * K[N] * K[6]) +
             (A[2] * B + S[2]) *
                 (-pow2(K[0]) * K[N] + K[0] * K[1] * K[2] - K[0] * K[5] * K[6] +
                  K[1] * K[3] * K[6] - K[2] * K[3] * K[5] + pow2(K[3]) * K[N]) +
             (A[3] * B + S[3]) * (-pow2(K[0]) * K[5] + K[0] * K[1] * K[3] +
                                  K[0] * K[N] * K[6] - K[1] * K[2] * K[6] +
                                  pow2(K[2]) * K[5] - K[2] * K[3] * K[N])) *
                invden,
            (K[0] * (A[2] * B + S[2]) *
                 (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) +
             (A[0] * B + S[0]) *
                 (-pow2(K[0]) * K[2] - K[0] * K[1] * K[N] + K[0] * K[3] * K[6] +
                  K[1] * K[5] * K[6] - K[2] * pow2(K[5]) + K[3] * K[N] * K[5]) +
             (A[1] * B + S[1]) *
                 (pow2(K[0]) * K[N] + K[0] * K[1] * K[2] - K[0] * K[5] * K[6] -
                  K[1] * K[3] * K[6] + K[2] * K[3] * K[5] - pow2(K[3]) * K[N]) +
             (A[3] * B + S[3]) * (-pow2(K[0]) * K[6] + K[0] * K[2] * K[3] -
                                  K[0] * K[N] * K[5] + pow2(K[1]) * K[6] -
                                  K[1] * K[2] * K[5] + K[1] * K[3] * K[N])) *
                invden,
            (K[0] * (A[3] * B + S[3]) *
                 (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[N])) +
             (A[0] * B + S[0]) *
                 (-pow2(K[0]) * K[3] - K[0] * K[1] * K[5] - K[0] * K[2] * K[6] -
                  K[1] * K[N] * K[6] + K[2] * K[N] * K[5] - K[3] * pow2(K[N])) +
             (A[1] * B + S[1]) *
                 (pow2(K[0]) * K[5] + K[0] * K[1] * K[3] + K[0] * K[N] * K[6] +
                  K[1] * K[2] * K[6] - pow2(K[2]) * K[5] + K[2] * K[3] * K[N]) +
             (A[2] * B + S[2]) * (pow2(K[0]) * K[6] + K[0] * K[2] * K[3] -
                                  K[0] * K[N] * K[5] - pow2(K[1]) * K[6] +
                                  K[1] * K[2] * K[5] - K[1] * K[3] * K[N])) *
                invden};
  } else if constexpr (N == 3) {
    const double invden = 1.0 / K.inverse_denominator();
    return RadVec<N>{((pow2(K[0]) + pow2(K[N])) * (A[0] * B + S[0]) -
                      (A[1] * B + S[1]) * (K[0] * K[1] + K[2] * K[N]) +
                      (A[2] * B + S[2]) * (-K[0] * K[2] + K[1] * K[N])) *
                         invden,
                     ((pow2(K[0]) - pow2(K[2])) * (A[1] * B + S[1]) +
                      (A[0] * B + S[0]) * (-K[0] * K[1] + K[2] * K[N]) +
                      (A[2] * B + S[2]) * (-K[0] * K[N] + K[1] * K[2])) *
                         invden,
                     ((pow2(K[0]) - pow2(K[1])) * (A[2] * B + S[2]) -
                      (A[0] * B + S[0]) * (K[0] * K[2] + K[1] * K[N]) +
                      (A[1] * B + S[1]) * (K[0] * K[N] + K[1] * K[2])) *
                         invden};
  } else if constexpr (N == 2) {
    const double invden = 1.0 / K.inverse_denominator();
    return RadVec<N>{
        (K[0] * (A[0] * B + S[0]) - K[1] * (A[1] * B + S[1])) * invden,
        (K[0] * (A[1] * B + S[1]) - K[1] * (A[0] * B + S[0])) * invden};
  } else if constexpr (N == 1) {
    return RadVec<N>{(A[0] * B + S[0]) / K.inverse_denominator()};
  }
}

/** Returns K^-1((dA B + A dB + dS) - dK K^-1 J)
 *
 * where J is K^-1 (A B + S) [e.g., source(K, S, A, B)]
 *
 * FIXME:  B should not be a double...
 */
template <size_t N>
constexpr RadVec<N> dsource(const Absorption::PropMat<N> K,
                            const Absorption::PropMat<N> dK, const RadVec<N> J,
                            const RadVec<N> dS, const RadVec<N> A,
                            const RadVec<N> dA, const double B,
                            const double dB) {
  using Constant::pow2;
  using Constant::pow4;
  if constexpr (N == 4) {
    const double invden = 1.0 / K.inverse_denominator();
    return RadVec<N>{
        (K[0] * (A[0] * dB + B * dA[0] + dS[0]) *
             (pow2(K[0]) + pow2(K[N]) + pow2(K[5]) + pow2(K[6])) +
         (A[1] * dB + B * dA[1] + dS[1]) *
             (-pow2(K[0]) * K[1] - K[0] * K[2] * K[N] - K[0] * K[3] * K[5] -
              K[1] * pow2(K[6]) + K[2] * K[5] * K[6] - K[3] * K[N] * K[6]) +
         (A[2] * dB + B * dA[2] + dS[2]) *
             (-pow2(K[0]) * K[2] + K[0] * K[1] * K[N] - K[0] * K[3] * K[6] +
              K[1] * K[5] * K[6] - K[2] * pow2(K[5]) + K[3] * K[N] * K[5]) +
         (A[3] * dB + B * dA[3] + dS[3]) *
             (-pow2(K[0]) * K[3] + K[0] * K[1] * K[5] + K[0] * K[2] * K[6] -
              K[1] * K[N] * K[6] + K[2] * K[N] * K[5] - K[3] * pow2(K[N])) +
         (-J[0] * dK[0] - J[1] * dK[1] - J[2] * dK[2] - J[3] * dK[3]) *
             (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
              pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[N]) +
              pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
              pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
              2 * K[1] * K[3] * K[N] * K[6] - pow2(K[2]) * pow2(K[5]) +
              2 * K[2] * K[3] * K[N] * K[5] - pow2(K[3]) * pow2(K[N]))) *
            invden,
        (K[0] * (A[1] * dB + B * dA[1] + dS[1]) *
             (pow2(K[0]) - pow2(K[2]) - pow2(K[3]) + pow2(K[6])) +
         (A[0] * dB + B * dA[0] + dS[0]) *
             (-pow2(K[0]) * K[1] + K[0] * K[2] * K[N] + K[0] * K[3] * K[5] -
              K[1] * pow2(K[6]) + K[2] * K[5] * K[6] - K[3] * K[N] * K[6]) +
         (A[2] * dB + B * dA[2] + dS[2]) *
             (-pow2(K[0]) * K[N] + K[0] * K[1] * K[2] - K[0] * K[5] * K[6] +
              K[1] * K[3] * K[6] - K[2] * K[3] * K[5] + pow2(K[3]) * K[N]) +
         (A[3] * dB + B * dA[3] + dS[3]) *
             (-pow2(K[0]) * K[5] + K[0] * K[1] * K[3] + K[0] * K[N] * K[6] -
              K[1] * K[2] * K[6] + pow2(K[2]) * K[5] - K[2] * K[3] * K[N]) +
         (-J[0] * dK[1] - J[1] * dK[0] - J[2] * dK[N] - J[3] * dK[5]) *
             (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
              pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[N]) +
              pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
              pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
              2 * K[1] * K[3] * K[N] * K[6] - pow2(K[2]) * pow2(K[5]) +
              2 * K[2] * K[3] * K[N] * K[5] - pow2(K[3]) * pow2(K[N]))) *
            invden,
        (K[0] * (A[2] * dB + B * dA[2] + dS[2]) *
             (pow2(K[0]) - pow2(K[1]) - pow2(K[3]) + pow2(K[5])) +
         (A[0] * dB + B * dA[0] + dS[0]) *
             (-pow2(K[0]) * K[2] - K[0] * K[1] * K[N] + K[0] * K[3] * K[6] +
              K[1] * K[5] * K[6] - K[2] * pow2(K[5]) + K[3] * K[N] * K[5]) +
         (A[1] * dB + B * dA[1] + dS[1]) *
             (pow2(K[0]) * K[N] + K[0] * K[1] * K[2] - K[0] * K[5] * K[6] -
              K[1] * K[3] * K[6] + K[2] * K[3] * K[5] - pow2(K[3]) * K[N]) +
         (A[3] * dB + B * dA[3] + dS[3]) *
             (-pow2(K[0]) * K[6] + K[0] * K[2] * K[3] - K[0] * K[N] * K[5] +
              pow2(K[1]) * K[6] - K[1] * K[2] * K[5] + K[1] * K[3] * K[N]) +
         (-J[0] * dK[2] + J[1] * dK[N] - J[2] * dK[0] - J[3] * dK[6]) *
             (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
              pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[N]) +
              pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
              pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
              2 * K[1] * K[3] * K[N] * K[6] - pow2(K[2]) * pow2(K[5]) +
              2 * K[2] * K[3] * K[N] * K[5] - pow2(K[3]) * pow2(K[N]))) *
            invden,
        (K[0] * (A[3] * dB + B * dA[3] + dS[3]) *
             (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[N])) +
         (A[0] * dB + B * dA[0] + dS[0]) *
             (-pow2(K[0]) * K[3] - K[0] * K[1] * K[5] - K[0] * K[2] * K[6] -
              K[1] * K[N] * K[6] + K[2] * K[N] * K[5] - K[3] * pow2(K[N])) +
         (A[1] * dB + B * dA[1] + dS[1]) *
             (pow2(K[0]) * K[5] + K[0] * K[1] * K[3] + K[0] * K[N] * K[6] +
              K[1] * K[2] * K[6] - pow2(K[2]) * K[5] + K[2] * K[3] * K[N]) +
         (A[2] * dB + B * dA[2] + dS[2]) *
             (pow2(K[0]) * K[6] + K[0] * K[2] * K[3] - K[0] * K[N] * K[5] -
              pow2(K[1]) * K[6] + K[1] * K[2] * K[5] - K[1] * K[3] * K[N]) +
         (-J[0] * dK[3] + J[1] * dK[5] + J[2] * dK[6] - J[3] * dK[0]) *
             (pow4(K[0]) - pow2(K[0]) * pow2(K[1]) - pow2(K[0]) * pow2(K[2]) -
              pow2(K[0]) * pow2(K[3]) + pow2(K[0]) * pow2(K[N]) +
              pow2(K[0]) * pow2(K[5]) + pow2(K[0]) * pow2(K[6]) -
              pow2(K[1]) * pow2(K[6]) + 2 * K[1] * K[2] * K[5] * K[6] -
              2 * K[1] * K[3] * K[N] * K[6] - pow2(K[2]) * pow2(K[5]) +
              2 * K[2] * K[3] * K[N] * K[5] - pow2(K[3]) * pow2(K[N]))) *
            invden};
  } else if constexpr (N == 3) {
    const double invden = 1.0 / K.inverse_denominator();
    return RadVec<N>{
        (K[0] * (-J[0] * dK[0] - J[1] * dK[1] - J[2] * dK[2]) *
             (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[N])) +
         (pow2(K[0]) + pow2(K[N])) * (A[0] * dB + B * dA[0] + dS[0]) -
         (K[0] * K[1] + K[2] * K[N]) * (A[1] * dB + B * dA[1] + dS[1]) +
         (-K[0] * K[2] + K[1] * K[N]) * (A[2] * dB + B * dA[2] + dS[2])) *
            invden,
        (K[0] * (-J[0] * dK[1] - J[1] * dK[0] - J[2] * dK[N]) *
             (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[N])) +
         (pow2(K[0]) - pow2(K[2])) * (A[1] * dB + B * dA[1] + dS[1]) +
         (-K[0] * K[1] + K[2] * K[N]) * (A[0] * dB + B * dA[0] + dS[0]) +
         (-K[0] * K[N] + K[1] * K[2]) * (A[2] * dB + B * dA[2] + dS[2])) *
            invden,
        (K[0] * (-J[0] * dK[2] + J[1] * dK[N] - J[2] * dK[0]) *
             (pow2(K[0]) - pow2(K[1]) - pow2(K[2]) + pow2(K[N])) +
         (pow2(K[0]) - pow2(K[1])) * (A[2] * dB + B * dA[2] + dS[2]) -
         (K[0] * K[2] + K[1] * K[N]) * (A[0] * dB + B * dA[0] + dS[0]) +
         (K[0] * K[N] + K[1] * K[2]) * (A[1] * dB + B * dA[1] + dS[1])) *
            invden};
  } else if constexpr (N == 2) {
    const double invden = 1.0 / K.inverse_denominator();
    return RadVec<N>{
        (K[0] * (A[0] * dB + B * dA[0] + dS[0]) -
         K[1] * (A[1] * dB + B * dA[1] + dS[1]) +
         (pow2(K[0]) - pow2(K[1])) * (-J[0] * dK[0] - J[1] * dK[1])) *
            invden,
        (K[0] * (A[1] * dB + B * dA[1] + dS[1]) -
         K[1] * (A[0] * dB + B * dA[0] + dS[0]) +
         (pow2(K[0]) - pow2(K[1])) * (-J[0] * dK[1] - J[1] * dK[0])) *
            invden};
  } else if constexpr (N == 1) {
    return RadVec<N>{(A[0] * dB + B * dA[0] - J[0] * K[0] * dK[0] + dS[0]) /
                     K.inverse_denominator()};
  }
}

template <size_t N>
constexpr TraMat<N> transmat(const Absorption::PropMat<N> K,
                             const double r) noexcept {
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
    const double a = -r * K[0], b = -r * K[1], c = -r * K[2], d = -r * K[3],
                 u = -r * K[N], v = -r * K[5], w = -r * K[6];
    const double b2 = b * b, c2 = c * c, d2 = d * d, u2 = u * u, v2 = v * v,
                 w2 = w * w;
    const double exp_a = exp(a);

    const double tmp =
        w2 * w2 + 2 * (b2 * (b2 * 0.5 + c2 + d2 - u2 - v2 + w2) +
                       c2 * (c2 * 0.5 + d2 - u2 + v2 - w2) +
                       d2 * (d2 * 0.5 + u2 - v2 - w2) +
                       u2 * (u2 * 0.5 + v2 + w2) + v2 * (v2 * 0.5 + w2) +
                       4 * (b * d * u * w - b * c * v * w - c * d * u * v));

    const std::complex<double> Const1 = sqrt(std::complex<double>{tmp});
    const double Const2 = b2 + c2 + d2 - u2 - v2 - w2;

    const std::complex<double> x = sqrt(Const2 + Const1) * inv_sqrt_2;
    const std::complex<double> y =
        sqrt(Const2 - Const1) * std::complex<double>{0, inv_sqrt_2};
    const std::complex<double> x2 = x * x;
    const std::complex<double> y2 = y * y;
    const std::complex<double> cy = cos(y);
    const std::complex<double> sy = sin(y);
    const std::complex<double> cx = cosh(x);
    const std::complex<double> sx = sinh(x);

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
    const std::complex<double> ix = x_zero ? 0.0 : 1.0 / x;
    const std::complex<double> iy = y_zero ? 0.0 : 1.0 / y;
    const std::complex<double> inv_x2y2 =
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
    const double a = -r * K[0], b = -r * K[1], c = -r * K[2], u = -r * K[N];
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
    const double A = exp(-r * (K[0] - K[1])) / 2;
    const double B = exp(-r * (K[0] + K[1])) / 2;
    return TraMat<N>{A + B, B - A, B - A, A + B};
  } else if constexpr (N == 1) {
    return TraMat<N>{exp(-K[0] * r)};
  }
}

template <size_t N>
constexpr TraMat<N> dtransmat(const TraMat<N> T, const Absorption::PropMat<N> K,
                              const Absorption::PropMat<N> dK, const double r,
                              const double dr) noexcept {
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
    const double a = -r * K[0], b = -r * K[1], c = -r * K[2], d = -r * K[3],
                 u = -r * K[N], v = -r * K[5], w = -r * K[6];
    const double da = (-r * dK[0] - dr * K[0]), db = (-r * dK[1] - dr * K[1]),
                 dc = (-r * dK[2] - dr * K[2]), dd = (-r * dK[3] - dr * K[3]),
                 du = (-r * dK[N] - dr * K[N]), dv = (-r * dK[5] - dr * K[5]),
                 dw = (-r * dK[6] - dr * K[6]);
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

    const std::complex<double> Const1 = sqrt(std::complex<double>{tmp});
    const double Const2 = b2 + c2 + d2 - u2 - v2 - w2;
    const std::complex<double> dConst1 = 0.5 * dtmp / Const1;
    const double dConst2 = db2 + dc2 + dd2 - du2 - dv2 - dw2;

    const std::complex<double> tmp_x_sqrt = std::sqrt(Const2 + Const1);
    const std::complex<double> tmp_y_sqrt = std::sqrt(Const2 - Const1);
    const std::complex<double> x = tmp_x_sqrt * inv_sqrt_2;
    const std::complex<double> y =
        tmp_y_sqrt * std::complex<double>{0, inv_sqrt_2};
    const std::complex<double> x2 = x * x;
    const std::complex<double> y2 = y * y;
    const std::complex<double> cy = cos(y);
    const std::complex<double> sy = sin(y);
    const std::complex<double> cx = cosh(x);
    const std::complex<double> sx = sinh(x);

    const bool x_zero = std::abs(x) < 1e-4;
    const bool y_zero = std::abs(y) < 1e-4;
    const bool both_zero = y_zero and x_zero;
    const bool either_zero = y_zero or x_zero;

    const std::complex<double> dx =
        x_zero ? 0 : (0.5 * (dConst2 + dConst1) / tmp_x_sqrt) * inv_sqrt_2;
    const std::complex<double> dy =
        y_zero ? 0
               : (0.5 * (dConst2 - dConst1) / tmp_y_sqrt) *
                     std::complex<double>(0, inv_sqrt_2);
    const std::complex<double> dx2 = 2 * x * dx;
    const std::complex<double> dy2 = 2 * y * dy;
    const std::complex<double> dcy = -sy * dy;
    const std::complex<double> dsy = cy * dy;
    const std::complex<double> dcx = sx * dx;
    const std::complex<double> dsx = cx * dx;

    /* Using:
     *    lim x→0 [({cosh(x),cos(x)} - 1) / x^2] → 1/2
     *    lim x→0 [{sinh(x),sin(x)} / x]  → 1
     *    inv_x2 := 1 for x == 0,
     *    -i sin(ix) → sinh(x)
     *    cos(ix) → cosh(x)
     *    C0, C1, C2 ∝ [1/x^2]
     */
    const std::complex<double> ix = x_zero ? 0.0 : 1.0 / x;
    const std::complex<double> iy = y_zero ? 0.0 : 1.0 / y;
    const std::complex<double> inv_x2y2 =
        both_zero
            ? 1.0
            : 1.0 / (x2 + y2);  // The first "1.0" is the trick for above limits
    const std::complex<double> dix = -dx * ix * ix;
    const std::complex<double> diy = -dy * iy * iy;
    const std::complex<double> dx2dy2 = dx2 + dy2;

    const std::complex<double> C0c =
        either_zero ? 1.0 : (cy * x2 + cx * y2) * inv_x2y2;
    const std::complex<double> C1c =
        either_zero ? 1.0 : (sy * x2 * iy + sx * y2 * ix) * inv_x2y2;
    const std::complex<double> C2c = both_zero ? 0.5 : (cx - cy) * inv_x2y2;
    const std::complex<double> C3c =
        both_zero ? 1.0 / 6.0
                  : (x_zero ? 1.0 - sy * iy
                            : y_zero ? sx * ix - 1.0 : sx * ix - sy * iy) *
                        inv_x2y2;
    const std::complex<double> dC0c =
        either_zero
            ? 0.0
            : (dcy * x2 + cy * dx2 + dcx * y2 + cx * dy2 - C0c * dx2dy2) *
                  inv_x2y2;
    const std::complex<double> dC1c =
        either_zero
            ? 0.0
            : (dsy * x2 * iy + sy * dx2 * iy + sy * x2 * diy + dsx * y2 * ix +
               sx * dy2 * ix + sx * y2 * dix - C1c * dx2dy2) *
                  inv_x2y2;
    const std::complex<double> dC2c =
        both_zero ? 0.0 : (dcx - dcy - C2c * dx2dy2) * inv_x2y2;
    const std::complex<double> dC3c =
        both_zero
            ? 0.0
            : ((x_zero ? -dsy * iy - sy * diy
                       : y_zero ? dsx * ix + sx * dix
                                : dsx * ix + sx * dix - dsy * iy - sy * diy) -
               C3c * dx2dy2) *
                  inv_x2y2;

    const double& C0 = reinterpret_cast<const double(&)[2]>(C0c)[0];
    const double& C1 = reinterpret_cast<const double(&)[2]>(C1c)[0];
    const double& C2 = reinterpret_cast<const double(&)[2]>(C2c)[0];
    const double& C3 = reinterpret_cast<const double(&)[2]>(C3c)[0];
    const double& dC0 = reinterpret_cast<const double(&)[2]>(dC0c)[0];
    const double& dC1 = reinterpret_cast<const double(&)[2]>(dC1c)[0];
    const double& dC2 = reinterpret_cast<const double(&)[2]>(dC2c)[0];
    const double& dC3 = reinterpret_cast<const double(&)[2]>(dC3c)[0];

    return TraMat<N>{
        T[0] * da +
            exp_a * (dC0 + dC2 * (b2 + c2 + d2) + C2 * (db2 + dc2 + dd2)),

        T[1] * da +
            exp_a * (db * C1 + b * dC1 + dC2 * (-c * u - d * v) +
                     C2 * (-dc * u - dd * v - c * du - d * dv) +
                     dC3 * (b * (b2 + c2 + d2) - u * (b * u - d * w) -
                            v * (b * v + c * w)) +
                     C3 * (db * (b2 + c2 + d2) - du * (b * u - d * w) -
                           dv * (b * v + c * w) + b * (db2 + dc2 + dd2) -
                           u * (db * u - dd * w) - v * (db * v + dc * w) -
                           u * (b * du - d * dw) - v * (b * dv + c * dw))),

        T[2] * da +
            exp_a * (dC1 * c + C1 * dc + dC2 * (b * u - d * w) +
                     C2 * (db * u - dd * w + b * du - d * dw) +
                     dC3 * (c * (b2 + c2 + d2) - u * (c * u + d * v) -
                            w * (b * v + c * w)) +
                     C3 * (dc * (b2 + c2 + d2) - du * (c * u + d * v) -
                           dw * (b * v + c * w) + c * (db2 + dc2 + dd2) -
                           u * (dc * u + dd * v) - w * (db * v + dc * w) -
                           u * (c * du + d * dv) - w * (b * dv + c * dw))),

        T[3] * da +
            exp_a * (dC1 * d + C1 * dd + dC2 * (b * v + c * w) +
                     C2 * (db * v + dc * w + b * dv + c * dw) +
                     dC3 * (d * (b2 + c2 + d2) - v * (c * u + d * v) +
                            w * (b * u - d * w)) +
                     C3 * (dd * (b2 + c2 + d2) - dv * (c * u + d * v) +
                           dw * (b * u - d * w) + d * (db2 + dc2 + dd2) -
                           v * (dc * u + dd * v) + w * (db * u - dd * w) -
                           v * (c * du + d * dv) + w * (b * du - d * dw))),

        T[4] * da +
            exp_a * (db * C1 + b * dC1 + dC2 * (c * u + d * v) +
                     C2 * (dc * u + dd * v + c * du + d * dv) +
                     dC3 * (-b * (-b2 + u2 + v2) + c * (b * c - v * w) +
                            d * (b * d + u * w)) +
                     C3 * (-db * (-b2 + u2 + v2) + dc * (b * c - v * w) +
                           dd * (b * d + u * w) - b * (-db2 + du2 + dv2) +
                           c * (db * c - dv * w) + d * (db * d + du * w) +
                           c * (b * dc - v * dw) + d * (b * dd + u * dw))),

        T[5] * da +
            exp_a * (dC0 + dC2 * (b2 - u2 - v2) + C2 * (db2 - du2 - dv2)),

        T[6] * da +
            exp_a *
                (dC2 * (b * c - v * w) +
                 C2 * (db * c + b * dc - dv * w - v * dw) + dC1 * u + C1 * du +
                 dC3 * (c * (c * u + d * v) - u * (-b2 + u2 + v2) -
                        w * (b * d + u * w)) +
                 C3 * (dc * (c * u + d * v) - du * (-b2 + u2 + v2) -
                       dw * (b * d + u * w) + c * (dc * u + dd * v) -
                       u * (-db2 + du2 + dv2) - w * (db * d + du * w) +
                       c * (c * du + d * dv) - w * (b * dd + u * dw))),

        T[7] * da +
            exp_a *
                (dC2 * (b * d + u * w) +
                 C2 * (db * d + b * dd + du * w + u * dw) + dC1 * v + C1 * dv +
                 dC3 * (d * (c * u + d * v) - v * (-b2 + u2 + v2) +
                        w * (b * c - v * w)) +
                 C3 * (dd * (c * u + d * v) - dv * (-b2 + u2 + v2) +
                       dw * (b * c - v * w) + d * (dc * u + dd * v) -
                       v * (-db2 + du2 + dv2) + w * (db * c - dv * w) +
                       d * (c * du + d * dv) + w * (b * dc - v * dw))),

        T[8] * da +
            exp_a * (dC1 * c + C1 * dc + dC2 * (-b * u + d * w) +
                     C2 * (-db * u + dd * w - b * du + d * dw) +
                     dC3 * (b * (b * c - v * w) - c * (-c2 + u2 + w2) +
                            d * (c * d - u * v)) +
                     C3 * (db * (b * c - v * w) - dc * (-c2 + u2 + w2) +
                           dd * (c * d - u * v) + b * (db * c - dv * w) -
                           c * (-dc2 + du2 + dw2) + d * (dc * d - du * v) +
                           b * (b * dc - v * dw) + d * (c * dd - u * dv))),

        T[9] * da +
            exp_a *
                (dC2 * (b * c - v * w) +
                 C2 * (db * c + b * dc - dv * w - v * dw) - dC1 * u - C1 * du +
                 dC3 * (-b * (b * u - d * w) + u * (-c2 + u2 + w2) -
                        v * (c * d - u * v)) +
                 C3 * (-db * (b * u - d * w) + du * (-c2 + u2 + w2) -
                       dv * (c * d - u * v) - b * (db * u - dd * w) +
                       u * (-dc2 + du2 + dw2) - v * (dc * d - du * v) -
                       b * (b * du - d * dw) - v * (c * dd - u * dv))),

        T[10] * da +
            exp_a * (dC0 + dC2 * (c2 - u2 - w2) + C2 * (dc2 - du2 - dw2)),

        T[11] * da +
            exp_a *
                (dC2 * (c * d - u * v) +
                 C2 * (dc * d + c * dd - du * v - u * dv) + dC1 * w + C1 * dw +
                 dC3 * (-d * (b * u - d * w) + v * (b * c - v * w) -
                        w * (-c2 + u2 + w2)) +
                 C3 * (-dd * (b * u - d * w) + dv * (b * c - v * w) -
                       dw * (-c2 + u2 + w2) - d * (db * u - dd * w) +
                       v * (db * c - dv * w) - w * (-dc2 + du2 + dw2) -
                       d * (b * du - d * dw) + v * (b * dc - v * dw))),

        T[12] * da +
            exp_a * (dC1 * d + C1 * dd + dC2 * (-b * v - c * w) +
                     C2 * (-db * v - dc * w - b * dv - c * dw) +
                     dC3 * (b * (b * d + u * w) + c * (c * d - u * v) -
                            d * (-d2 + v2 + w2)) +
                     C3 * (db * (b * d + u * w) + dc * (c * d - u * v) -
                           dd * (-d2 + v2 + w2) + b * (db * d + du * w) +
                           c * (dc * d - du * v) - d * (-dd2 + dv2 + dw2) +
                           b * (b * dd + u * dw) + c * (c * dd - u * dv))),

        T[13] * da +
            exp_a *
                (dC2 * (b * d + u * w) +
                 C2 * (db * d + b * dd + du * w + u * dw) - dC1 * v - C1 * dv +
                 dC3 * (-b * (b * v + c * w) - u * (c * d - u * v) +
                        v * (-d2 + v2 + w2)) +
                 C3 * (-db * (b * v + c * w) - du * (c * d - u * v) +
                       dv * (-d2 + v2 + w2) - b * (db * v + dc * w) -
                       u * (dc * d - du * v) + v * (-dd2 + dv2 + dw2) -
                       b * (b * dv + c * dw) - u * (c * dd - u * dv))),

        T[14] * da +
            exp_a *
                (dC2 * (c * d - u * v) +
                 C2 * (dc * d + c * dd - du * v - u * dv) - dC1 * w - C1 * dw +
                 dC3 * (-c * (b * v + c * w) + u * (b * d + u * w) +
                        w * (-d2 + v2 + w2)) +
                 C3 * (-dc * (b * v + c * w) + du * (b * d + u * w) +
                       dw * (-d2 + v2 + w2) - c * (db * v + dc * w) +
                       u * (db * d + du * w) + w * (-dd2 + dv2 + dw2) -
                       c * (b * dv + c * dw) + u * (b * dd + u * dw))),

        T[15] * da +
            exp_a * (dC0 + dC2 * (d2 - v2 - w2) + C2 * (dd2 - dv2 - dw2))};
  } else if constexpr (N == 3) {
    const double a = -r * K[0], b = -r * K[1], c = -r * K[2], u = -r * K[N];
    const double da = -r * dK[0] - dr * K[0], db = -r * dK[1] - dr * K[1],
                 dc = -r * dK[2] - dr * K[2], du = -r * dK[N] - dr * K[N];
    const double exp_a = exp(a);
    const double a2 = a * a, b2 = b * b, c2 = c * c, u2 = u * u;
    const double da2 = 2 * a * da, db2 = 2 * b * db, dc2 = 2 * c * dc,
                 du2 = 2 * u * du;
    const double Const = b2 + c2 - u2;

    // Type of calculations
    const bool real = Const > 0;
    const bool imag = Const < 0;
    const bool either = real or imag;

    const double x =
        sqrt(imag ? -Const : Const);  // test to just use real values
    const double x2 =
        (real ? 1 : -1) * x * x;  // test to change sign if imaginary
    const double inv_x =
        either ? 1.0 / x
               : 1.0;  // test so further calculations are replaced as x→0
    const double inv_x2 = inv_x * inv_x;
    const double dx = either ? ((db2 + dc2 - du2) * inv_x * 0.5) : 0;
    const double dx2 = 2 * x * dx;

    const double sx = real ? sinh(x) : sin(-x);  // -i sin(ix) → sinh(x)
    const double cx = real ? cosh(x) : cos(+x);  //    cos(ix) → cosh(x)
    const double dsx = (real ? 1 : -1) * cx * dx;
    const double dcx = sx * dx;

    /* Using:
     *    lim x→0 [(cosh(x) - 1) / x^2] → 1/2
     *    lim x→0 [sinh(x) / x]  → 1
     *    inv_x2 := 1 for x == 0,
     *    C0, C1, C2 ∝ [1/x^2]
     */
    const double C0 =
        either ? a2 * (cx - 1.0) - a * x * sx + x2 : 1.0 + 0.5 * a2 - a;
    const double dC0 = either
                           ? da2 * (cx - 1.0) + da2 * (dcx - 1.0) -
                                 da * x * sx - a * dx * sx - a * x * dsx + dx2
                           : 0.5 * da2 - da;
    const double C1 = either ? 2.0 * a * (1.0 - cx) + x * sx : 1.0 - a;
    const double dC1 =
        either ? 2.0 * (da * (1.0 - cx) - a * dcx) + dx * sx + x * dsx : -da;
    const double C2 = either ? cx - 1.0 : 0.5;
    const double dC2 = either ? dcx : 0;

    return TraMat<N>{T[0] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC0 + dC1 * a + C1 * da + dC2 * (a2 + b2 + c2) +
                              C2 * (da2 + db2 + dc2)),
                     T[1] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC1 * b + C1 * db + dC2 * (2 * a * b - c * u) +
                              C2 * (2 * da * b - dc * u + 2 * a * db - c * du)),
                     T[2] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC1 * c + C1 * dc + dC2 * (2 * a * c + b * u) +
                              C2 * (2 * da * c + db * u + 2 * a * dc + b * du)),
                     T[3] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC1 * b + C1 * db + dC2 * (2 * a * b + c * u) +
                              C2 * (2 * da * b + dc * u + 2 * a * db + c * du)),
                     T[4] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC0 + dC1 * a + C1 * da + dC2 * (a2 + b2 - u2) +
                              C2 * (da2 + db2 - du2)),
                     T[5] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC1 * u + C1 * du + dC2 * (2 * a * u + b * c) +
                              C2 * (2 * da * u + db * c + 2 * a * du + b * dc)),
                     T[6] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC1 * c + C1 * dc + dC2 * (2 * a * c - b * u) +
                              C2 * (2 * da * c - db * u + 2 * a * dc - b * du)),
                     T[7] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (-dC1 * u - C1 * du - dC2 * (2 * a * u - b * c) -
                              C2 * (2 * da * u - db * c + 2 * a * du - b * dc)),
                     T[8] * (da + dx2 * inv_x2) +
                         exp_a * inv_x2 *
                             (dC0 + dC1 * a + C1 * da + dC2 * (a2 + c2 - u2) +
                              C2 * (da2 + dc2 - du2))};
  } else if constexpr (N == 2) {
    const double dA = (-r * (dK[0] - dK[1]) - dr * (K[0] - K[1])) *
                      exp(-r * (K[0] - K[1])) / 2;
    const double dB = (-r * (dK[0] + dK[1]) - dr * (K[0] + K[1])) *
                      exp(-r * (K[0] + K[1])) / 2;
    return TraMat<N>{dA + dB, dB - dA, dB - dA, dA + dB};
    return TraMat<N>{};
  } else if constexpr (N == 1) {
    const double a = K[0], da = dK[0];
    return TraMat<N>{-(r * da + dr * a) * T[0]};
  }
}
}  // namespace RTE

#endif
