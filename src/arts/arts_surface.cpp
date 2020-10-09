#include "arts_surface.h"

namespace ARTS::Surface {
/*! Ignore the surface */
Workspace& ignore(Workspace& ws) {
  Method::Touch(ws, Var::surface_props_names(ws));
  Method::Touch(ws, Var::surface_props_data(ws));
  return ws;
}
}  // namespace ARTS::Surface
