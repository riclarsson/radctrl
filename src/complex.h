#ifndef complex_h
#define complex_h

#include <complex>

#include "constants.h"

using Complex = std::complex<double>;

[[gnu::const]] constexpr double abs2(Complex z) noexcept {
  return Constant::pow2(z.real()) + Constant::pow2(z.imag());
}
[[gnu::const]] constexpr Complex conj(Complex z) {
  return Complex{z.real(), -z.imag()};
}

#define _complex_op_(TYPE)                                                 \
  [[gnu::const]] constexpr Complex operator*(TYPE a, Complex z) noexcept { \
    return Complex{a * z.real(), a * z.imag()};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator*(Complex z, TYPE a) noexcept { \
    return Complex{a * z.real(), a * z.imag()};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator+(TYPE a, Complex z) noexcept { \
    return Complex{a + z.real(), a + z.imag()};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator+(Complex z, TYPE a) noexcept { \
    return Complex{a + z.real(), a + z.imag()};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator-(TYPE a, Complex z) noexcept { \
    return Complex{a - z.real(), a - z.imag()};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator-(Complex z, TYPE a) noexcept { \
    return Complex{z.real() - a, z.imag() - a};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator/(Complex z, TYPE a) noexcept { \
    return Complex{z.real() / a, z.imag() / a};                            \
  }                                                                        \
  [[gnu::const]] constexpr Complex operator/(TYPE a, Complex z) noexcept { \
    return conj(a * z) / abs2(z);                                          \
  }
_complex_op_(double) _complex_op_(int)
#undef _complex_op_

    [[gnu::const]] constexpr Complex
    operator-(Complex z) noexcept {
  return Complex{-z.real(), -z.imag()};
}
[[gnu::const]] constexpr Complex operator+(Complex c, Complex z) noexcept {
  return Complex{c.real() + z.real(), c.imag() + z.imag()};
}
[[gnu::const]] constexpr Complex operator-(Complex c, Complex z) noexcept {
  return Complex{c.real() - z.real(), c.imag() - z.imag()};
}
[[gnu::const]] constexpr Complex operator*(Complex c, Complex z) noexcept {
  return Complex{c.real() * z.real() - c.imag() * z.imag(),
                 c.real() * z.imag() + c.imag() * z.real()};
}
[[gnu::const]] constexpr Complex operator/(Complex c, Complex z) noexcept {
  return c * conj(z) / abs2(z);
}

#endif  // complex_h
