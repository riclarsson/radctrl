#ifndef rational_h
#define rational_h

#include <iostream>
#include <cstdint>
#include <sstream>
#include <string>
#include <numeric>

constexpr long gcd(long a, long b) {
  if (b == 0)
    return a;
  else
    return gcd(b, a%b);
}

class Rational {
private:
  long mn;
  long md;
public:
  constexpr Rational(long n=0, long d=1) : mn(d ? n/gcd(n,d) : 0), md(d ? d/gcd(n,d) : 0) {}
  
  Rational(const std::string& s);
  
  constexpr long n() const {return mn;}
  constexpr long d() const {return md;}
  
  explicit constexpr operator double() {return double(mn)/double(md);}
  
  constexpr bool ok() const noexcept {return md != 0;}
  
  constexpr bool operator==(Rational r) const noexcept {return mn==r.mn and md==r.md;}
  constexpr bool operator!=(Rational r) const noexcept {return not operator==(r);}

  friend constexpr Rational operator+(Rational one, Rational two) 
  {
    return Rational(one.n()*two.d() + two.n()*one.d(), one.d()*two.d());
  }

  friend constexpr Rational operator+(Rational one, long two) 
  {
    return Rational(one.n() + two*one.d(), one.d());
  }

  friend constexpr Rational operator+(long one, Rational two) 
  {
    return operator+(two, one);
  }

  friend constexpr Rational operator*(long one, Rational two) 
  {
    return Rational(one * two.d(), two.d());
  }

  friend constexpr Rational operator*(Rational one, long two) 
  {
    return operator*(two, one);
  }

  friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
    if (r.d() == 1)
      return os << r.n();
    else
      return os << r.n() << '/' << r.d();
  }
  
  friend std::istream& operator>>(std::istream& is, Rational& r) {
    std::string x;
    is >> x;
    r = Rational{x};
    return is;
  }
};

constexpr Rational rational_undefined() noexcept {return Rational{0, 0};}

#endif  // rational_h
