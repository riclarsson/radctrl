#include "xsec.h"

#include <execution>

#include "xsec_lbl.h"

namespace Absorption {
namespace PropagationMatrix {
template <size_t N>
void internal_compute(Results<N> &res, Results<N> &src,
                      const std::vector<Frequency<FrequencyType::Freq>> &f,
                      const std::vector<Band> &bands, const Path::Point &atm) {
  std::fill(res.x.begin(), res.x.end(), PropMat<N>());
  std::fill(src.x.begin(), src.x.end(), PropMat<N>());
  std::fill(res.dx.begin(), res.dx.end(), res.x);
  std::fill(src.dx.begin(), src.dx.end(), src.x);

  const auto mag = atm.atm.MagField();
  const auto los = atm.nav.sphericalLos();
  [[maybe_unused]] const auto a =
      Zeeman::angles(mag.u(), mag.v(), mag.w(), los.za(), los.aa());

  Xsec::Lbl::Results lbl_res(f.size());
  Xsec::Lbl::Results lbl_src(f.size());
  Xsec::Lbl::Results lbl_clc(f.size());
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

    // Add all bands
    for (const auto &band : bands) {
      Xsec::Lbl::compute(lbl_res, lbl_src, lbl_clc, f, band, atm, z);
    }

    // FIXME: Add other types of cross-sections here

    // Sum up this polarization in the propagation matrix
    if (z == Polarization::None) {
      std::transform(lbl_res.x.cbegin(), lbl_res.x.cend(), res.x.begin(),
                     res.x.begin(),
                     [](Complex dx, PropMat<N> p) { return p + dx.real(); });

      for (size_t i = 0; i < res.dx.size(); i++) {
        std::transform(lbl_res.dx[i].cbegin(), lbl_res.dx[i].cend(),
                       res.dx[i].cbegin(), res.dx[i].begin(),
                       [](Complex dx, PropMat<N> p) { return p + dx.real(); });
      }

      std::transform(lbl_src.x.cbegin(), lbl_src.x.cend(), src.x.begin(),
                     src.x.begin(),
                     [](Complex dx, PropMat<N> p) { return p + dx.real(); });

      for (size_t i = 0; i < src.dx.size(); i++) {
        std::transform(lbl_src.dx[i].cbegin(), lbl_src.dx[i].cend(),
                       src.dx[i].cbegin(), src.dx[i].begin(),
                       [](Complex dx, PropMat<N> p) { return p + dx.real(); });
      }
    } else if (z == Polarization::SigmaMinus or z == Polarization::Pi or
               z == Polarization::SigmaPlus) {
      if constexpr (N == 4) {
        const auto pol =
            (z == Polarization::SigmaMinus)
                ? Zeeman::PolarizationVector<Polarization::SigmaMinus>(a)
                : (z == Polarization::Pi)
                      ? Zeeman::PolarizationVector<Polarization::Pi>(a)
                      : Zeeman::PolarizationVector<Polarization::SigmaPlus>(a);

        std::transform(lbl_res.x.cbegin(), lbl_res.x.cend(), res.x.begin(),
                       res.x.begin(), [pol](Complex dx, PropMat<N> p) {
                         return p + ZeemanPropMat4x4(dx, pol);
                       });

        for (size_t i = 0; i < res.dx.size(); i++) {
          std::transform(lbl_res.dx[i].cbegin(), lbl_res.dx[i].cend(),
                         res.dx[i].cbegin(), res.dx[i].begin(),
                         [pol](Complex dx, PropMat<N> p) {
                           return p + ZeemanPropMat4x4(dx, pol);
                         });
        }

        std::transform(lbl_src.x.cbegin(), lbl_src.x.cend(), src.x.begin(),
                       src.x.begin(), [pol](Complex dx, PropMat<N> p) {
                         return p + ZeemanPropMat4x4(dx, pol);
                       });

        for (size_t i = 0; i < src.dx.size(); i++) {
          std::transform(lbl_src.dx[i].cbegin(), lbl_src.dx[i].cend(),
                         src.dx[i].cbegin(), src.dx[i].begin(),
                         [pol](Complex dx, PropMat<N> p) {
                           return p + ZeemanPropMat4x4(dx, pol);
                         });
        }
      }
    }  // Add other types of polarizations (e.g., Faraday) here if and when
       // necessary
  }

  // Turn cross-sections into absorption coefficients
  const auto numden = atm.atm.NumberDensity();
  std::transform(res.x.cbegin(), res.x.cend(), res.x.begin(),
                 [numden](auto &x) { return x * numden; });
  std::transform(src.x.cbegin(), src.x.cend(), src.x.begin(),
                 [numden](auto &x) { return x * numden; });
  //   for (size_t i = 0; i < res.dx.size(); i++) {
  //     if (jac[i] == Derivative::Type::Temperature)
  //       std::transform(res.dx[i].cbegin(), res.dx[i].cend(),
  //       res.dx[i].begin(),
  //       [numden=atm.atm.NumberDensityTemperatureDerivative()](auto& x){return
  //       x*numden;});
  //     else if (jac[i] == Derivative::Type::Pressure)
  //       std::transform(res.dx[i].cbegin(), res.dx[i].cend(),
  //       res.dx[i].begin(),
  //       [numden=atm.atm.NumberDensityPressureDerivative()](auto& x){return
  //       x*numden;});
  //     else
  //       std::transform(res.dx[i].cbegin(), res.dx[i].cend(),
  //       res.dx[i].begin(), [numden](auto& x){return x*numden;});
  //   }
  //   for (size_t i = 0; i < res.dx.size(); i++) {
  //     if (jac[i] == Derivative::Type::Temperature)
  //       std::transform(src.dx[i].cbegin(), src.dx[i].cend(),
  //       src.dx[i].begin(),
  //       [numden=atm.atm.NumberDensityTemperatureDerivative()](auto& x){return
  //       x*numden;});
  //     else if (jac[i] == Derivative::Type::Pressure)
  //       std::transform(src.dx[i].cbegin(), src.dx[i].cend(),
  //       src.dx[i].begin(),
  //       [numden=atm.atm.NumberDensityPressureDerivative()](auto& x){return
  //       x*numden;});
  //     else
  //       std::transform(src.dx[i].cbegin(), src.dx[i].cend(),
  //       src.dx[i].begin(), [numden](auto& x){return x*numden;});
  //   }
}

void compute(Results<1> &res, Results<1> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm) {
  internal_compute(res, src, f, bands, atm);
}
void compute(Results<2> &res, Results<2> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm) {
  internal_compute(res, src, f, bands, atm);
}
void compute(Results<3> &res, Results<3> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm) {
  internal_compute(res, src, f, bands, atm);
}
void compute(Results<4> &res, Results<4> &src,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const std::vector<Band> &bands, const Path::Point &atm) {
  internal_compute(res, src, f, bands, atm);
}
}  // namespace PropagationMatrix
}  // namespace Absorption
