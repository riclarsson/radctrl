#include "inverse.h"

namespace RTE::Inverse {
template <std::size_t N>
void internal_map_to_jacobian(
    ForwardConvolution& res, const Forward::Results<N>& partres,
    const std::vector<Derivative::Target>& derivs,
    const std::vector<Atmosphere::InterPoints>& interp_weights,
    const double path_weight) {
  const std::size_t np = partres.x.sizes()[0];
  const std::size_t nf = partres.x.sizes()[1];
  const std::size_t nt = derivs.size();

  std::size_t NT0 = 0;
  for (std::size_t ip = 0; ip < np; ip++) {
    auto weights = interp_weights[ip].Weights();

    for (std::size_t it = 0; it < nt; it++) {
      for (std::size_t iv = 0; iv < nf; iv++) {
        // Path-weighted radiation derivative vector
        const RadVec<N> this_val = path_weight * partres.dx(it, ip, iv);
        for (std::size_t is = 0; is < N; is++) {
          for (auto& iw : weights) {
            // Add if there is weight... note that 0 weight might mean out of
            // bound index.
            if (iw.w not_eq 0) {
              res.jac(iv * N + is,
                      NT0 + res.iatmdata(iw.itid, iw.ialt, iw.ilat, iw.ilon)) +=
                  iw.w * this_val[is];
            }
          }
        }
      }

      // Move derivative pointer
      NT0 += (derivs[it] == Derivative::Type::Atm) ? res.natmdata() : 1;
    }
  }
}

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<1>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight) {
  internal_map_to_jacobian(res, partres, derivs, interp_weights, path_weight);
}

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<2>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight) {
  internal_map_to_jacobian(res, partres, derivs, interp_weights, path_weight);
}

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<3>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight) {
  internal_map_to_jacobian(res, partres, derivs, interp_weights, path_weight);
}

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<4>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight) {
  internal_map_to_jacobian(res, partres, derivs, interp_weights, path_weight);
}
}  // namespace RTE::Inverse
