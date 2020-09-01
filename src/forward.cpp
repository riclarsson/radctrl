#include "forward.h"

#include "mathhelpers.h"

namespace RTE {
namespace Forward {
template <size_t N>
Results<N> internal_compute(const std::vector<RadVec<N>> &rad0,
                            const Calculations &calcs) {
  // Output
  Results<N> rad(rad0, calcs);

  // Propagation matrix on levels before (1) and after (2)
  Absorption::PropagationMatrix::Results<N> K1(rad0.size()), K2(rad0.size());
  Absorption::PropagationMatrix::Results<N> S1(rad0.size()), S2(rad0.size());

  // Compute the point farthest from the sensor and set it as starting point
  // (FIXME: Only LTE at this point)
  Absorption::PropagationMatrix::compute(K2, S2, calcs.f_grid, calcs.bands,
                                         calcs.path[calcs.path.size() - 1]);

  // For all path points starting at the second to last
  for (size_t ip = calcs.path.size() - 2; ip < calcs.path.size() - 1; ip--) {
    // First, swap the points
    std::swap(K1, K2);
    std::swap(S1, S2);

    // Compute the end-point of the path
    Absorption::PropagationMatrix::compute(K2, S2, calcs.f_grid, calcs.bands,
                                           calcs.path[ip]);

    for (size_t iv = 0; iv < rad0.size(); iv++) {
      auto J1 = source(K1.x[iv], RadVec<N>{S1.x[iv]}, RadVec<N>{K1.x[iv]},
                       B(calcs.path[ip + 1].atm.Temp(), calcs.f_grid[iv]));
      auto J2 = source(K2.x[iv], RadVec<N>{S2.x[iv]}, RadVec<N>{K2.x[iv]},
                       B(calcs.path[ip].atm.Temp(), calcs.f_grid[iv]));
      auto J = 0.5 * (J1 + J2);
      auto T = linear_transmat(K1.x[iv], K2.x[iv],
                               dist(calcs.path[ip], calcs.path[ip + 1]));
      rad.x(ip, iv) = update(rad.x(ip + 1, iv), T, J);
    }
  }

  return rad;
}

Results<1> compute(const std::vector<RadVec<1>> &rad0,
                   const Calculations &calcs) {
  return internal_compute(rad0, calcs);
}

Results<2> compute(const std::vector<RadVec<2>> &rad0,
                   const Calculations &calcs) {
  return internal_compute(rad0, calcs);
}

Results<3> compute(const std::vector<RadVec<3>> &rad0,
                   const Calculations &calcs) {
  return internal_compute(rad0, calcs);
}

Results<4> compute(const std::vector<RadVec<4>> &rad0,
                   const Calculations &calcs) {
  return internal_compute(rad0, calcs);
}

}  // namespace Forward
}  // namespace RTE
