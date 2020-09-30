#include <iostream>

#include "arts/interface.h"

int main() try {
  auto ws = ARTS::init();
  using namespace ARTS::Agenda;
  iy_main_agenda_emission(ws);
  iy_main_agenda_transmission(ws);
  iy_space_agenda_cosmic_background(ws);
  iy_surface_agenda_use_surface_property(ws);
  ppath_agenda_follow_sensor_los(ws);
  ppath_agenda_plane_parallel(ws);
  ppath_step_agenda_geometric_path(ws);
  ppath_step_agenda_refracted_path(ws);
  propmat_clearsky_agenda_on_the_fly(ws);
  propmat_clearsky_agenda_on_the_fly_zeeman(ws);
  abs_xsec_agenda_standard(ws);
  abs_xsec_agenda_standard_with_cia(ws);
  surface_rtprop_agenda_blackbody_from_surface(ws);
  surface_rtprop_agenda_blackbody_from_atmosphere(ws);
  
  std::cout << "Hello!\n";
} catch(const std::exception& e) {
  std::cerr << "EXITING WITH ERROR:\n" << e.what() << '\n';
  return 1;
}
