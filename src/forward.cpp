#include "forward.h"

#include "mathhelpers.h"

namespace RTE {
namespace Forward {
template <size_t N>
void forward_rte_step(SubGrid<RadVec<N>, 2>& rad_out,
                      SubGrid<TraMat<N>, 2>& T_out,
                      SubGrid<RadVec<N>, 3>& jac_out1,
                      SubGrid<RadVec<N>, 3>& jac_out2,
                      const ConstSubGrid<RadVec<N>, 2>& rad_in,
                      const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                      const Path::Point& p1, const Path::Point& p2,
                      const Absorption::PropagationMatrix::Results<N>& K1,
                      const Absorption::PropagationMatrix::Results<N>& K2,
                      const Absorption::PropagationMatrix::Results<N>& S1,
                      const Absorption::PropagationMatrix::Results<N>& S2,
                      const std::vector<Derivative::Target>& derivs) {
  for (size_t iv = 0; iv < f_grid.size(); iv++) {
    auto B1 = B(p1.atm.Temp(), f_grid[iv]);
    auto B2 = B(p2.atm.Temp(), f_grid[iv]);
    auto J =
        0.5 * (source(K1.x[iv], RadVec<N>{S1.x[iv]}, RadVec<N>{K1.x[iv]}, B1) +
               source(K2.x[iv], RadVec<N>{S2.x[iv]}, RadVec<N>{K2.x[iv]}, B2));
    auto r = dist(p1, p2);
    T_out[iv] = linear_transmat(K1.x[iv], K2.x[iv], r);
    rad_out[iv] = update(rad_in[iv], T_out[iv], J);

    for (size_t id = 0; id < derivs.size(); id++) {
      auto dB1 = derivs[id] == Derivative::Atm::Temperature
                     ? dBdT(p1.atm.Temp(), f_grid[iv])
                     : derivs[id].isWind() ? dBdf(p1.atm.Temp(), f_grid[iv])
                                           : decltype(B1)(0);
      auto dB2 = derivs[id] == Derivative::Atm::Temperature
                     ? dBdT(p2.atm.Temp(), f_grid[iv])
                     : derivs[id].isWind() ? dBdf(p2.atm.Temp(), f_grid[iv])
                                           : decltype(B2)(0);
      auto dJ1 =
          dsource(K1.x[iv], K1.dx[id][iv], J, RadVec<N>{S1.dx[id][iv]},
                  RadVec<N>{K1.x[iv]}, RadVec<N>{K2.dx[id][iv]}, B1, dB1);
      auto dJ2 =
          dsource(K2.x[iv], K2.dx[id][iv], J, RadVec<N>{S2.dx[id][iv]},
                  RadVec<N>{K2.x[iv]}, RadVec<N>{K2.dx[id][iv]}, B2, dB2);
      auto dT1 = dlinear_transmat(T_out[iv], K1.x[iv], K2.x[iv], K1.dx[id][iv],
                                  r, 0.0);
      auto dT2 = dlinear_transmat(T_out[iv], K1.x[iv], K2.x[iv], K2.dx[id][iv],
                                  r, 0.0);

      jac_out1(id, iv) = dupdate(rad_in[iv], T_out[iv], dT1, J, dJ1);
      jac_out2(id, iv) = dupdate(rad_in[iv], T_out[iv], dT2, J, dJ2) +
                         T_out[iv] * jac_out2(id, iv);
    }
  }
}

template <size_t N>
Results<N> internal_compute(const std::vector<RadVec<N>>& rad0,
                            const Calculations& calcs) {
  // Output
  Results<N> rad(rad0, calcs);

  // Propagation matrix on levels before (1) and after (2)
  Absorption::PropagationMatrix::Results<N> K1(rad0.size(),
                                               calcs.targets.size()),
      K2(rad0.size(), calcs.targets.size());
  Absorption::PropagationMatrix::Results<N> S1(rad0.size(),
                                               calcs.targets.size()),
      S2(rad0.size(), calcs.targets.size());

  // Compute the point farthest from the sensor and set it as starting point
  // (FIXME: Only LTE at this point)
  Absorption::PropagationMatrix::compute(K2, S2, calcs.f_grid, calcs.bands,
                                         calcs.path[calcs.path.size() - 1],
                                         calcs.targets);

  // For all path points starting at the second to last
  for (size_t ip = calcs.path.size() - 2; ip < calcs.path.size() - 1; ip--) {
    // First, swap the points
    std::swap(K1, K2);
    std::swap(S1, S2);

    // Compute the end-point of the path
    Absorption::PropagationMatrix::compute(K2, S2, calcs.f_grid, calcs.bands,
                                           calcs.path[ip], calcs.targets);

    SubGrid<RadVec<N>, 2> rad_out(rad.x, 0, ip);
    SubGrid<TraMat<N>, 2> T_out(rad.T, 0, ip);
    SubGrid<RadVec<N>, 3> jac_out1(rad.dx, 1, ip + 1);
    SubGrid<RadVec<N>, 3> jac_out2(rad.dx, 1, ip);
    const ConstSubGrid<RadVec<N>, 2> rad_in(rad.x, 0, ip + 1);
    forward_rte_step(rad_out, T_out, jac_out1, jac_out2, rad_in, calcs.f_grid,
                     calcs.path[ip + 1], calcs.path[ip], K1, K2, S1, S2,
                     calcs.targets);

//     for (size_t iv = 0; iv < rad0.size(); iv++) {
//       for (size_t id = 0; id < calcs.targets.size(); id++) {
//         for (size_t ip2 = ip + 2; ip2 < calcs.path.size(); ip2++) {
//           rad.dx(id, ip2, iv) = rad.T(ip, iv) * rad.dx(id, ip2, iv);
//         }
//       }
//     }
  }

  for (size_t iv = 0; iv < calcs.f_grid.size(); iv++) {
    auto T = TraMat<N>::Identity();
    for (size_t ip = 0; ip < calcs.path.size()-2; ip++) {
      T = T * rad.T(ip, iv);
      for (size_t id = 0; id < calcs.targets.size(); id++) {
        rad.dx(id, ip+2, iv) = T * rad.dx(id, ip+2, iv);
      }
    }
  }

  return rad;
}

Results<1> compute(const std::vector<RadVec<1>>& rad0,
                   const Calculations& calcs) {
  return internal_compute(rad0, calcs);
}

Results<2> compute(const std::vector<RadVec<2>>& rad0,
                   const Calculations& calcs) {
  return internal_compute(rad0, calcs);
}

Results<3> compute(const std::vector<RadVec<3>>& rad0,
                   const Calculations& calcs) {
  return internal_compute(rad0, calcs);
}

Results<4> compute(const std::vector<RadVec<4>>& rad0,
                   const Calculations& calcs) {
  return internal_compute(rad0, calcs);
}

}  // namespace Forward
}  // namespace RTE
