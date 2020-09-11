#include "xsec_lbl.h"

#include <execution>

namespace Absorption {
namespace Xsec {
namespace Lbl {
union ComputedDerivData {
  double d;
  LineShape::Output lso;
  ComputedDerivData() noexcept : d(0) {}
};

#define LINESHAPEDERIVATIVES(PARAM)                           \
  else if (derivs[i] == Derivative::Line::Shape##PARAM##X0) { \
    out[i].d = lsmod.dX0(T, T0, P, vmr, derivs[i].Species(),  \
                         LineShape::Parameter::PARAM);        \
  }                                                           \
  else if (derivs[i] == Derivative::Line::Shape##PARAM##X1) { \
    out[i].d = lsmod.dX1(T, T0, P, vmr, derivs[i].Species(),  \
                         LineShape::Parameter::PARAM);        \
  }                                                           \
  else if (derivs[i] == Derivative::Line::Shape##PARAM##X2) { \
    out[i].d = lsmod.dX2(T, T0, P, vmr, derivs[i].Species(),  \
                         LineShape::Parameter::PARAM);        \
  }                                                           \
  else if (derivs[i] == Derivative::Line::Shape##PARAM##X3) { \
    out[i].d = lsmod.dX3(T, T0, P, vmr, derivs[i].Species(),  \
                         LineShape::Parameter::PARAM);        \
  }

void update_derivatives(std::vector<ComputedDerivData> &cpp,
                        const std::vector<Derivative::Target> &derivs,
                        const Frequency<FrequencyType::Freq> dZ) noexcept {
  const std::size_t n = derivs.size();
  for (std::size_t i = 0; i < n; i++) {
    if (derivs[i].isMagnetism()) {
      cpp[i].d = dZ;
    }
  }
}

std::vector<ComputedDerivData> process_derivatives(
    const std::vector<Derivative::Target> &derivs,
    const Absorption::LineShape::Model &lsmod,
    Temperature<TemperatureType::K> T, Temperature<TemperatureType::K> T0,
    Pressure<PressureType::Pa> P,
    const std::vector<VMR<VMRType::ratio>> &vmr) noexcept {
  const std::size_t n = derivs.size();

  std::vector<ComputedDerivData> out(n);

  for (std::size_t i = 0; i < n; i++) {
    if (derivs[i] == Derivative::Atm::Temperature) {
      out[i].lso = lsmod.dT(T, T0, P, vmr);
    } else if (derivs[i] == Derivative::Atm::VMR) {
      out[i].lso = out[i].lso = lsmod.dVMR(T, T0, P, vmr, derivs[i].Species());
    }
    LINESHAPEDERIVATIVES(G0)
    LINESHAPEDERIVATIVES(D0)
    LINESHAPEDERIVATIVES(G2)
    LINESHAPEDERIVATIVES(D2)
    LINESHAPEDERIVATIVES(FVC)
    LINESHAPEDERIVATIVES(ETA)
    LINESHAPEDERIVATIVES(Y)
    LINESHAPEDERIVATIVES(G)
    LINESHAPEDERIVATIVES(DV)
  }

  return out;
}

#undef LINESHAPEDERIVATIVES

template <class LineShape>
void compute_lineshape(std::vector<Complex> &comp_x,
                       std::vector<std::vector<Complex>> &comp_dx,
                       const std::vector<Frequency<FrequencyType::Freq>> &f,
                       const std::vector<Derivative::Target> &derivs,
                       const std::vector<ComputedDerivData> &cdd,
                       const Complex lm, Temperature<TemperatureType::K> T,
                       std::size_t this_line,
                       Frequency<FrequencyType::Freq> cutoff_low,
                       Frequency<FrequencyType::Freq> cutoff_upp,
                       LineShape ls) {
  const size_t nv = f.size();
  const size_t nd = derivs.size();

  for (size_t iv = 0; iv < nv; iv++) {
    if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) {
      comp_x[iv] = lm * ls(f[iv]);
      for (size_t id = 0; id < nd; id++) {
        if (derivs[id] == Derivative::Atm::Temperature) {
          comp_dx[id][iv] = lm * ls.dFdT(cdd[id].lso, T) +
                            linemixing_derivative(cdd[id].lso) * ls.F;
        } else if (derivs[id].isWind()) {
          comp_dx[id][iv] = lm * ls.dFdf();  // WARNING: Not true derivative
        } else if (derivs[id].isMagnetism()) {
          comp_dx[id][iv] = lm * ls.dFdH(cdd[id].d);
        } else if (derivs[id] == Derivative::Atm::VMR and
                   isnonzero(cdd[id].lso)) {
          comp_dx[id][iv] = lm * ls.dFdVMR(cdd[id].lso) +
                            linemixing_derivative(cdd[id].lso) * ls.F;
        } else if (derivs[id].isLineCenter(this_line)) {
          comp_dx[id][iv] = lm * ls.dFdF0();
        } else if (derivs[id].isshapeG0()) {
          comp_dx[id][iv] = lm * ls.dFdG0(cdd[id].d);
        } else if (derivs[id].isshapeD0()) {
          comp_dx[id][iv] = lm * ls.dFdD0(cdd[id].d);
        } else if (derivs[id].isshapeG2()) {
          comp_dx[id][iv] = lm * ls.dFdG2(cdd[id].d);
        } else if (derivs[id].isshapeD2()) {
          comp_dx[id][iv] = lm * ls.dFdD2(cdd[id].d);
        } else if (derivs[id].isshapeFVC()) {
          comp_dx[id][iv] = lm * ls.dFdFVC(cdd[id].d);
        } else if (derivs[id].isshapeETA()) {
          comp_dx[id][iv] = lm * ls.dFdETA(cdd[id].d);
        } else if (derivs[id].isshapeY()) {
          comp_dx[id][iv] = Complex(0, -cdd[id].d) * ls.F;
        } else if (derivs[id].isshapeG()) {
          comp_dx[id][iv] = cdd[id].d * ls.F;
        } else if (derivs[id].isshapeDV()) {
          comp_dx[id][iv] = lm * ls.dFdDV(cdd[id].d);
        }
      }
    } else {
      comp_x[iv] = Complex(0, 0);
      for (size_t id = 0; id < nd; id++) {
        comp_dx[id][iv] = Complex(0, 0);
      }
    }
  }

