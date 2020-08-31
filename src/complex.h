#ifndef complex_h
#define complex_h

#include <complex>

#include "constants.h"

using Complex = std::complex<double>;

[[gnu::const]] constexpr double abs_squared(Complex z) noexcept {
  return Constant::pow2(z.real()) + Constant::pow2(z.imag());
}
[[gnu::const]] constexpr Complex conj(Complex z) {
  return Complex{z.real(), -z.imag()};
}

#define _complex_op_(TYPE)                                            \
  [[gnu::const]] constexpr Complex operator+(Complex z1, TYPE e) {    \
    return Complex(e + z1.real(), z1.imag());                         \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator-(Complex z1, TYPE e) {    \
    return Complex(-e + z1.real(), z1.imag());                        \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator*(Complex z1, TYPE e) {    \
    return Complex(e * z1.real(), e * z1.imag());                     \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator/(Complex z1, TYPE e) {    \
    return Complex(z1.real() / e, z1.imag() / e);                     \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator+(TYPE e, Complex z1) {    \
    return Complex(e + z1.real(), z1.imag());                         \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator-(TYPE e, Complex z1) {    \
    return Complex(e - z1.real(), -z1.imag());                        \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator*(TYPE e, Complex z1) {    \
    return Complex(e * z1.real(), e * z1.imag());                     \
  }                                                                   \
  [[gnu::const]] constexpr Complex operator/(TYPE e, Complex z1) {    \
    return Complex(                                                   \
        e * z1.real() /                                               \
            (Constant::pow2(z1.imag()) + Constant::pow2(z1.real())),  \
        -e * z1.imag() /                                              \
            (Constant::pow2(z1.imag()) + Constant::pow2(z1.real()))); \
  }
_complex_op_(double) _complex_op_(int)
#undef _complex_op_

    [[gnu::const]] constexpr Complex
    operator-(Complex z1) noexcept {
  return Complex{-z1.real(), -z1.imag()};
}
[[gnu::const]] constexpr Complex operator+(Complex z1, Complex z2) {
  return Complex(z1.real() + z2.real(), z1.imag() + z2.imag());
}
[[gnu::const]] constexpr Complex operator-(Complex z1, Complex z2) {
  return Complex(z1.real() - z2.real(), z1.imag() - z2.imag());
}
[[gnu::const]] constexpr Complex operator*(Complex z1, Complex z2) {
  return Complex(-z1.imag() * z2.imag() + z1.real() * z2.real(),
                 z1.imag() * z2.real() + z1.real() * z2.imag());
}
[[gnu::const]] constexpr Complex operator/(Complex z1, Complex z2) {
  return Complex(
      z1.imag() * z2.imag() /
              (Constant::pow2(z2.imag()) + Constant::pow2(z2.real())) +
          z1.real() * z2.real() /
              (Constant::pow2(z2.imag()) + Constant::pow2(z2.real())),
      z1.imag() * z2.real() /
              (Constant::pow2(z2.imag()) + Constant::pow2(z2.real())) -
          z1.real() * z2.imag() /
              (Constant::pow2(z2.imag()) + Constant::pow2(z2.real())));
}

#endif  // complex_h
