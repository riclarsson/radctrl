#ifndef arts_agendas_h
#define arts_agendas_h

#include <autoarts.h>
namespace ARTS::Agenda {
/*! Standard emission agenda */
Workspace& iy_main_agenda_emission(Workspace& ws);

/*! Standard transmission agenda */
Workspace& iy_main_agenda_transmission(Workspace& ws);

/*! Standard cosmic background agenda */
Workspace& iy_space_agenda_cosmic_background(Workspace& ws);

/*! Standard surface properties agenda */
Workspace& iy_surface_agenda_use_surface_property(Workspace& ws);

/*! Standard ppath agenda to just follow line-of-sight agenda */
Workspace& ppath_agenda_follow_sensor_los(Workspace& ws);

/*! Standard ppath agenda as plane parallell */
Workspace& ppath_agenda_plane_parallel(Workspace& ws);

/*! Standard ppath geometric stepping agenda */
Workspace& ppath_step_agenda_geometric_path(Workspace& ws);

/*! Standard ppath refracted stepping agenda */
Workspace& ppath_step_agenda_refracted_path(Workspace& ws);

/*! Standard propagation matrix for clearsky agenda */
Workspace& propmat_clearsky_agenda_on_the_fly(Workspace& ws);

/*! Standard propagation matrix for clearsky with Zeeman agenda */
Workspace& propmat_clearsky_agenda_on_the_fly_zeeman(Workspace& ws);

/*! Standard cross-section agenda with continua and line absorption */
Workspace& abs_xsec_agenda_standard(Workspace& ws);

/*! Standard cross-section agenda with continua and line absorption and CIA */
Workspace& abs_xsec_agenda_standard_with_cia(Workspace& ws);

/*! Standard surface properties is that of a blackbody from surface temperature
 */
Workspace& surface_rtprop_agenda_blackbody_from_surface(Workspace& ws);

/*! Standard surface properties is that of a blackbody from surface temperatures
 * extracted from lower-bound atmospheric temperatures */
Workspace& surface_rtprop_agenda_blackbody_from_atmosphere(Workspace& ws);

/*! Standard empty geo pos agenda */
Workspace& geo_pos_agenda_empty(Workspace& ws);

/*! Standard water_p_eq_agenda */
Workspace& water_p_eq_agenda_default(Workspace& ws);
};  // namespace ARTS::Agenda

#endif  // arts_agendas_a
