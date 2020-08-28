#include "forward.h"

#include "mathhelpers.h"

namespace RTE {
namespace ForwardCalculations {
template <size_t N>
Results<N> internal_compute(const std::vector<RadVec<N>>& rad0,
                            const std::vector<Path::Point>& path,
                            const std::vector<Absorption::Band>& bands,
                            const Frequency<FrequencyType::Freq> flow,
                            const Frequency<FrequencyType::Freq> fupp,
                            const size_t size) {
  if (rad0.size() not_eq size) throw std::logic_error("Bad size");

  // Frequency vector
  const std::vector<double> f{linspace(flow, fupp, size)};

  // Output
  Results<N> rad(rad0, {}, path, f, {});

  // Propagation matrix on levels before (1) and after (2)
  Absorption::Xsec::Results<N> K1(size), K2(size);
  Absorption::Xsec::Results<N> S1(size), S2(size);

  // Compute the point farthest from the sensor and set it as starting point
  // (FIXME: Only LTE at this point)
  Absorption::Xsec::compute(K1, S1, f, bands, path[path.size() - 1]);

  // For all path points starting at the second to last
  for (size_t ip = path.size() - 2; ip >= 0; ip--) {
    // Compute the end-point of the path
    Absorption::Xsec::compute(K2, S2, f, bands, path[ip]);

    for (size_t iv = 0; iv < size; iv++) {
      auto J1 = source(K1.x[iv], RadVec<N>{S1.x[iv]}, RadVec<N>{K1.x[iv]},
                       B(path[ip + 1].atm.Temp(), f[iv]));
      auto J2 = source(K2.x[iv], RadVec<N>{S2.x[iv]}, RadVec<N>{K2.x[iv]},
                       B(path[ip].atm.Temp(), f[iv]));
      auto J = 0.5 * (J1 + J2);
      auto T = linear_transmat(K1.x[iv], K2.x[iv], 1.0);
      rad.x(ip, iv) = update(rad.x(ip + 1, iv), T, J);
    }

    // Lastly, swap the points
    std::swap(K1, K2);
    std::swap(S1, S2);
  }

  return rad;
}

Results<1> compute(const std::vector<RadVec<1>>& rad0,
                   const std::vector<Path::Point>& path,
                   const std::vector<Absorption::Band>& bands,
                   const Frequency<FrequencyType::Freq> flow,
                   const Frequency<FrequencyType::Freq> fupp,
                   const size_t size) {
  return internal_compute(rad0, path, bands, flow, fupp, size);
}

Results<2> compute(const std::vector<RadVec<2>>& rad0,
                   const std::vector<Path::Point>& path,
                   const std::vector<Absorption::Band>& bands,
                   const Frequency<FrequencyType::Freq> flow,
                   const Frequency<FrequencyType::Freq> fupp,
                   const size_t size) {
  return internal_compute(rad0, path, bands, flow, fupp, size);
}

Results<3> compute(const std::vector<RadVec<3>>& rad0,
                   const std::vector<Path::Point>& path,
                   const std::vector<Absorption::Band>& bands,
                   const Frequency<FrequencyType::Freq> flow,
                   const Frequency<FrequencyType::Freq> fupp,
                   const size_t size) {
  return internal_compute(rad0, path, bands, flow, fupp, size);
}

Results<4> compute(const std::vector<RadVec<4>>& rad0,
                   const std::vector<Path::Point>& path,
                   const std::vector<Absorption::Band>& bands,
                   const Frequency<FrequencyType::Freq> flow,
                   const Frequency<FrequencyType::Freq> fupp,
                   const size_t size) {
  return internal_compute(rad0, path, bands, flow, fupp, size);
}

}  // namespace ForwardCalculations
}  // namespace RTE
