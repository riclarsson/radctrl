#ifndef arts_amtosphere_h
#define arts_amtosphere_h

#include <autoarts.h>

namespace ARTS::Atmosphere::Earth {
/*! Fascod midlatitude winter */
Workspace& fascod_midlatitude_winter(Workspace& ws);

/*! Fascod tropical */
Workspace& fascod_tropical(Workspace& ws);

/*! Fascod subarctic winter */
Workspace& fascod_subarctic_winter(Workspace& ws);

/*! Fascod midlatitude summer */
Workspace& fascod_midlatitude_summer(Workspace& ws);

/*! Fascod subarctic summer */
Workspace& fascod_subarctic_summer(Workspace& ws);
}  // namespace ARTS::Atmosphere::Earth

#endif  // arts_amtosphere_h
