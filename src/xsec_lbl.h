#ifndef xsec_lbl_h
#define xsec_lbl_h

#include "atmpath.h"
#include "lbl.h"
#include "lineshapes.h"
#include "propmat.h"

namespace Absorption {
namespace Xsec {
namespace Lbl {
struct Results {
  std::vector<Complex> x;
  std::vector<std::vector<Complex>> dx;

  Results(size_t nfreq = 0, size_t njac = 0) noexcept
      : x(nfreq, Complex{0, 0}), dx(njac, x) {}
};

void compute(Results& res, Results& src, Results& comp,
             const std::vector<double>& f, const Band& band,
             const Path::Point& atm,
             const Polarization polarization = Polarization::None);
}  // namespace Lbl
}  // namespace Xsec
}  // namespace Absorption

#endif  // xsec_lbl_h
