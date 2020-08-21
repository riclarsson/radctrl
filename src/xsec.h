#ifndef xsec_h
#define xsec_h

#include "atmpath.h"
#include "lbl.h"
#include "propmat.h"

namespace Absorption {
namespace Xsec {
void compute(std::vector<PropMat<1>>&, const std::vector<double>&,
             const std::vector<Band>&, const Path::Point&);
void compute(std::vector<PropMat<2>>&, const std::vector<double>&,
             const std::vector<Band>&, const Path::Point&);
void compute(std::vector<PropMat<3>>&, const std::vector<double>&,
             const std::vector<Band>&, const Path::Point&);
void compute(std::vector<PropMat<4>>&, const std::vector<double>&,
             const std::vector<Band>&, const Path::Point&);
}  // namespace Xsec
}  // namespace Absorption

#endif  // absorption_h
