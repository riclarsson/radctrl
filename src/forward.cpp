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

    for (size_t iv = 0; iv < rad0.size(); iv++) {
      auto B1 = B(calcs.path[ip + 1].atm.Temp(), calcs.f_grid[iv]);
      auto B2 = B(calcs.path[ip].atm.Temp(), calcs.f_grid[iv]);
      auto J = 0.5 *
               (source(K1.x[iv], RadVec<N>{S1.x[iv]}, RadVec<N>{K1.x[iv]}, B1) +
                source(K2.x[iv], RadVec<N>{S2.x[iv]}, RadVec<N>{K2.x[iv]}, B2));
      auto r = dist(calcs.path[ip], calcs.path[ip + 1]);
      auto T = linear_transmat(K1.x[iv], K2.x[iv], r);
      rad.x(ip, iv) = update(rad.x(ip + 1, iv), T, J);

      for (size_t id = 0; id < calcs.targets.size(); id++) {
        auto dB1 =
            calcs.targets[id] == Derivative::Atm::Temperature
                ? dBdT(calcs.path[ip + 1].atm.Temp(), calcs.f_grid[iv])
                : calcs.targets[id].isWind()
                      ? dBdf(calcs.path[ip + 1].atm.Temp(), calcs.f_grid[iv])
                      : decltype(B1)(0);
        auto dB2 = calcs.targets[id] == Derivative::Atm::Temperature
                       ? dBdT(calcs.path[ip].atm.Temp(), calcs.f_grid[iv])
                       : calcs.targets[id].isWind()
                             ? dBdf(calcs.path[ip].atm.Temp(), calcs.f_grid[iv])
                             : decltype(B2)(0);
        auto dJ1 =
            dsource(K1.x[iv], K1.dx[id][iv], J, RadVec<N>{S1.dx[id][iv]},
                    RadVec<N>{K1.x[iv]}, RadVec<N>{K2.dx[id][iv]}, B1, dB1);
        auto dJ2 =
            dsource(K2.x[iv], K2.dx[id][iv], J, RadVec<N>{S2.dx[id][iv]},
                    RadVec<N>{K2.x[iv]}, RadVec<N>{K2.dx[id][iv]}, B2, dB2);
        auto dT1 =
            dlinear_transmat(T, K1.x[iv], K2.x[iv], K1.dx[id][iv], r, 0.0);
        auto dT2 =
            dlinear_transmat(T, K1.x[iv], K2.x[iv], K2.dx[id][iv], r, 0.0);

        rad.dx(id * calcs.targets.size() + ip + 1, iv) =
            dupdate(rad.x(ip + 1, iv), T, dT1, J, dJ1);
        rad.dx(id * calcs.targets.size() + ip, iv) =
            dupdate(rad.x(ip + 1, iv), T, dT2, J, dJ2) +
            T * rad.dx(id * calcs.targets.size() + ip, iv);

        for (size_t ip2 = ip + 2; ip2 < calcs.path.size(); ip2++) {
          rad.dx(id * calcs.targets.size() + ip2, iv) =
              T * rad.dx(id * calcs.targets.size() + ip2, iv);
        }
        //         if (iv == 0)  std::cout << T << '\n';
      }
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
