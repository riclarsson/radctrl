#ifndef xsec_lbl_h
#define xsec_lbl_h

#include "propmat.h"
#include "lineshapes.h"
#include "lbl.h"
#include "atmpath.h"

namespace Absorption {
namespace Xsec {
namespace Lbl {
void compute(std::vector<Complex>& x,
             std::vector<Complex>& comp_x,
             const std::vector<double>& f,
             const Band& band,
             const Path::Point& atm,
             const Absorption::Zeeman::Polarization polarization=Absorption::Zeeman::Polarization::Pi);
}  // Lbl
}  // Xsec
}  // Absorption

#endif  // xsec_lbl_h
