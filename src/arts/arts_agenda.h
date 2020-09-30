#ifndef arts_agendas_h
#define arts_agendas_h

#include <autoarts.h>
namespace ARTS::Agenda {
Workspace& iy_main_agenda_emission(Workspace& ws);
Workspace& iy_main_agenda_transmission(Workspace& ws);
Workspace& iy_space_agenda_cosmic_background(Workspace& ws);
Workspace& iy_surface_agenda_use_surface_property(Workspace& ws);
Workspace& ppath_agenda_follow_sensor_los(Workspace& ws);
Workspace& ppath_agenda_plane_parallel(Workspace& ws);
Workspace& ppath_step_agenda_geometric_path(Workspace& ws);
Workspace& ppath_step_agenda_refracted_path(Workspace& ws);
Workspace& propmat_clearsky_agenda_on_the_fly(Workspace& ws);
Workspace& propmat_clearsky_agenda_on_the_fly_zeeman(Workspace& ws);
Workspace& abs_xsec_agenda_standard(Workspace& ws);
Workspace& abs_xsec_agenda_standard_with_cia(Workspace& ws);
Workspace& surface_rtprop_agenda_blackbody_from_surface(Workspace& ws);
Workspace& surface_rtprop_agenda_blackbody_from_atmosphere(Workspace& ws);
};  // namespace ARTS::Agenda

#endif  // arts_agendas_a