  if (cutoff_upp > 0.0) {
    ls(cutoff_upp);
    for (size_t iv = 0; iv < nv; iv++) {
      if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) {
        comp_x[iv] -= lm * ls.F;
        for (size_t id = 0; id < nd; id++) {
          if (derivs[id] == Derivative::Atm::Temperature) {
            comp_dx[id][iv] -= lm * ls.dFdT(cdd[id].lso, T) +
                               linemixing_derivative(cdd[id].lso) * ls.F;
          } else if (derivs[id].isWind()) {
            comp_dx[id][iv] -= lm * ls.dFdf();  // WARNING: Not true derivative
          } else if (derivs[id].isMagnetism()) {
            comp_dx[id][iv] -= lm * ls.dFdH(cdd[id].d);
          } else if (derivs[id] == Derivative::Atm::VMR and
                     isnonzero(cdd[id].lso)) {
            comp_dx[id][iv] -= lm * ls.dFdVMR(cdd[id].lso) +
                               linemixing_derivative(cdd[id].lso) * ls.F;
          } else if (derivs[id].isLineCenter(this_line)) {
            comp_dx[id][iv] -= lm * ls.dFdF0();
          } else if (derivs[id].isshapeG0()) {
            comp_dx[id][iv] -= lm * ls.dFdG0(cdd[id].d);
          } else if (derivs[id].isshapeD0()) {
            comp_dx[id][iv] -= lm * ls.dFdD0(cdd[id].d);
          } else if (derivs[id].isshapeG2()) {
            comp_dx[id][iv] -= lm * ls.dFdG2(cdd[id].d);
          } else if (derivs[id].isshapeD2()) {
            comp_dx[id][iv] -= lm * ls.dFdD2(cdd[id].d);
          } else if (derivs[id].isshapeFVC()) {
            comp_dx[id][iv] -= lm * ls.dFdFVC(cdd[id].d);
          } else if (derivs[id].isshapeETA()) {
            comp_dx[id][iv] -= lm * ls.dFdETA(cdd[id].d);
          } else if (derivs[id].isshapeY()) {
            comp_dx[id][iv] -= Complex(0, -cdd[id].d) * ls.F;
          } else if (derivs[id].isshapeG()) {
            comp_dx[id][iv] -= cdd[id].d * ls.F;
          } else if (derivs[id].isshapeDV()) {
            comp_dx[id][iv] -= lm * ls.dFdDV(cdd[id].d);
          }
        }
      }
    }
  }
}

template <class LineShape>
void compute_mirrored_lineshape(
    std::vector<Complex> &comp_x, std::vector<std::vector<Complex>> &comp_dx,
    const std::vector<Frequency<FrequencyType::Freq>> &f,
    const std::vector<Derivative::Target> &derivs,
    const std::vector<ComputedDerivData> &cdd, const Complex lm,
    Temperature<TemperatureType::K> T, std::size_t this_line,
    Frequency<FrequencyType::Freq> cutoff_low,
    Frequency<FrequencyType::Freq> cutoff_upp, LineShape ls) {
  const size_t nv = f.size();
  const size_t nd = derivs.size();

  for (size_t iv = 0; iv < nv; iv++) {
    if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) {
      comp_x[iv] += lm * conj(ls(f[iv]));
      for (size_t id = 0; id < nd; id++) {
        if (derivs[id] == Derivative::Atm::Temperature) {
          comp_dx[id][iv] += lm * ls.dFdT(cdd[id].lso, T) +
                             linemixing_derivative(cdd[id].lso) * ls.F;
        } else if (derivs[id].isWind()) {
          comp_dx[id][iv] += lm * ls.dFdf();  // WARNING: Not true derivative
        } else if (derivs[id].isMagnetism()) {
          comp_dx[id][iv] += lm * ls.dFdH(cdd[id].d);
        } else if (derivs[id] == Derivative::Atm::VMR and
                   isnonzero(cdd[id].lso)) {
          comp_dx[id][iv] += lm * ls.dFdVMR(cdd[id].lso) +
                             linemixing_derivative(cdd[id].lso) * ls.F;
        } else if (derivs[id].isLineCenter(this_line)) {
          comp_dx[id][iv] += lm * ls.dFdF0();
        } else if (derivs[id].isshapeG0()) {
          comp_dx[id][iv] += lm * ls.dFdG0(cdd[id].d);
        } else if (derivs[id].isshapeD0()) {
          comp_dx[id][iv] += lm * ls.dFdD0(cdd[id].d);
        } else if (derivs[id].isshapeG2()) {
          comp_dx[id][iv] += lm * ls.dFdG2(cdd[id].d);
        } else if (derivs[id].isshapeD2()) {
          comp_dx[id][iv] += lm * ls.dFdD2(cdd[id].d);
        } else if (derivs[id].isshapeFVC()) {
          comp_dx[id][iv] += lm * ls.dFdFVC(cdd[id].d);
        } else if (derivs[id].isshapeETA()) {
          comp_dx[id][iv] += lm * ls.dFdETA(cdd[id].d);
        } else if (derivs[id].isshapeY()) {
          comp_dx[id][iv] += Complex(0, -cdd[id].d) * ls.F;
        } else if (derivs[id].isshapeG()) {
          comp_dx[id][iv] += cdd[id].d * ls.F;
        } else if (derivs[id].isshapeDV()) {
          comp_dx[id][iv] += lm * ls.dFdDV(cdd[id].d);
        }
      }
    }
  }

