#ifndef zeeman_h
#define zeeman_h

#include <array>
#include <cmath>

#include "constants.h"
#include "rational.h"
#include "units.h"
#include "wigner.h"

namespace Absorption {
/** Polarization selection */
enum class Polarization : char { SigmaMinus, Pi, SigmaPlus, None };

namespace Zeeman {
/** Gives the change of M given a polarization type
 *
 * @param[in] type The polarization type
 *
 * @return The change in M
 */
constexpr int dM(Polarization type) noexcept {
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
constexpr Rational start(Rational Ju, Rational Jl, Polarization type) noexcept {
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
constexpr Rational end(Rational Ju, Rational Jl, Polarization type) noexcept {
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
constexpr Rational Mu(Rational Ju, Rational Jl, Polarization type,
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
constexpr Rational Ml(Rational Ju, Rational Jl, Polarization type,
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
constexpr double PolarizationFactor(Polarization type) noexcept {
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

struct CalcSingle {
  double H;
  double theta;
  double eta;
};

struct Calc {
  CalcSingle calc;
  CalcSingle du;
  CalcSingle dv;
  CalcSingle dw;
};

Calc angles_with_derivatives(Magnetism<MagnetismType::T> mag,
                             Angle<AngleType::deg> z,
                             Angle<AngleType::deg> a) noexcept;

template <Polarization pol>
constexpr std::array<double, 7> PolarizationVector(CalcSingle a) noexcept {
  using namespace Conversion;
  [[maybe_unused]] const double ST = sind(a.theta), CT = cosd(a.theta),
                                ST2 = ST * ST, CT2 = CT * CT,
                                ST2C2E = ST2 * cosd(2 * a.eta),
                                ST2S2E = ST2 * sind(2 * a.eta);

  if constexpr (pol == Polarization::SigmaMinus)
    return {1 + CT2, ST2C2E, ST2S2E, 2 * CT, 4 * CT, 2 * ST2S2E, -2 * ST2C2E};
  else if constexpr (pol == Polarization::Pi)
    return {ST2, -ST2C2E, -ST2S2E, 0, 0, -2 * ST2S2E, 2 * ST2C2E};
  else if constexpr (pol == Polarization::SigmaPlus)
    return {1 + CT2, ST2C2E, ST2S2E, -2 * CT, -4 * CT, 2 * ST2S2E, -2 * ST2C2E};
  else if constexpr (pol == Polarization::None)
    return {1, 0, 0, 0, 0, 0};
  else
    std::terminate();
}

template <Polarization pol>
constexpr std::array<double, 7> PolarizationVectorEtaDerivative(
    CalcSingle a) noexcept {
  using namespace Conversion;
  [[maybe_unused]] const double ST = sind(a.theta), ST2 = ST * ST,
                                C2E = cosd(2 * a.eta), S2E = sind(2 * a.eta),
                                dST2C2E = -2 * ST2 * S2E,
                                dST2S2E = 2 * ST2 * C2E;

  if constexpr (pol == Polarization::SigmaMinus)
    return {0, dST2C2E, dST2S2E, 0, 0, 2 * dST2S2E, -2 * dST2C2E};
  else if constexpr (pol == Polarization::Pi)
    return {0, -dST2C2E, -dST2S2E, 0, 0, -2 * dST2S2E, 2 * dST2C2E};
  else if constexpr (pol == Polarization::SigmaPlus)
    return {0, dST2C2E, dST2S2E, 0, 0, 2 * dST2S2E, -2 * dST2C2E};
  else if constexpr (pol == Polarization::None)
    return {0, 0, 0, 0, 0, 0};
  else
    std::terminate();
}

template <Polarization pol>
constexpr std::array<double, 7> PolarizationVectorThetaDerivative(
    CalcSingle a) noexcept {
  using namespace Conversion;
  [[maybe_unused]] const double ST = sind(a.theta), CT = cosd(a.theta),
                                C2E = cosd(2 * a.eta), S2E = sind(2 * a.eta),
                                dST = CT, dST2 = 2 * ST * dST, dCT = -ST,
                                dST2C2E = dST2 * C2E, dST2S2E = dST2 * S2E,
                                dCT2 = 2 * CT * dCT;

  if constexpr (pol == Polarization::SigmaMinus)
    return {dCT2,    dST2C2E,     dST2S2E,     2 * dCT,
            4 * dCT, 2 * dST2S2E, -2 * dST2C2E};
  else if constexpr (pol == Polarization::Pi)
    return {dST2, -dST2C2E, -dST2S2E, 0, 0, -2 * dST2S2E, 2 * dST2C2E};
  else if constexpr (pol == Polarization::SigmaPlus)
    return {dCT2,     dST2C2E,     dST2S2E,     -2 * dCT,
            -4 * dCT, 2 * dST2S2E, -2 * dST2C2E};
  else if constexpr (pol == Polarization::None)
    return {0, 0, 0, 0, 0, 0};
  else
    std::terminate();
}

struct Model {
  double gu, gl;
  constexpr Model(double u = 0, double l = 0) noexcept : gu(u), gl(l) {}
  friend std::ostream &operator<<(std::ostream &os, Model z) {
    return os << z.gu << ' ' << z.gl;
  }
  friend std::istream &operator>>(std::istream &os, Model &z) {
    return os >> z.gu >> z.gl;
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
   * @return The relative strength of the Zeeman sub-line
   */
  double Strength(Rational Ju, Rational Jl, Polarization type,
                  int n) const noexcept {
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
   * @return The splitting of the Zeeman sub-line
   */
  constexpr double Splitting(Rational Ju, Rational Jl, Polarization type,
                             int n) const noexcept {
    using Constant::bohr_magneton;
    using Constant::h;
    constexpr double C = bohr_magneton / h;

    return C * (double(Ml(Ju, Jl, type, n)) * gl -
                double(Mu(Ju, Jl, type, n)) * gu);
  }
};
}  // namespace Zeeman
}  // namespace Absorption

#endif  // zeeman_h
