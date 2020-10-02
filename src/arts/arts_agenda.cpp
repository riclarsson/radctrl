#include "arts_agenda.h"

namespace ARTS::Agenda {
Workspace& iy_main_agenda_emission(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  iy_main_agenda(ws, ppathCalc(ws), iyEmissionStandard(ws));
  return ws;
}

Workspace& iy_main_agenda_transmission(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  iy_main_agenda(ws, Ignore(ws, iy_unit(ws)), Ignore(ws, iy_id(ws)),
                 ppathCalc(ws), iyTransmissionStandard(ws));
  return ws;
}

Workspace& iy_space_agenda_cosmic_background(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  iy_space_agenda(ws, Ignore(ws, rtp_pos(ws)), Ignore(ws, rtp_los(ws)),
                  MatrixCBR(ws, iy(ws), f_grid(ws)));
  return ws;
}

Workspace& iy_surface_agenda_use_surface_property(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  iy_surface_agenda(ws, SurfaceDummy(ws), iySurfaceRtpropAgenda(ws));
  return ws;
}

Workspace& ppath_agenda_follow_sensor_los(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  ppath_agenda(ws, Ignore(ws, rte_pos2(ws)), ppathStepByStep(ws));
  return ws;
}

Workspace& ppath_agenda_plane_parallel(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  ppath_agenda(ws, Ignore(ws, ppath_lraytrace(ws)), Ignore(ws, rte_pos2(ws)),
               Ignore(ws, t_field(ws)), Ignore(ws, vmr_field(ws)),
               Ignore(ws, f_grid(ws)), ppathPlaneParallel(ws));
  return ws;
}

Workspace& ppath_step_agenda_geometric_path(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  ppath_step_agenda(ws, Ignore(ws, ppath_lraytrace(ws)), Ignore(ws, f_grid(ws)),
                    ppath_stepGeometric(ws));
  return ws;
}

Workspace& ppath_step_agenda_refracted_path(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  ppath_step_agenda(ws, ppath_stepRefractionBasic(ws));
  return ws;
}

Workspace& propmat_clearsky_agenda_on_the_fly(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  propmat_clearsky_agenda(ws, Ignore(ws, rtp_mag(ws)), Ignore(ws, rtp_los(ws)),
                          propmat_clearskyInit(ws),
                          propmat_clearskyAddOnTheFly(ws));
  return ws;
}

Workspace& propmat_clearsky_agenda_on_the_fly_zeeman(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  propmat_clearsky_agenda(ws, propmat_clearskyInit(ws),
                          propmat_clearskyAddOnTheFly(ws),
                          propmat_clearskyAddZeeman(ws));
  return ws;
}

Workspace& abs_xsec_agenda_standard(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  abs_xsec_agenda(ws, abs_xsec_per_speciesInit(ws),
                  abs_xsec_per_speciesAddLines(ws),
                  abs_xsec_per_speciesAddConts(ws));
  return ws;
}

Workspace& abs_xsec_agenda_standard_with_cia(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  abs_xsec_agenda(
      ws, abs_xsec_per_speciesInit(ws), abs_xsec_per_speciesAddLines(ws),
      abs_xsec_per_speciesAddConts(ws), abs_xsec_per_speciesAddCIA(ws));
  return ws;
}

Workspace& surface_rtprop_agenda_blackbody_from_surface(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  surface_rtprop_agenda(
      ws, InterpSurfaceFieldToPosition(ws, surface_skin_t(ws), t_surface(ws)),
      surfaceBlackbody(ws));
  return ws;
}

Workspace& surface_rtprop_agenda_blackbody_from_atmosphere(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  surface_rtprop_agenda(
      ws, InterpAtmFieldToPosition(ws, surface_skin_t(ws), t_field(ws)),
      surfaceBlackbody(ws));
  return ws;
}

Workspace& geo_pos_agenda_empty(Workspace& ws) {
  using namespace AgendaMethod;
  using namespace AgendaDefine;
  using namespace AgendaVar;
  geo_pos_agenda(ws, Ignore(ws, ppath(ws)),
                 VectorSet(ws, geo_pos(ws), VectorCreate(ws, {}, "Default")));
  return ws;
}
}  // namespace ARTS::Agenda