  if (cutoff_upp > 0.0) {
    ls(cutoff_upp);
    for (size_t iv = 0; iv < nv; iv++) {
      if (cutoff_low <= f[iv] and f[iv] <= cutoff_upp) {
        comp_x[iv] -= lm * conj(ls.F);
        for (size_t id = 0; id < nd; id++) {
          if (derivs[id] == Derivative::Atm::Temperature) {
            comp_dx[id][iv] -= lm * ls.dFdT(cdd[id].lso, T) +
                               linemixing_derivative(cdd[id].lso) * ls.F;
          } else if (derivs[id].isWind()) {
            comp_dx[id][iv] -= lm * ls.dFdf();  // WARNING: Not true derivative
          } else if (derivs[id].isMagnetism()) {
            comp_dx[id][iv] -= lm * ls.dFdH(cdd[id].d);
          } else if (derivs[id] == Derivative::Atm::VMR and
                     isnonzero(cdd[id].lso)) {
            comp_dx[id][iv] -= lm * ls.dFdVMR(cdd[id].lso) +
                               linemixing_derivative(cdd[id].lso) * ls.F;
          } else if (derivs[id].isLineCenter(this_line)) {
            comp_dx[id][iv] -= lm * ls.dFdF0();
          } else if (derivs[id].isshapeG0()) {
            comp_dx[id][iv] -= lm * ls.dFdG0(cdd[id].d);
          } else if (derivs[id].isshapeD0()) {
            comp_dx[id][iv] -= lm * ls.dFdD0(cdd[id].d);
          } else if (derivs[id].isshapeG2()) {
            comp_dx[id][iv] -= lm * ls.dFdG2(cdd[id].d);
          } else if (derivs[id].isshapeD2()) {
            comp_dx[id][iv] -= lm * ls.dFdD2(cdd[id].d);
          } else if (derivs[id].isshapeFVC()) {
            comp_dx[id][iv] -= lm * ls.dFdFVC(cdd[id].d);
          } else if (derivs[id].isshapeETA()) {
            comp_dx[id][iv] -= lm * ls.dFdETA(cdd[id].d);
          } else if (derivs[id].isshapeY()) {
            comp_dx[id][iv] -= Complex(0, -cdd[id].d) * ls.F;
          } else if (derivs[id].isshapeG()) {
            comp_dx[id][iv] -= cdd[id].d * ls.F;
          } else if (derivs[id].isshapeDV()) {
            comp_dx[id][iv] -= lm * ls.dFdDV(cdd[id].d);
          }
        }
      }
    }
  }
}

