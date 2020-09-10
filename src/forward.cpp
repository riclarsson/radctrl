#include "forward.h"

#include "mathhelpers.h"

namespace RTE {
namespace Forward {
template <size_t N>
void forward_rte_step(Sub<Grid<RadVec<N>, 2>>& rad_out,
                      Sub<Grid<TraMat<N>, 2>>& T_out,
                      Sub<Grid<RadVec<N>, 3>>& jac_out1,
                      Sub<Grid<RadVec<N>, 3>>& jac_out2,
                      const Sub<Grid<RadVec<N>, 2>>& rad_in,
                      const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                      const Path::Point& p1, const Path::Point& p2,
                      const Absorption::PropagationMatrix::Results<N>& K1,
                      const Absorption::PropagationMatrix::Results<N>& K2,
                      const Absorption::PropagationMatrix::Results<N>& S1,
                      const Absorption::PropagationMatrix::Results<N>& S2,
                      const Absorption::PropagationMatrix::Results<N>& A1,
                      const Absorption::PropagationMatrix::Results<N>& A2,
                      const std::vector<Derivative::Target>& derivs) {
  for (size_t iv = 0; iv < f_grid.size(); iv++) {
    auto B1 = B(p1.atm.Temp(), f_grid[iv]);
    auto B2 = B(p2.atm.Temp(), f_grid[iv]);
    auto J =
        0.5 * (source(K1.x[iv], RadVec<N>{S1.x[iv]}, RadVec<N>{A1.x[iv]}, B1) +
               source(K2.x[iv], RadVec<N>{S2.x[iv]}, RadVec<N>{A2.x[iv]}, B2));
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

      auto dJ1 = 0.5 * dsource(K1.x[iv], K1.dx[id][iv], RadVec<N>{S1.x[iv]},
                               RadVec<N>{S1.dx[id][iv]}, RadVec<N>{A1.x[iv]},
                               RadVec<N>{A1.dx[id][iv]}, B1, dB1);
      auto dJ2 = 0.5 * dsource(K2.x[iv], K2.dx[id][iv], RadVec<N>{S2.x[iv]},
                               RadVec<N>{S2.dx[id][iv]}, RadVec<N>{A2.x[iv]},
                               RadVec<N>{A2.dx[id][iv]}, B2, dB2);
      auto dT1 = dlinear_transmat(T_out[iv], K1.x[iv], K2.x[iv], K1.dx[id][iv],
                                  r, 0.0);
      auto dT2 = dlinear_transmat(T_out[iv], K1.x[iv], K2.x[iv], K2.dx[id][iv],
                                  r, 0.0);
      jac_out1(id, iv) = dupdate(rad_in[iv], T_out[iv], dT1, J, dJ1) +
                         T_out[iv] * jac_out1(id, iv);
      jac_out2(id, iv) = dupdate(rad_in[iv], T_out[iv], dT2, J, dJ2);
    }
  }
}

template <size_t N>
Results<N> internal_compute(
    const std::vector<RadVec<N>>& rad0,
    const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
    const std::vector<Derivative::Target>& targets,
    const std::vector<Absorption::Band>& bands,
    const std::vector<Path::Point>& path) {
  const size_t np = path.size();
  const size_t nf = f_grid.size();
  const size_t nt = targets.size();

  // Output
  Results<N> rad(rad0, np, nf, nt);

  // Propagation matrix on levels before (1) and after (2)
  Absorption::PropagationMatrix::Results<N> K1(rad0.size(), nt), K2(K1), S1(K1),
      S2(K1), A1(K1), A2(K1);

  // Compute the point farthest from the sensor and set it as starting point
  // (FIXME: Only LTE at this point)
  Absorption::PropagationMatrix::compute(K2, S2, /*A2,*/ f_grid, bands,
                                         path[np - 1], targets);

  // For all path points starting at the second to last (first is computed
  // already
  for (size_t ip = np - 2; ip < np - 1;
       ip--) {  // nb. under-flow → -1 == MAX size_t
    // First, swap the point of the atmosphere so transmission is 1 → 2
    std::swap(K1, K2);
    std::swap(S1, S2);
    std::swap(A1, A2);

    // Compute the current point of the path
    Absorption::PropagationMatrix::compute(K2, S2, /*A2,*/ f_grid, bands,
                                           path[ip], targets);

    // Extract and compute this level
    Sub rad_out(rad.x, 0, ip);
    Sub T_out(rad.T, 0, ip);
    Sub jac_out1(rad.dx, 1, ip + 1);
    Sub jac_out2(rad.dx, 1, ip);
    const Sub rad_in(rad.x, 0, ip + 1);
    forward_rte_step(rad_out, T_out, jac_out1, jac_out2, rad_in, f_grid,
                     path[ip + 1], path[ip], K1, K2, S1, S2, A1, A2, targets);
  }

  // Fix Jacobian transmission
  if (nt) {
    for (size_t iv = 0; iv < nf; iv++) {
      auto T = TraMat<N>::Identity();
      for (size_t ip = 0; ip < np - 1; ip++) {
        T = rad.T(ip, iv) *
            T;  // FIXME: Confirm multiplication order for polarization
        for (size_t id = 0; id < nt; id++) {
          rad.dx(id, ip + 1, iv) = T * rad.dx(id, ip + 1, iv);
        }
      }
    }
  }

  return rad;
}

Results<1> compute(const std::vector<RadVec<1>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path) {
  return internal_compute(rad0, f_grid, targets, bands, path);
}

Results<2> compute(const std::vector<RadVec<2>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path) {
  return internal_compute(rad0, f_grid, targets, bands, path);
}

Results<3> compute(const std::vector<RadVec<3>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path) {
  return internal_compute(rad0, f_grid, targets, bands, path);
}

Results<4> compute(const std::vector<RadVec<4>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path) {
  return internal_compute(rad0, f_grid, targets, bands, path);
}

}  // namespace Forward
}  // namespace RTE
