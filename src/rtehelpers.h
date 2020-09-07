#include "mathhelpers.h"
#include "rte.h"

namespace RTE {
template <size_t N>
std::vector<RadVec<N>> source_vec_planck(
    const Temperature<TemperatureType::K> T,
    const std::vector<Frequency<FrequencyType::Freq>>& fvec) {
  std::vector<RadVec<N>> out(fvec.size());

  if constexpr (N == 1)
    std::transform(fvec.cbegin(), fvec.cend(), out.begin(),
                   [T](auto& f) { return RadVec<1>{B(T, f)}; });
  else if constexpr (N == 2)
    std::transform(fvec.cbegin(), fvec.cend(), out.begin(), [T](auto& f) {
      return RadVec<2>{B(T, f), 0};
    });
  else if constexpr (N == 3)
    std::transform(fvec.cbegin(), fvec.cend(), out.begin(), [T](auto& f) {
      return RadVec<3>{B(T, f), 0, 0};
    });
  else if constexpr (N == 4)
    std::transform(fvec.cbegin(), fvec.cend(), out.begin(), [T](auto& f) {
      return RadVec<4>{B(T, f), 0, 0, 0};
    });
  return out;
}

template <size_t N>
void to_planck(Grid<RadVec<N>, 2>& rad, Grid<RadVec<N>, 3>& jac,
               const std::vector<Frequency<FrequencyType::Freq>>& fvec) {
  const size_t np = rad.sizes()[0];
  const size_t nv = rad.sizes()[1];
  const size_t nt = jac.sizes()[0];

  if (nt) {
    for (size_t ip = 0; ip < np; ip++) {
      for (size_t iv = 0; iv < nv; iv++) {
        RadVec<N> scl;
        if constexpr (N == 1) {
          scl = RadVec<1>{invB(rad(ip, iv)[0], fvec[iv])};
        } else if constexpr (N == 2) {
          scl = RadVec<2>{
              invB(rad(ip, iv)[0], fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv])};
        } else if constexpr (N == 3) {
          scl = RadVec<3>{
              invB(rad(ip, iv)[0], fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[2]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[2]), fvec[iv])};
        } else if constexpr (N == 4) {
          scl = RadVec<4>{
              invB(rad(ip, iv)[0], fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[2]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[2]), fvec[iv]),
              invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[3]), fvec[iv]) -
                  invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[3]), fvec[iv])};
        }

        for (size_t it = 0; it < nt; it++) {
          if constexpr (N == 1) {
            jac(it, ip, iv) = RadVec<1>{scl[0] * jac(it, ip, iv)[0]};
          } else if constexpr (N == 2) {
            jac(it, +ip, iv) = RadVec<2>{scl[0] * jac(it, ip, iv)[0],
                                         scl[1] * jac(it, ip, iv)[1]};
          } else if constexpr (N == 3) {
            jac(it, ip, iv) = RadVec<3>{scl[0] * jac(it, ip, iv)[0],
                                        scl[1] * jac(it, ip, iv)[1],
                                        scl[2] * jac(it, ip, iv)[2]};
          } else if constexpr (N == 4) {
            jac(it, ip, iv) = RadVec<4>{
                scl[0] * jac(it, ip, iv)[0], scl[1] * jac(it, ip, iv)[1],
                scl[2] * jac(it, ip, iv)[2], scl[3] * jac(it, ip, iv)[3]};
          }
        }
      }
    }
  }

  for (size_t ip = 0; ip < np; ip++) {
    for (size_t iv = 0; iv < nv; iv++) {
      if constexpr (N == 1) {
        rad(ip, iv) = RadVec<1>{invB(rad(ip, iv)[0], fvec[iv])};
      } else if constexpr (N == 2) {
        rad(ip, iv) = RadVec<2>{
            invB(rad(ip, iv)[0], fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv])};
      } else if constexpr (N == 3) {
        rad(ip, iv) = RadVec<3>{
            invB(rad(ip, iv)[0], fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[2]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[2]), fvec[iv])};
      } else if constexpr (N == 4) {
        rad(ip, iv) = RadVec<4>{
            invB(rad(ip, iv)[0], fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[1]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[1]), fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[2]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[2]), fvec[iv]),
            invB(0.5 * (rad(ip, iv)[0] + rad(ip, iv)[3]), fvec[iv]) -
                invB(0.5 * (rad(ip, iv)[0] - rad(ip, iv)[3]), fvec[iv])};
      }
    }
  }
}
}  // namespace RTE
