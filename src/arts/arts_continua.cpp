#include "arts_continua.h"

namespace ARTS::Continua {
Workspace& ignore(Workspace& ws) {
  Method::Touch(ws, Var::abs_cont_names(ws));
  Method::Touch(ws, Var::abs_cont_models(ws));
  Method::Touch(ws, Var::abs_cont_parameters(ws));
  return ws;
}

Workspace& init(Workspace& ws) {
  Method::abs_cont_descriptionInit(ws);
  return ws;
}

Workspace& addH2OPWR98(Workspace& ws) {
  Method::abs_cont_descriptionAppend(ws, Group::String{"H2O-PWR98"},
                                     Group::String{"Rosenkranz"});
  return ws;
}

Workspace& addO2PWR98(Workspace& ws) {
  Method::abs_cont_descriptionAppend(ws, Group::String{"O2-PWR98"},
                                     Group::String{"Rosenkranz"});
  return ws;
}

Workspace& addN2SelfContStandardType(Workspace& ws) {
  Method::abs_cont_descriptionAppend(ws,
                                     Group::String{"N2-SelfContStandardType"},
                                     Group::String{"Rosenkranz"});
  return ws;
}
}  // namespace ARTS::Continua
