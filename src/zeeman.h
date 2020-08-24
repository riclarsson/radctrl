#ifndef zeeman_h
#define zeeman_h

#include <array>
#include <cmath>

#include "constants.h"
#include "rational.h"
#include "wigner.h"

namespace Absorption {
/** Polarization selection */
enum class Polarization : char { SigmaMinus, Pi, SigmaPlus, None };

struct Zeeman {
  double gu, gl;
  constexpr Zeeman(double u = 0, double l = 0) noexcept : gu(u), gl(l) {}
  friend std::ostream& operator<<(std::ostream& os, Zeeman z) {
    return os << z.gu << ' ' << z.gl;
  }
  friend std::istream& operator>>(std::istream& os, Zeeman& z) {
    return os >> z.gu >> z.gl;
  }

  /** Gives the change of M given a polarization type
   *
   * @param[in] type The polarization type
   *
   * @return The change in M
   */
  static constexpr int dM(Polarization type) noexcept {
    switch (type) {
      case Polarization::SigmaMinus:
        return -1;
      case Polarization::None:
      case Polarization::Pi:
        return 0;
      case Polarization::SigmaPlus:
        return 1;
    }
    return std::numeric_limits<int>::max();
  }

  /** Gives the lowest M for a polarization type of this transition
   *
   * Since the polarization determines the change in M, this
   * function gives the first M of interest in the range of M
   * possible for a given transition
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   *
   * @return The lowest M value
   */
  static constexpr Rational start(Rational Ju, Rational Jl,
                                  Polarization type) noexcept {
    switch (type) {
      case Polarization::SigmaMinus:
        if (Ju < Jl)
          return -Ju;
        else if (Ju == Jl)
          return -Ju + 1;
        else
          return -Ju + 2;
      case Polarization::Pi:
        return -std::min(Ju, Jl);
      case Polarization::SigmaPlus:
        return -Ju;
      case Polarization::None:
        return 0;
    }
    return Rational(std::numeric_limits<int>::max());
  }

  /** Gives the largest M for a polarization type of this transition
   *
   * Since the polarization determines the change in M, this
   * function gives the last M of interest in the range of M
   * possible for a given transition
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   *
   * @return The largest M value
   */
  static constexpr Rational end(Rational Ju, Rational Jl,
                                Polarization type) noexcept {
    switch (type) {
      case Polarization::SigmaMinus:
        return Ju + 1;
      case Polarization::Pi:
        return std::min(Ju, Jl);
      case Polarization::SigmaPlus:
        if (Ju < Jl)
          return Ju + 1;
        else if (Ju == Jl)
          return Ju;
        else
          return Jl;
      case Polarization::None:
        return 0;
    }
    return Rational(std::numeric_limits<int>::max());
  }

  /** Gives the upper state M value at an index
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * The user has to ensure n is less than the number of elements
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   * @param[in] n The position
   *
   * @return The upper state M
   */
  static constexpr Rational Mu(Rational Ju, Rational Jl, Polarization type,
                               int n) noexcept {
    return start(Ju, Jl, type) + n;
  }

  /** Gives the lower state M value at an index
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * The user has to ensure n is less than the number of elements
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   * @param[in] n The position
   *
   * @return The lower state M
   */
  static constexpr Rational Ml(Rational Ju, Rational Jl, Polarization type,
                               int n) noexcept {
    return Mu(Ju, Jl, type, n) + dM(type);
  }

  /** The renormalization factor of a polarization type
   *
   * The polarization comes from some geometry.  This function
   * returns the factor we need to compute that geometry and to
   * turn it into something that normalizes every possible M
   * for this type into some strength that sums to unity
   *
   * @param[in] type The polarization type
   *
   * @return Rescale factor
   */
  static constexpr double PolarizationFactor(Polarization type) noexcept {
    switch (type) {
      case Polarization::SigmaMinus:
        return .75;
      case Polarization::Pi:
        return 1.5;
      case Polarization::SigmaPlus:
        return .75;
      case Polarization::None:
        return 1.0;
    }
    return std::numeric_limits<double>::max();
  }

