#ifndef inverse_h
#define inverse_h

#include "atm.h"
#include "forward.h"

namespace Inverse {
  ENUMCLASS(BeamType, char, PencilBeam)
  ENUMCLASS(Stokes, char, Full, None)
  
  void compute(const Atmosphere::Atm& atm,
               const Geom::Nav& pos_los,
               const std::vector<Absorption::Band>& bands,
               const std::vector<Derivative::Target>& derivs,
               const std::vector<Frequency<FrequencyType::Freq>>& sensor_f_grid,
               const Stokes stokes_dimension,
               const BeamType beamtype);
};

#endif  // inverse_h
