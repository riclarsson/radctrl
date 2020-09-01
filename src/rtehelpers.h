#include "mathhelpers.h"
#include "rte.h"

namespace RTE {
template <size_t N>
std::vector<RadVec<N>> source_vec_planck(
    Temperature<TemperatureType::K> T,
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
std::vector<RadVec<N>> to_planck(
    const std::vector<RadVec<N>>& rad,
    const std::vector<Frequency<FrequencyType::Freq>>& fvec) {
  std::vector<RadVec<N>> out(fvec.size());

  if constexpr (N == 1)
    std::transform(rad.cbegin(), rad.cend(), fvec.cbegin(), out.begin(),
                   [](auto& I, auto& f) { return RadVec<1>{invB(I[0], f)}; });
  else if constexpr (N == 2)
    std::transform(rad.cbegin(), rad.cend(), fvec.cbegin(), out.begin(),
                   [](auto& I, auto& f) {
                     return RadVec<2>{invB(I[0], f),
                                      invB(0.5 * (I[0] + I[1]), f) -
                                          invB(0.5 * (I[0] - I[1]), f)};
                   });
  else if constexpr (N == 3)
    std::transform(
        rad.cbegin(), rad.cend(), fvec.cbegin(), out.begin(),
        [](auto& I, auto& f) {
          return RadVec<3>{
              invB(I[0], f),
              invB(0.5 * (I[0] + I[1]), f) - invB(0.5 * (I[0] - I[1]), f),
              invB(0.5 * (I[0] + I[2]), f) - invB(0.5 * (I[0] - I[2]), f)};
        });
  else if constexpr (N == 4)
    std::transform(
        rad.cbegin(), rad.cend(), fvec.cbegin(), out.begin(),
        [](auto& I, auto& f) {
          return RadVec<4>{
              invB(I[0], f),
              invB(0.5 * (I[0] + I[1]), f) - invB(0.5 * (I[0] - I[1]), f),
              invB(0.5 * (I[0] + I[2]), f) - invB(0.5 * (I[0] - I[2]), f),
              invB(0.5 * (I[0] + I[3]), f) - invB(0.5 * (I[0] - I[3]), f)};
        });
  return out;
}
}  // namespace RTE
