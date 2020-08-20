#ifndef xsec_lbl_h
#define xsec_lbl_h

#include "atmpath.h"
#include "lbl.h"
#include "lineshapes.h"
#include "propmat.h"

namespace Absorption {
namespace Xsec {
namespace Lbl {
void compute(std::vector<Complex>& x, std::vector<Complex>& comp_x,
             const std::vector<double>& f, const Band& band,
             const Path::Point& atm,
             const Absorption::Zeeman::Polarization polarization =
                 Absorption::Zeeman::Polarization::Pi);
}  // namespace Lbl
}  // namespace Xsec
}  // namespace Absorption

#endif  // xsec_lbl_h
