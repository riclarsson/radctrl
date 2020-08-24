#include "xsec.h"

#include <execution>

#include "xsec_lbl.h"

namespace Absorption {
namespace Xsec {
template <size_t N>
void internal_compute(std::vector<PropMat<N>>& pm, const std::vector<double>& f,
                      const std::vector<Band>& bands, const Path::Point& atm) {
  const auto mag = atm.atm.MagField();
  const auto los = atm.nav.sphericalLos();
  [[maybe_unused]] const auto a =
      Zeeman::angles(mag.u(), mag.v(), mag.w(), los.za(), los.aa());

  std::vector<Complex> x(f.size());
  std::vector<Complex> xpart(f.size());
  for (auto z : {Polarization::SigmaMinus, Polarization::Pi,
                 Polarization::SigmaPlus, Polarization::None}) {
    if constexpr (N not_eq 4) {
      if (z == Polarization::SigmaMinus or z == Polarization::Pi or
          z == Polarization::SigmaPlus) {
        continue;
      }
    }

    // Empty x since polarizations must be treated independently
    std::fill(x.begin(), x.end(), Complex(0, 0));

    // Add all bands
    for (auto& band : bands) {
      Lbl::compute(x, xpart, f, band, atm, z);
    }

    // FIXME: Add other types of cross-sections here

    // Sum up this polarization in the propagation matrix
    if (z == Polarization::None) {
      std::transform(x.cbegin(), x.cend(), pm.begin(), pm.begin(),
                     [](auto& dx, auto& p) { return p.add_unpolarized(dx); });
    } else if (z == Polarization::SigmaMinus) {
      if constexpr (N == 4)
        std::transform(
            x.cbegin(), x.cend(), pm.begin(), pm.begin(),
            [sm = Zeeman::PolarizationVector<Polarization::SigmaMinus>(a)](
                auto& dx, auto& p) { return p.add_polarized(dx, sm); });
    } else if (z == Polarization::Pi) {
      if constexpr (N == 4)
        std::transform(
            x.cbegin(), x.cend(), pm.begin(), pm.begin(),
            [pi = Zeeman::PolarizationVector<Polarization::Pi>(a)](
                auto& dx, auto& p) { return p.add_polarized(dx, pi); });
    } else if (z == Polarization::SigmaPlus) {
      if constexpr (N == 4)
        std::transform(
            x.cbegin(), x.cend(), pm.begin(), pm.begin(),
            [sp = Zeeman::PolarizationVector<Polarization::SigmaPlus>(a)](
                auto& dx, auto& p) { return p.add_polarized(dx, sp); });
    }
  }
}

void compute(std::vector<PropMat<1>>& pm, const std::vector<double>& f,
             const std::vector<Band>& bands, const Path::Point& atm) {
  internal_compute(pm, f, bands, atm);
}
void compute(std::vector<PropMat<2>>& pm, const std::vector<double>& f,
             const std::vector<Band>& bands, const Path::Point& atm) {
  internal_compute(pm, f, bands, atm);
}
void compute(std::vector<PropMat<3>>& pm, const std::vector<double>& f,
             const std::vector<Band>& bands, const Path::Point& atm) {
  internal_compute(pm, f, bands, atm);
}
void compute(std::vector<PropMat<4>>& pm, const std::vector<double>& f,
             const std::vector<Band>& bands, const Path::Point& atm) {
  internal_compute(pm, f, bands, atm);
}
}  // namespace Xsec
}  // namespace Absorption
