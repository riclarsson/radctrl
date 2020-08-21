#include "xsec.h"

#include <execution>

#include "xsec_lbl.h"

namespace Absorption {
namespace Xsec {
template <size_t N>
void internal_compute(std::vector<PropMat<N>>& pm, const std::vector<double>& f,
                      const std::vector<Band>& bands, const Path::Point& atm) {
  std::vector<Complex> x(f.size(), {0, 0});
  std::vector<Complex> xpart(f.size());

  for (auto& band : bands) {
    if (band.doZeeman()) {
      if constexpr (N == 4) {
        for (auto z :
             {Zeeman::Polarization::SigmaMinus, Zeeman::Polarization::SigmaPlus,
              Zeeman::Polarization::Pi}) {
          Lbl::compute(x, xpart, f, band, atm, z);
          std::array<double, PropMat<N>::SIZE> polar;
          if (z == Zeeman::Polarization::SigmaMinus) {
          } else if (z == Zeeman::Polarization::SigmaPlus) {
          } else if (z == Zeeman::Polarization::Pi) {
          }
          std::transform(std::execution::par_unseq, x.cbegin(), x.cend(),
                         pm.begin(), pm.begin(), [&polar](auto& dx, auto& p) {
                           return p.add_polarized(dx, polar);
                         });
        }
      } else {
        std::cerr << "Zeeman lines declared but Stokes dimensionality is bad\n";
        std::exit(1);
      }
    } else {
      Lbl::compute(x, xpart, f, band, atm);
      std::transform(std::execution::par_unseq, x.cbegin(), x.cend(),
                     pm.begin(), pm.begin(),
                     [](auto& dx, auto& p) { return p.add_unpolarized(dx); });
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
