#ifndef arts_continua_h
#define arts_continua_h

#include <autoarts.h>

namespace ARTS::Continua {
/*! Ignore the continua */
Workspace& ignore(Workspace& ws);

/*! Initialize the continua */
Workspace& init(Workspace& ws);

/*! Add H2O-PWR98 to the continua model list */
Workspace& addH2OPWR98(Workspace& ws);

/*! Add O2-PWR98 to the continua model list */
Workspace& addO2PWR98(Workspace& ws);

/*! Add N2-SelfContStandardType to the continua model list */
Workspace& addN2SelfContStandardType(Workspace& ws);
}  // namespace ARTS::Continua

#endif  // arts_continua_h
