#include "xsec_lbl.h"

#include <execution>

namespace Absorption {
namespace Xsec {
namespace Lbl {
template <class LineShape>
void compute_lineshape(std::vector<Complex>& comp_x,
                       const std::vector<double>& f, const Complex lm,
                       Frequency<FrequencyType::Freq> cutoff_low,
                       Frequency<FrequencyType::Freq> cutoff_upp,
                       LineShape ls) {
  const size_t nv = f.size();
  for (size_t iv = 0; iv < nv; iv++) {
    if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) {
      comp_x[iv] = lm * ls(f[iv]);
    } else {
      comp_x[iv] = Complex(0, 0);
    }
  }

  if (cutoff_upp > 0.0) {
    ls(cutoff_upp);
    for (size_t iv = 0; iv < nv; iv++) {
      if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) comp_x[iv] -= lm * ls.F;
    }
  }
}

template <class LineShape>
void compute_mirrored_lineshape(std::vector<Complex>& comp_x,
                                const std::vector<double>& f, const Complex lm,
                                Frequency<FrequencyType::Freq> cutoff_low,
                                Frequency<FrequencyType::Freq> cutoff_upp,
                                LineShape ls) {
  const size_t nv = f.size();
  for (size_t iv = 0; iv < nv; iv++) {
    if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp)
      comp_x[iv] += lm * conj(ls(f[iv]));
  }

  if (cutoff_upp > 0.0) {
    ls(cutoff_upp);
    for (size_t iv = 0; iv < nv; iv++) {
      if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp)
        comp_x[iv] -= lm * conj(ls.F);
    }
  }
}

double boltzman_ratio(double T, double T0, double E0) {
  return std::exp(E0 * (T - T0) / (Constant::k * T * T0));
}

double stimulated_emission(double T, double F0) {
  return std::exp(-(Constant::h / Constant::k) * F0 / T);
}

constexpr double stimulated_relative_emission(double gamma, double gamma_ref) {
  return (1. - gamma) / (1. - gamma_ref);
}

double compute_lte_linestrength(double S0, double SZ, double E0, double F0,
                                double QT0, double T0, double QT, double T) {
  return SZ * S0 * boltzman_ratio(T, T0, E0) *
         stimulated_relative_emission(stimulated_emission(T, F0),
                                      stimulated_emission(T0, F0)) *
         QT0 / QT;
}

std::pair<double, double> compute_nlte_linestrength(double SZ, double F0,
                                                    double Gl, double Gu,
                                                    double A, double r2,
                                                    double r1, double T) {
  constexpr double c0 = 2.0 * Constant::h / Constant::pow2(Constant::c);
  constexpr double c1 = Constant::h / (4 * Constant::pi);

  const double c2 = c0 * Constant::pow3(F0);
  const double c3 = c1 * F0;
  const double k = c3 * (r1 * Gl / Gu - r2) * (A / c2);
  const double e = c3 * r2 * A;
  const double exp_T = std::exp(Constant::h / Constant::k * F0 / T);
  const double b = c2 / (exp_T - 1);
  const double ratio = e / b - k;

  return {SZ * k, SZ * ratio};
}