double boltzman_ratio(Temperature<TemperatureType::K> T,
                      Temperature<TemperatureType::K> T0,
                      Energy<EnergyType::Joule> E0) {
  return std::exp(E0 * (T - T0) / (Constant::k * T * T0));
}

constexpr double dboltzman_ratio_dT(double boltzmann_ratio,
                                    Temperature<TemperatureType::K> T,
                                    Energy<EnergyType::Joule> E0) {
  return E0 * boltzmann_ratio / (Constant::k * T * T);
}

double stimulated_emission(Temperature<TemperatureType::K> T,
                           Frequency<FrequencyType::Freq> F0) {
  return std::exp(-(Constant::h / Constant::k) * F0 / T);
}

constexpr double stimulated_relative_emission(double gamma, double gamma_ref) {
  return (1. - gamma) / (1. - gamma_ref);
}

constexpr double dstimulated_relative_emission_dT(
    double gamma, double gamma0, Frequency<FrequencyType::Freq> F0,
    Temperature<TemperatureType::K> T) {
  return (-Constant::h / Constant::k) * F0 * gamma / (T * T * (1 - gamma0));
}

constexpr double dstimulated_relative_emission_dF0(
    double gamma, double gamma0, Temperature<TemperatureType::K> T,
    Temperature<TemperatureType::K> T0) {
  return (-Constant::h / Constant::k) *
         ((1 - gamma) * gamma0 / (T0 * (1 - gamma0) * (1 - gamma0)) -
          gamma / (T * (1 - gamma0)));
}

