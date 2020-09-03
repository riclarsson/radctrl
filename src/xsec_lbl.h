#ifndef xsec_lbl_h
#define xsec_lbl_h

#include "atmpath.h"
#include "derivatives.h"
#include "lbl.h"
#include "lineshapes.h"
#include "propmat.h"

namespace Absorption::Xsec::Lbl {
struct Results {
  std::vector<Complex> x;
  std::vector<std::vector<Complex>> dx;

  Results(size_t nfreq = 0, size_t njac = 0) noexcept
      : x(nfreq, Complex{0, 0}), dx(njac, x) {}
};

void compute(Results &res, Results &src, Results &comp,
             const std::vector<Frequency<FrequencyType::Freq>> &f,
             const Band &band, const Path::Point &atm,
             const std::vector<Derivative::Target> &derivs,
             const Polarization polarization = Polarization::None);
}  // namespace Absorption::Xsec::Lbl

#endif  // xsec_lbl_h