void compute(Results& res, Results& src, Results& comp,
             const std::vector<double>& f, const Band& band,
             const Path::Point& atm, const Polarization polarization) {
  const double H = atm.atm.MagField().Strength();
  const double GDpart = band.GD_giv_F0(atm.atm.Temp());
  const double QT0 = band.QT0();
  const double QT = band.QT(atm.atm.Temp());
  const double vmr = atm.atm.VolumeMixingRatio(band.Isotopologue());

  // Skip Zeeman copies if Zeeman or without
  if (band.doZeeman() and polarization == Polarization::None)
    return;
  else if (not band.doZeeman() and polarization not_eq Polarization::None)
    return;

  for (size_t iline = 0; iline < band.n_lines(); iline++) {
    const auto& line = band.Lines()[iline];
    const auto zeeman_range =
        line.ZeemanRange(polarization, band.Isotopologue());
    const auto X = line.ShapeModel()(atm.atm.Temp(), band.T0(), atm.atm.Pres(),
                                     atm.atm.VolumeMixingRatios());
    const Complex lm{1.0 + X.G, -X.Y};

    for (int iz = zeeman_range.first; iz <= zeeman_range.second; iz++) {
      const auto DZ =
          line.ZeemanSplitting(polarization, band.Isotopologue(), iz);
      const auto SZ =
          line.ZeemanStrength(polarization, band.Isotopologue(), iz);
      const auto cutoff_low = band.CutoffLower(iline);
      const auto cutoff_upp = band.CutoffUpper(iline);

      // Forward Line Shape
      switch (band.ShapeType()) {
        case Shape::DP:
          compute_lineshape(
              comp.x, f, lm, cutoff_low, cutoff_upp,
              LineShape::Base::Doppler(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::LP:
          compute_lineshape(
              comp.x, f, lm, cutoff_low, cutoff_upp,
              LineShape::Base::Lorentz(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::VP:
          compute_lineshape(
              comp.x, f, lm, cutoff_low, cutoff_upp,
              LineShape::Base::Voigt(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::SDVP:
          compute_lineshape(comp.x, f, lm, cutoff_low, cutoff_upp,
                            LineShape::Base::SpeedDependentVoigt(
                                line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::SDHCVP:
          compute_lineshape(comp.x, f, lm, cutoff_low, cutoff_upp,
                            LineShape::Base::SpeedDependentHardCollisionVoigt(
                                line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::HTP:
          compute_lineshape(
              comp.x, f, lm, cutoff_low, cutoff_upp,
              LineShape::Base::HartmannTran(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::FINAL: { /* leave last */
        }
      }

      // Mirrored Line Shape
      switch (band.MirrorType()) {
        case Mirroring::Same:
          switch (band.ShapeType()) {
            case Shape::DP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::Doppler(-line.F0(), mirrored(X), -GDpart,
                                           DZ * H));
              break;
            case Shape::LP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::Lorentz(-line.F0(), mirrored(X), -GDpart,
                                           DZ * H));
              break;
            case Shape::VP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::Voigt(-line.F0(), mirrored(X), -GDpart,
                                         DZ * H));
              break;
            case Shape::SDVP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::SpeedDependentVoigt(-line.F0(), mirrored(X),
                                                       -GDpart, DZ * H));
              break;
            case Shape::SDHCVP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::SpeedDependentHardCollisionVoigt(
                      -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::HTP:
              compute_mirrored_lineshape(
                  comp.x, f, lm, cutoff_low, cutoff_upp,
                  LineShape::Base::HartmannTran(-line.F0(), mirrored(X),
                                                -GDpart, DZ * H));
              break;
            case Shape::FINAL: { /* leave last */
            }
          }
          break;
        case Mirroring::Lorentz:
          compute_mirrored_lineshape(
              comp.x, f, lm, cutoff_low, cutoff_upp,
              LineShape::Base::Lorentz(-line.F0(), mirrored(X), -GDpart,
                                       DZ * H));
          break;
        case Mirroring::None:
        case Mirroring::FINAL: { /* leave last */
        }
      }

      // Normalized Line Shape
      switch (band.NormType()) {
        case Normalization::VVH:
          //           apply_VVH_scaling(F, dF, data, f, band.F0(i), T, band, i,
          //           derivatives_data, derivatives_data_active);
          break;
        case Normalization::VVW:
          //           apply_VVW_scaling(F, dF, f, band.F0(i), band, i,
          //           derivatives_data, derivatives_data_active);
          break;
        case Normalization::RosenkranzQuadratic:
          //           apply_rosenkranz_quadratic_scaling(F, dF, f, band.F0(i),
          //           T, band, i, derivatives_data, derivatives_data_active);
          break;
        case Normalization::None:
        case Normalization::FINAL: { /* leave last */
        }
      }

      // Apply line strength by whatever method is necessary
      switch (band.PopType()) {
        case Population::ByLTE: {
          const double S = vmr * compute_lte_linestrength(
                                     line.I0(), SZ, line.E0(), line.F0(), QT0,
                                     band.T0(), QT, atm.atm.Temp());
          std::transform(comp.x.cbegin(), comp.x.cend(), res.x.cbegin(),
                         res.x.begin(),
                         [S](auto& a, auto& b) { return S * a + b; });
        } break;
        case Population::ByNLTE: {
          double r1 = 0, r2 = 0;  // FIXME: Input the correct numbers when
                                  // operating in NLTE mode

          const std::pair<double, double> S =
              compute_nlte_linestrength(SZ, line.F0(), line.Gl(), line.Gu(),
                                        line.A(), r2, r1, atm.atm.Temp());

          std::transform(comp.x.cbegin(), comp.x.cend(), res.x.cbegin(),
                         res.x.begin(),
                         [k = S.first](auto& a, auto& b) { return k * a + b; });

          std::transform(
              comp.x.cbegin(), comp.x.cend(), src.x.cbegin(), src.x.begin(),
              [r = S.second](auto& a, auto& b) { return r * a + b; });
        } break;
        case Population::FINAL: { /* leave last */
        }
      }
    }
  }
}
}  // namespace Lbl
}  // namespace Xsec
}  // namespace Absorption