void compute_lte_linestrength(
    Results &res, const Results &comp,
    const std::vector<Frequency<FrequencyType::Freq>> &fs,
    const Path::Point &atm, const std::vector<Derivative::Target> &derivs,
    LineStrength<FrequencyType::Freq, AreaType::m2> S0, double SZ,
    Energy<EnergyType::Joule> E0, Frequency<FrequencyType::Freq> F0, double QT0,
    Temperature<TemperatureType::K> T0, double QT, double dQTdT,
    double mixing_ratio, size_t line_id, Species::Isotope line_spec) {
  const double K1 = boltzman_ratio(atm.atm.Temp(), T0, E0);
  const double gamma = stimulated_emission(atm.atm.Temp(), F0);
  const double gamma0 = stimulated_emission(T0, F0);
  const double K2 = stimulated_relative_emission(gamma, gamma0);
  const double lte = mixing_ratio * SZ * S0 * K1 * K2 * QT0 / QT;

  std::transform(comp.x.cbegin(), comp.x.cend(), res.x.begin(),
                 [lte](auto &x) { return lte * x; });
  for (size_t i = 0; i < derivs.size(); i++) {
    if (derivs[i] == Derivative::Atm::Temperature) {
      const double dK1dT = dboltzman_ratio_dT(K1, atm.atm.Temp(), E0);
      const double dK2dT =
          dstimulated_relative_emission_dT(gamma, gamma0, F0, atm.atm.Temp());
      const double dltedT = mixing_ratio * SZ * S0 * dK1dT * K2 * QT0 / QT +
                            mixing_ratio * SZ * S0 * K1 * dK2dT * QT0 / QT -
                            dQTdT * lte / QT;
      std::transform(comp.x.cbegin(), comp.x.cend(), comp.dx[i].cbegin(),
                     res.dx[i].begin(), [dltedT, lte](auto &x, auto &dx) {
                       return dltedT * x + lte * dx;
                     });

    } else if (derivs[i].isLineCenter(line_id)) {
      const double dK2dF0 =
          dstimulated_relative_emission_dF0(gamma, gamma0, atm.atm.Temp(), T0);
      const double dltedF0 = mixing_ratio * SZ * S0 * K1 * dK2dF0 * QT0 / QT;
      std::transform(comp.x.cbegin(), comp.x.cend(), comp.dx[i].cbegin(),
                     res.dx[i].begin(), [dltedF0, lte](auto &x, auto &dx) {
                       return dltedF0 * x + lte * dx;
                     });
    } else if (derivs[i].isLineStrength(line_id)) {
      const double dltedS0 = lte / S0;  // OK beause S0 cannot be 0
      std::transform(comp.x.cbegin(), comp.x.cend(), res.dx[i].begin(),
                     [dltedS0](auto &x) { return dltedS0 * x; });
    } else if (derivs[i].isVMR(line_spec)) {
      const double dltedVMR =
          SZ * S0 * K1 * K2 * QT0 /
          QT;  // mixing_ratio can be 0 but we still have a derivative...
      std::transform(comp.x.cbegin(), comp.x.cend(), comp.dx[i].cbegin(),
                     res.dx[i].begin(), [dltedVMR, lte](auto &x, auto &dx) {
                       return dltedVMR * x + lte * dx;
                     });
    } else if (derivs[i] == Derivative::Atm::WindU) {
      const double dfdwu = atm.DopplerShiftRatioDerivativeU();
      std::transform(comp.dx[i].cbegin(), comp.dx[i].cend(), fs.cbegin(),
                     res.dx[i].begin(), [dfdwu, lte](auto &dx, auto &f) {
                       return dfdwu * lte * f * dx;
                     });
    } else if (derivs[i] == Derivative::Atm::WindV) {
      const double dfdwv = atm.DopplerShiftRatioDerivativeV();
      std::transform(comp.dx[i].cbegin(), comp.dx[i].cend(), fs.cbegin(),
                     res.dx[i].begin(), [dfdwv, lte](auto &dx, auto &f) {
                       return dfdwv * lte * f * dx;
                     });
    } else if (derivs[i] == Derivative::Atm::WindV) {
      const double dfdww = atm.DopplerShiftRatioDerivativeW();
      std::transform(comp.dx[i].cbegin(), comp.dx[i].cend(), fs.cbegin(),
                     res.dx[i].begin(), [dfdww, lte](auto &dx, auto &f) {
                       return dfdww * lte * f * dx;
                     });
    } else {
      std::transform(comp.dx[i].cbegin(), comp.dx[i].cend(), res.dx[i].begin(),
                     [lte](auto &dx) { return lte * dx; });
    }
  }
}

