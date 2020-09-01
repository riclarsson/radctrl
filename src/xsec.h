#ifndef xsec_h
#define xsec_h

#include "atmpath.h"
#include "lbl.h"
#include "propmat.h"

namespace Absorption {
namespace PropagationMatrix {

template <size_t N>
struct Results {
  std::vector<PropMat<N>> x;
  std::vector<std::vector<PropMat<N>>> dx;

  Results(size_t nfreq = 0, size_t njac = 0) noexcept
      : x(nfreq, PropMat<N>()), dx(njac, x) {}
};

void compute(Results<1> &, Results<1> &,
             const std::vector<Frequency<FrequencyType::Freq>> &,
             const std::vector<Band> &, const Path::Point &);
void compute(Results<2> &, Results<2> &,
             const std::vector<Frequency<FrequencyType::Freq>> &,
             const std::vector<Band> &, const Path::Point &);
void compute(Results<3> &, Results<3> &,
             const std::vector<Frequency<FrequencyType::Freq>> &,
             const std::vector<Band> &, const Path::Point &);
void compute(Results<4> &, Results<4> &,
             const std::vector<Frequency<FrequencyType::Freq>> &,
             const std::vector<Band> &, const Path::Point &);
}  // namespace PropagationMatrix
}  // namespace Absorption

#endif  // absorption_h
