#ifndef xsec_lbl_h
#define xsec_lbl_h

#include "propmat.h"
#include "lineshapes.h"
#include "lbl.h"
#include "atmpath.h"

namespace Absorption {
namespace Xsec {
  void compute(std::vector<LineStrength<FrequencyType::Freq, AreaType::m2>>& x,
               const std::vector<Frequency<FrequencyType::Freq>>& f,
               const Band& band,
               const Path::Point& atm,
               const Absorption::Zeeman::Polarization polarization);
}
}

#endif  // xsec_lbl_h