std::pair<double, double> compute_nlte_linestrength(
    double SZ, Frequency<FrequencyType::Freq> F0, double Gl, double Gu,
    Decay<DecayType::ExponentialPerSecond> A, double r2, double r1,
    Temperature<TemperatureType::K> T) {
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

void compute(Results &res, Results &src, Results &comp,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const Band &band, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs,
             const Polarization polarization) {
  const double H = atm.atm.MagField().Strength();
  const double GDpart = band.GD_giv_F0(atm.atm.Temp());
  const double QT0 = band.QT0();
  const double QT = band.QT(atm.atm.Temp());
  const double dQTdT = band.dQT(atm.atm.Temp());
  const double mixing_ratio = atm.atm.VolumeMixingRatio(band.Isotopologue());

  // Skip Zeeman copies if Zeeman or without
  if (band.doZeeman() and polarization == Polarization::None)
    return;
  else if (not band.doZeeman() and polarization not_eq Polarization::None)
    return;

  for (size_t iline = 0; iline < band.n_lines(); iline++) {
    const auto &line = band.Lines()[iline];
    const auto zeeman_range =
        line.ZeemanRange(polarization, band.Isotopologue());
    const auto X = line.ShapeModel()(atm.atm.Temp(), band.T0(), atm.atm.Pres(),
                                     atm.atm.VolumeMixingRatios());
    const Complex lm{1.0 + X.G, -X.Y};

    auto cdd = process_derivatives(derivs, line.ShapeModel(), atm.atm.Temp(),
                                   band.T0(), atm.atm.Pres(),
                                   atm.atm.VolumeMixingRatios());

    for (int iz = zeeman_range.first; iz <= zeeman_range.second; iz++) {
      const auto DZ =
          line.ZeemanSplitting(polarization, band.Isotopologue(), iz);
      const auto SZ =
          line.ZeemanStrength(polarization, band.Isotopologue(), iz);
      const auto cutoff_low = band.CutoffLower(iline);
      const auto cutoff_upp = band.CutoffUpper(iline);

      // Update the derivatives for Zeeman line
      update_derivatives(cdd, derivs, DZ);

      // Forward Line Shape
      switch (band.ShapeType()) {
        case Shape::DP:
          compute_lineshape(
              comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(), line.ID(),
              cutoff_low, cutoff_upp,
              LineShape::Base::Doppler(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::LP:
          compute_lineshape(
              comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(), line.ID(),
              cutoff_low, cutoff_upp,
              LineShape::Base::Lorentz(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::VP:
          compute_lineshape(
              comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(), line.ID(),
              cutoff_low, cutoff_upp,
              LineShape::Base::Voigt(line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::SDVP:
          compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(),
                            line.ID(), cutoff_low, cutoff_upp,
                            LineShape::Base::SpeedDependentVoigt(
                                line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::SDHCVP:
          compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(),
                            line.ID(), cutoff_low, cutoff_upp,
                            LineShape::Base::SpeedDependentHardCollisionVoigt(
                                line.F0(), X, GDpart, DZ * H));
          break;
        case Shape::HTP:
          compute_lineshape(
              comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(), line.ID(),
              cutoff_low, cutoff_upp,
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
              compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm,
                                atm.atm.Temp(), line.ID(), cutoff_low,
                                cutoff_upp,
                                LineShape::Base::Doppler(
                                    -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::LP:
              compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm,
                                atm.atm.Temp(), line.ID(), cutoff_low,
                                cutoff_upp,
                                LineShape::Base::Lorentz(
                                    -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::VP:
              compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm,
                                atm.atm.Temp(), line.ID(), cutoff_low,
                                cutoff_upp,
                                LineShape::Base::Voigt(-line.F0(), mirrored(X),
                                                       -GDpart, DZ * H));
              break;
            case Shape::SDVP:
              compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm,
                                atm.atm.Temp(), line.ID(), cutoff_low,
                                cutoff_upp,
                                LineShape::Base::SpeedDependentVoigt(
                                    -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::SDHCVP:
              compute_lineshape(
                  comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(),
                  line.ID(), cutoff_low, cutoff_upp,
                  LineShape::Base::SpeedDependentHardCollisionVoigt(
                      -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::HTP:
              compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm,
                                atm.atm.Temp(), line.ID(), cutoff_low,
                                cutoff_upp,
                                LineShape::Base::HartmannTran(
                                    -line.F0(), mirrored(X), -GDpart, DZ * H));
              break;
            case Shape::FINAL: { /* leave last */
            }
          }
          break;
        case Mirroring::Lorentz:
          compute_lineshape(comp.x, comp.dx, f, derivs, cdd, lm, atm.atm.Temp(),
                            line.ID(), cutoff_low, cutoff_upp,
                            LineShape::Base::Lorentz(-line.F0(), mirrored(X),
                                                     -GDpart, DZ * H));
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
          compute_lte_linestrength(res, comp, f, atm, derivs, line.I0(), SZ,
                                   line.E0(), line.F0(), QT0, band.T0(), QT,
                                   dQTdT, mixing_ratio, line.ID(),
                                   band.Isotopologue());
        } break;
        case Population::ByNLTE: {
          double r1 = 0, r2 = 0;  // FIXME: Input the correct numbers when
                                  // operating in NLTE mode

          const std::pair<double, double> S =
              compute_nlte_linestrength(SZ, line.F0(), line.Gl(), line.Gu(),
                                        line.A(), r2, r1, atm.atm.Temp());

          std::transform(comp.x.cbegin(), comp.x.cend(), res.x.cbegin(),
                         res.x.begin(),
                         [k = S.first](auto &a, auto &b) { return k * a + b; });

          std::transform(
              comp.x.cbegin(), comp.x.cend(), src.x.cbegin(), src.x.begin(),
              [r = S.second](auto &a, auto &b) { return r * a + b; });
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
