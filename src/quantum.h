#ifndef quantum_h
#define quantum_h

#include <array>
#include <cctype>
#include <string>

#include "enums.h"
#include "rational.h"

namespace Quantum {

ENUMCLASS(Type, unsigned char, J, N, S, F, K, Ka, Kc, N1, L, v, v1, v2, v3, v4,
          v5, v6, v7, v8, v9, v10, v11, v12, l, l2, l3, l4, l5, l6, l7, l8, l9,
          l10, l11, l12, Lambda, Omega, rovibSym, n, alpha, tau, r, config,
          term, ElecStateLabel, kronigParity, parity, vibSym, vibInv, rotSym,
          vibRefl)

class Number {
  static constexpr unsigned char N = 15;
  std::array<char, N> text;
  bool istext;
  Rational value;

 public:
  constexpr Number() noexcept
      : text({' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
              ' ', ' '}),
        istext(false),
        value(rational_undefined()) {}
  constexpr Number(std::array<char, Number::N> t) noexcept
      : text(t), istext(true), value(rational_undefined()) {}
  constexpr Number(Rational v) noexcept
      : text({' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
              ' ', ' '}),
        istext(false),
        value(v) {}
  Number(const std::string &val) noexcept
      : text({' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
              ' ', ' '}),
        istext(false),
        value(rational_undefined()) {
    if (val.size() and (std::isdigit(val[0]) or val[0] == '.')) {
      istext = false;
      value = Rational(val);
    } else if (val.size()) {
      istext = true;
      auto n = val.size();
      for (size_t i = 0; i < n; i++) {
        if (not std::isspace(val[i]))
          text[Number::N - n + i] = val[i];
        else {
          n--;
        }
      }
    }
  }

  friend std::ostream &operator<<(std::ostream &os, Number n) {
    if (n.istext) {
      for (char t : n.text) {
        if (not std::isspace(t)) {
          os << t;
        }
      }
      return os;
    } else {
      return os << n.value;
    }
  }

  constexpr bool operator==(Number n) const noexcept {
    if (n.istext != istext)
      return false;
    else if (istext)
      return n.text == text;
    else
      return n.value == value;
  }

  friend std::istream &operator>>(std::istream &is, Number &n) {
    std::string x;
    is >> x;
    n = Number(x);
    return is;
  }

  constexpr bool defined() const noexcept {
    return istext or value != rational_undefined();
  }

  constexpr Rational rat() const noexcept { return value; }
};  // Number
}  // namespace Quantum

#endif  // quantum_h
