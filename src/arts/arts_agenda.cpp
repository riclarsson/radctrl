#include "arts_agenda.h"

namespace ARTS::Agenda {
Workspace& iy_main_agenda_emission(Workspace& ws) {
  AgendaDefine::iy_main_agenda(ws, AgendaMethod::ppathCalc(ws),
                               AgendaMethod::iyEmissionStandard(ws));
  return ws;
}

Workspace& iy_main_agenda_transmission(Workspace& ws) {
  AgendaDefine::iy_main_agenda(
      ws, AgendaMethod::Ignore(ws, AgendaVar::iy_unit(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::iy_id(ws)),
      AgendaMethod::ppathCalc(ws), AgendaMethod::iyTransmissionStandard(ws));
  return ws;
}

Workspace& iy_space_agenda_cosmic_background(Workspace& ws) {
  AgendaDefine::iy_space_agenda(
      ws, AgendaMethod::Ignore(ws, AgendaVar::rtp_pos(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::rtp_los(ws)),
      AgendaMethod::MatrixCBR(ws, AgendaVar::iy(ws), AgendaVar::f_grid(ws)));
  return ws;
}

Workspace& iy_surface_agenda_use_surface_property(Workspace& ws) {
  AgendaDefine::iy_surface_agenda(ws, AgendaMethod::SurfaceDummy(ws),
                                  AgendaMethod::iySurfaceRtpropAgenda(ws));
  return ws;
}

Workspace& ppath_agenda_follow_sensor_los(Workspace& ws) {
  AgendaDefine::ppath_agenda(ws,
                             AgendaMethod::Ignore(ws, AgendaVar::rte_pos2(ws)),
                             AgendaMethod::ppathStepByStep(ws));
  return ws;
}

Workspace& ppath_agenda_plane_parallel(Workspace& ws) {
  AgendaDefine::ppath_agenda(
      ws, AgendaMethod::Ignore(ws, AgendaVar::ppath_lraytrace(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::rte_pos2(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::t_field(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::vmr_field(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::f_grid(ws)),
      AgendaMethod::ppathPlaneParallel(ws));
  return ws;
}

Workspace& ppath_step_agenda_geometric_path(Workspace& ws) {
  AgendaDefine::ppath_step_agenda(
      ws, AgendaMethod::Ignore(ws, AgendaVar::ppath_lraytrace(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::f_grid(ws)),
      AgendaMethod::ppath_stepGeometric(ws));
  return ws;
}

Workspace& ppath_step_agenda_refracted_path(Workspace& ws) {
  AgendaDefine::ppath_step_agenda(ws,
                                  AgendaMethod::ppath_stepRefractionBasic(ws));
  return ws;
}

Workspace& propmat_clearsky_agenda_on_the_fly(Workspace& ws) {
  AgendaDefine::propmat_clearsky_agenda(
      ws, AgendaMethod::Ignore(ws, AgendaVar::rtp_mag(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::rtp_los(ws)),
      AgendaMethod::propmat_clearskyInit(ws),
      AgendaMethod::propmat_clearskyAddOnTheFly(ws));
  return ws;
}

Workspace& propmat_clearsky_agenda_on_the_fly_zeeman(Workspace& ws) {
  AgendaDefine::propmat_clearsky_agenda(
      ws, AgendaMethod::propmat_clearskyInit(ws),
      AgendaMethod::propmat_clearskyAddOnTheFly(ws),
      AgendaMethod::propmat_clearskyAddZeeman(ws));
  return ws;
}

Workspace& abs_xsec_agenda_standard(Workspace& ws) {
  AgendaDefine::abs_xsec_agenda(ws, AgendaMethod::abs_xsec_per_speciesInit(ws),
                                AgendaMethod::abs_xsec_per_speciesAddLines(ws),
                                AgendaMethod::abs_xsec_per_speciesAddConts(ws));
  return ws;
}

Workspace& abs_xsec_agenda_standard_with_cia(Workspace& ws) {
  AgendaDefine::abs_xsec_agenda(ws, AgendaMethod::abs_xsec_per_speciesInit(ws),
                                AgendaMethod::abs_xsec_per_speciesAddLines(ws),
                                AgendaMethod::abs_xsec_per_speciesAddConts(ws),
                                AgendaMethod::abs_xsec_per_speciesAddCIA(ws));
  return ws;
}

Workspace& surface_rtprop_agenda_blackbody_from_surface(Workspace& ws) {
  AgendaDefine::surface_rtprop_agenda(
      ws,
      AgendaMethod::InterpSurfaceFieldToPosition(
          ws, AgendaVar::surface_skin_t(ws), AgendaVar::t_surface(ws)),
      AgendaMethod::surfaceBlackbody(ws));
  return ws;
}

Workspace& surface_rtprop_agenda_blackbody_from_atmosphere(Workspace& ws) {
  AgendaDefine::surface_rtprop_agenda(
      ws,
      AgendaMethod::InterpAtmFieldToPosition(ws, AgendaVar::surface_skin_t(ws),
                                             AgendaVar::t_field(ws)),
      AgendaMethod::surfaceBlackbody(ws));
  return ws;
}
}  // namespace ARTS::Agenda
