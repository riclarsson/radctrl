#include "inverse.h"

namespace Inverse {
  void compute(const Atmosphere::Atm& atm,
               const Geom::Nav& pos_los,
               const std::vector<Absorption::Band>& bands,
               const std::vector<Derivative::Target>& derivs,
               const std::vector<Frequency<FrequencyType::Freq>>& sensor_f_grid,
               const Stokes stokes_dimension,
               const BeamType beamtype) 
               {}
}
