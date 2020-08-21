#ifndef xsec_h
#define xsec_h

#include "lbl.h"

namespace Absorption {
namespace Xsec {
  void compute(std::vector<Complex>& xsec,
               const std::vector<double>& f,
               std::vector<Band>& bands);
}
}

#endif  // absorption_h