  /** Gives the strength of one subline of a given polarization
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * The user has to ensure n is less than the number of elements
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   * @param[in] n The position
   *
   * @return The relative strength of the Zeeman subline
   */
  double Strength(Rational Ju, Rational Jl, Polarization type, int n) const {
    using Constant::pow2;

    auto ml = Ml(Ju, Jl, type, n);
    auto mu = Mu(Ju, Jl, type, n);
    auto dm = Rational(dM(type));
    return PolarizationFactor(type) *
           pow2(wigner3j(Jl, Rational(1), Ju, ml, -dm, -mu));
  }

  /** Gives the splitting of one subline of a given polarization
   *
   * The user has to ensure that Ju and Jl is a valid transition
   *
   * The user has to ensure n is less than the number of elements
   *
   * @param[in] Ju J of the upper state
   * @param[in] Jl J of the upper state
   * @param[in] type The polarization type
   * @param[in] n The position
   *
   * @return The splitting of the Zeeman subline
   */
  constexpr double Splitting(Rational Ju, Rational Jl, Polarization type,
                             int n) const noexcept {
    using Constant::bohr_magneton;
    using Constant::h;
    constexpr double C = bohr_magneton / h;

    return C * (double(Ml(Ju, Jl, type, n)) * gl -
                double(Mu(Ju, Jl, type, n)) * gu);
  }

  struct vec3 {
    double x, y, z;
    constexpr vec3(double mx, double my, double mz) noexcept
        : x(mx), y(my), z(mz) {}
    static std::pair<vec3, double> norm(double mx, double my, double mz) {
      const double d = std::hypot(mx, my, mz);
      return {vec3{mx / d, my / d, mz / d}, d};
    }
    constexpr double dot(const vec3 o) const {
      return o.x * x + o.y * y + o.z * z;
    }
    constexpr vec3 cross(const vec3 o) const {
      return vec3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }
    friend constexpr vec3 operator*(double m, vec3 v) {
      return {m * v.x, m * v.y, m * v.z};
    }
    constexpr vec3 operator-(vec3 v) const {
      return {x - v.x, y - v.y, z - v.z};
    }
  };

  struct Angles {
    double theta;
    double eta;
  };

  static inline Angles angles(double u, double v, double w, double z,
                              double a) noexcept {
    using Conversion::cosd;
    using Conversion::sind;

    // XYZ vectors normalized
    const vec3 n{cosd(a) * sind(z), sind(a) * sind(z), cosd(z)};
    const vec3 ev{cosd(a) * cosd(z), sind(a) * cosd(z), -sind(z)};
    const auto normalized = vec3::norm(u, v, w);
    const vec3 inplane = normalized.first - normalized.first.dot(n) * n;

    if (normalized.second == 0)
      return {0, 0};
    else
      return {Conversion::acosd(n.dot(normalized.first)),
              Conversion::atan2d(ev.dot(inplane), ev.cross(inplane).dot(n))};
  }

  template <Polarization pol>
  static constexpr std::array<double, 7> PolarizationVector(Angles a) {
    using namespace Conversion;
    const double ST = sind(a.theta), CT = cosd(a.theta), ST2 = ST * ST,
                 CT2 = CT * CT, ST2C2E = ST2 * cosd(2 * a.eta),
                 ST2S2E = ST2 * sind(2 * a.eta);

    if constexpr (pol == Polarization::SigmaMinus)
      return {1 + CT2, ST2C2E, ST2S2E, 2 * CT, 4 * CT, 2 * ST2S2E, -2 * ST2C2E};
    else if constexpr (pol == Polarization::Pi)
      return {ST2, -ST2C2E, -ST2S2E, 0, 0, -2 * ST2S2E, 2 * ST2C2E};
    else if constexpr (pol == Polarization::SigmaPlus)
      return {1 + CT2, ST2C2E,     ST2S2E,     -2 * CT,
              -4 * CT, 2 * ST2S2E, -2 * ST2C2E};
    else if constexpr (pol == Polarization::None)
      return {1, 0, 0, 0, 0, 0};
    else
      std::exit(1);
  }
};
}  // namespace Absorption

#endif  // zeeman_h
