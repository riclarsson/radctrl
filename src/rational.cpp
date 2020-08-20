#include "rational.h"

#include <algorithm>

/** Rational from Numeric
 *
 * Performs basic rounding
 *
 * @param[in] x Numeric value
 * @param[in] maxdec Maximum number of decimals
 */
constexpr Rational numeric2rational(double x, size_t maxdec = 4) {
  long nom = 0, denom = 1;

  // Keep track of sign independently
  const bool signchange = x < 0;
  x = x < 0 ? -x : x;

  // Add numbers by keeping the floor
  size_t i = 0;
  do {
    const long xi = long(x);
    nom += xi;
    x = 10 * (x - double(xi));
    nom *= 10;
    denom *= 10;
    i++;
  } while (i <= maxdec);

  // Fix possible rounding error
  if (x >= 5) nom += 10;

  // Change sign or not
  if (signchange)
    return Rational(-nom, denom);
  else
    return Rational(nom, denom);
}

Rational::Rational(const std::string& s) {
  if (s.cend() not_eq s.cbegin()) {
    if (auto dot_pos = std::find(s.cbegin(), s.end(), '.');
        s.cend() > dot_pos) {
      *this = numeric2rational(std::stod(s), s.cend() - dot_pos - 1);
    } else if (auto slash_pos = std::find(s.cbegin(), s.end(), '/');
               s.cend() > slash_pos) {
      *this = Rational(std::stol(std::string{s.cbegin(), slash_pos}),
                       std::stol(std::string{slash_pos + 1, s.cend()}));
    } else {
      *this = Rational(std::stoi(s));
    }
  } else {
    *this = rational_undefined();
  }
}
