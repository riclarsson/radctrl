#include "xsec.h"

#include <execution>

#include "xsec_lbl.h"

namespace Absorption {
namespace PropagationMatrix {
template <size_t N>
void internal_compute(Results<N> &res, Results<N> &src,
                      const std::vector<Frequency<FrequencyType::Freq>> &f,
                      const std::vector<Band> &bands, const Path::Point &atm,
                      const std::vector<Derivative::Target> &derivs) {
  std::fill(res.x.begin(), res.x.end(), PropMat<N>());
  std::fill(src.x.begin(), src.x.end(), PropMat<N>());
  std::fill(res.dx.begin(), res.dx.end(), res.x);
  std::fill(src.dx.begin(), src.dx.end(), src.x);

  const auto mag = atm.atm.MagField();
  const auto los = atm.nav.sphericalLos();
  [[maybe_unused]] const auto zeeman_angles =
      Zeeman::angles_with_derivatives(mag, los.za(), los.aa());

  Xsec::Lbl::Results lbl_res(f.size(), derivs.size());
  Xsec::Lbl::Results lbl_src(f.size(), derivs.size());
  Xsec::Lbl::Results lbl_clc(f.size(), derivs.size());
  bool first = true;
  for (auto z : {Polarization::SigmaMinus, Polarization::Pi,
                 Polarization::SigmaPlus, Polarization::None}) {
    if constexpr (N not_eq 4) {
      if (z == Polarization::SigmaMinus or z == Polarization::Pi or
          z == Polarization::SigmaPlus) {
        continue;
      }
    }

    // Reset all since polarizations must be treated independently
    if (not first) {
      std::fill(lbl_res.x.begin(), lbl_res.x.end(), Complex(0, 0));
      std::fill(lbl_src.x.begin(), lbl_src.x.end(), Complex(0, 0));
      std::fill(lbl_res.dx.begin(), lbl_res.dx.end(), lbl_res.x);
      std::fill(lbl_src.dx.begin(), lbl_src.dx.end(), lbl_src.x);
    } else {
      first = false;
    }

    // Add up xsec from all bands
    for (const auto &band : bands) {
      Xsec::Lbl::compute(lbl_res, lbl_src, lbl_clc, f, band, atm, derivs, z);
    }

    // FIXME: Add other types of cross-sections here

    // Sum up this polarization in the propagation matrix
    if (z == Polarization::None) {
      std::transform(lbl_res.x.cbegin(), lbl_res.x.cend(), res.x.cbegin(),
                     res.x.begin(),
                     [](auto &dx, auto &p) { return p + dx.real(); });

      for (size_t i = 0; i < res.dx.size(); i++) {
        std::transform(lbl_res.dx[i].cbegin(), lbl_res.dx[i].cend(),
                       res.dx[i].cbegin(), res.dx[i].begin(),
                       [](auto &dx, auto &p) { return p + dx.real(); });
      }

      std::transform(lbl_src.x.cbegin(), lbl_src.x.cend(), src.x.cbegin(),
                     src.x.begin(),
                     [](auto &dx, auto &p) { return p + dx.real(); });

      for (size_t i = 0; i < src.dx.size(); i++) {
        std::transform(lbl_src.dx[i].cbegin(), lbl_src.dx[i].cend(),
                       src.dx[i].cbegin(), src.dx[i].begin(),
                       [](auto &dx, auto &p) { return p + dx.real(); });
      }
    } else if (z == Polarization::SigmaMinus or z == Polarization::Pi or
               z == Polarization::SigmaPlus) {
      if constexpr (N == 4) {
        const auto pol =
            (z == Polarization::SigmaMinus)
                ? Zeeman::PolarizationVector<Polarization::SigmaMinus>(
                      zeeman_angles.calc)
                : (z == Polarization::Pi)
                      ? Zeeman::PolarizationVector<Polarization::Pi>(
                            zeeman_angles.calc)
                      : Zeeman::PolarizationVector<Polarization::SigmaPlus>(
                            zeeman_angles.calc);
        const auto dpol_dtheta =
            (z == Polarization::SigmaMinus)
                ? Zeeman::PolarizationVectorThetaDerivative<
                      Polarization::SigmaMinus>(zeeman_angles.calc)
                : (z == Polarization::Pi)
                      ? Zeeman::PolarizationVectorThetaDerivative<
                            Polarization::Pi>(zeeman_angles.calc)
                      : Zeeman::PolarizationVectorThetaDerivative<
                            Polarization::SigmaPlus>(zeeman_angles.calc);
        const auto dpol_deta =
            (z == Polarization::SigmaMinus)
                ? Zeeman::PolarizationVectorEtaDerivative<
                      Polarization::SigmaMinus>(zeeman_angles.calc)
                : (z == Polarization::Pi)
                      ? Zeeman::PolarizationVectorEtaDerivative<
                            Polarization::Pi>(zeeman_angles.calc)
                      : Zeeman::PolarizationVectorEtaDerivative<
                            Polarization::SigmaPlus>(zeeman_angles.calc);

        std::transform(
            lbl_res.x.cbegin(), lbl_res.x.cend(), res.x.cbegin(), res.x.begin(),
            [pol](auto &x, auto &p) { return p + ZeemanPropMat4x4(x, pol); });

        for (size_t i = 0; i < res.dx.size(); i++) {
          if (derivs[i].isMagnetism()) {
            std::transform(lbl_res.dx[i].cbegin(), lbl_res.dx[i].cend(),
                           res.x.cbegin(), res.dx[i].begin(),
                           [dpol_deta, dpol_dtheta](auto &x, auto &p) {
                             return p + ZeemanPropMat4x4(x, dpol_deta) +
                                    ZeemanPropMat4x4(x, dpol_dtheta);
                           });
          }
          std::transform(lbl_res.dx[i].cbegin(), lbl_res.dx[i].cend(),
                         res.dx[i].cbegin(), res.dx[i].begin(),
                         [pol](auto &dx, auto &p) {
                           return p + ZeemanPropMat4x4(dx, pol);
                         });
        }

        std::transform(
            lbl_src.x.cbegin(), lbl_src.x.cend(), src.x.cbegin(), src.x.begin(),
            [pol](auto &x, auto &p) { return p + ZeemanPropMat4x4(x, pol); });

        for (size_t i = 0; i < src.dx.size(); i++) {
          if (derivs[i].isMagnetism()) {
            std::transform(lbl_src.dx[i].cbegin(), lbl_src.dx[i].cend(),
                           src.x.cbegin(), src.dx[i].begin(),
                           [dpol_deta, dpol_dtheta](auto &x, auto &p) {
                             return p + ZeemanPropMat4x4(x, dpol_deta) +
                                    ZeemanPropMat4x4(x, dpol_dtheta);
                           });
          }
          std::transform(lbl_src.dx[i].cbegin(), lbl_src.dx[i].cend(),
                         src.dx[i].cbegin(), src.dx[i].begin(),
                         [pol](auto &dx, auto &p) {
                           return p + ZeemanPropMat4x4(dx, pol);
                         });
        }
      }
    }  // Add other types of polarizations (e.g., Faraday) here if and when
       // necessary
  }

  // Turn cross-sections into absorption coefficients (in-place modification, so
  // order of operations matter)
  const double numden = atm.atm.NumberDensity();
  const double dnumden_dtemperature =
      atm.atm.NumberDensityTemperatureDerivative();
  // First the derivatives
  for (size_t i = 0; i < res.dx.size(); i++) {
    if (derivs[i] == Derivative::Atm::Temperature) {
      std::transform(
          res.x.cbegin(), res.x.cend(), res.dx[i].begin(),
                     [dnumden_dtemperature](auto &x) { return x * dnumden_dtemperature; });
    }
    std::transform(res.dx[i].cbegin(), res.dx[i].cend(), res.dx[i].begin(),
                   [numden](auto &dx) { return dx * numden; });
  }

  // Second the absorption coefficient
  std::transform(res.x.cbegin(), res.x.cend(), res.x.begin(),
                 [numden](auto &x) { return x * numden; });

  // First the derivatives
  for (size_t i = 0; i < src.dx.size(); i++) {
    if (derivs[i] == Derivative::Atm::Temperature) {
      std::transform(
          src.x.cbegin(), src.x.cend(), src.dx[i].begin(),
                     [dnumden_dtemperature](auto &x) { return x * dnumden_dtemperature; });
    }
    std::transform(src.dx[i].cbegin(), src.dx[i].cend(), src.dx[i].begin(),
                   [numden](auto &dx) { return dx * numden; });
  }

  // Second the absorption coefficient
  std::transform(src.x.cbegin(), src.x.cend(), src.x.begin(),
                 [numden](auto &x) { return x * numden; });
}

void compute(Results<1> &res, Results<1> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs) {
  internal_compute(res, src, f, bands, atm, derivs);
}
void compute(Results<2> &res, Results<2> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs) {
  internal_compute(res, src, f, bands, atm, derivs);
}
void compute(Results<3> &res, Results<3> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs) {
  internal_compute(res, src, f, bands, atm, derivs);
}
void compute(Results<4> &res, Results<4> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs) {
  internal_compute(res, src, f, bands, atm, derivs);
}
}  // namespace PropagationMatrix
}  // namespace Absorption
