#include "waspam_water_simulations.h"

namespace Waspam::Forward {
void compute() {
  using namespace ARTS;

  // Single core, silent, saving files by default prepends "waspam"
  auto ws = init(0, 0, 0, "waspam", 20);

  // Define the agendas
  ARTS::Agenda::iy_main_agenda_emission(ws);
  ARTS::Agenda::iy_space_agenda_cosmic_background(ws);
  ARTS::Agenda::iy_surface_agenda_use_surface_property(ws);
  ARTS::Agenda::ppath_agenda_follow_sensor_los(ws);
  ARTS::Agenda::ppath_step_agenda_geometric_path(ws);
  ARTS::Agenda::propmat_clearsky_agenda_on_the_fly(ws);
  ARTS::Agenda::abs_xsec_agenda_standard(ws);
  ARTS::Agenda::surface_rtprop_agenda_blackbody_from_surface(ws);
  ARTS::Agenda::geo_pos_agenda_empty(ws);
  ARTS::Agenda::water_p_eq_agenda_default(ws);

  Method::nlteOff(ws);
  Var::iy_unit(ws) = "PlanckBT";
  Method::Touch(ws, Var::iy_aux_vars(ws));

  // Surface
  ARTS::Surface::ignore(ws);

  // Continua
  ARTS::Continua::init(ws);
  ARTS::Continua::addO2PWR98(ws);
  ARTS::Continua::addN2SelfContStandardType(ws);

  Method::abs_speciesSet(
      ws, Group::ArrayOfString{"H2O", "N2-SelfContStandardType", "O2-PWR98"});

  Method::partition_functionsInitFromBuiltin(ws);
  Method::isotopologue_ratiosInitFromBuiltin(ws);

  Method::AtmosphereSet1D(ws);
  Var::lat_true(ws) = Var::lat_grid(ws);
  Var::lon_true(ws) = Var::lon_grid(ws);

  // Set atmosphere
  Atmosphere::Earth::fascod_subarctic_summer(ws);
  Method::Touch(ws, Var::wind_u_field(ws));
  Method::Touch(ws, Var::wind_v_field(ws));
  Method::Touch(ws, Var::wind_w_field(ws));
  Method::Touch(ws, Var::mag_u_field(ws));
  Method::Touch(ws, Var::mag_v_field(ws));
  Method::Touch(ws, Var::mag_w_field(ws));
  Method::Touch(ws, Var::nlte_field(ws));

  // Set planet and spin
  Method::refellipsoidEarth(ws, Group::String{"Sphere"});
  Method::Touch(ws, Var::rte_alonglos_v(ws));
  Method::z_surfaceConstantAltitude(ws);
  Var::t_surface(ws) = Group::Matrix(1, 1, 600.0);

  Method::Touch(ws, Var::abs_lines(ws));
  Method::abs_lines_per_speciesCreateFromLines(ws);

  Var::abs_f_interp_order(ws) = 1;
  Var::stokes_dim(ws) = 1;
  Var::ppath_lraytrace(ws) = 1e3;
  Var::ppath_lmax(ws) = 1e3;

  Method::VectorNLinSpace(ws, Var::f_grid(ws).value(), 101, 22e9 - 500e6, 22e9 + 500e6);

  Var::sensor_pos(ws) = Group::Matrix(1, 1, 100);
  Var::sensor_los(ws) = Group::Matrix(1, 1, 75);
  Method::Touch(ws, Var::transmitter_pos(ws));
  Method::sensorOff(ws);
  Method::cloudboxOff(ws);
  
  // Set up the Retrievals
  Method::retrievalDefInit(ws);
  Method::covmatDiagonal(ws, Var::covmat_block(ws), Var::covmat_inv_block(ws), Group::Vector(Var::p_grid(ws).value().nelem(), 1e-6));
  Method::retrievalAddAbsSpecies(ws, Var::p_grid(ws), Group::Vector{}, Group::Vector{}, Group::String{"H2O"}, Group::String{"vmr"}, 0);
  Method::covmatDiagonal(ws, Var::covmat_block(ws), Var::covmat_inv_block(ws), Group::Vector(2, 1e-4));
  Method::retrievalAddPolyfit(ws, 1, 1, 1, 1);
  Method::covmatDiagonal(ws, Var::covmat_block(ws), Var::covmat_inv_block(ws), Group::Vector(2, 1e-4));
  Method::retrievalAddSinefit(ws, Group::Vector{5e6, 10e6, 20e6, 40e6});
  Method::retrievalDefClose(ws);
  
  Method::atmgeom_checkedCalc(ws);
  Method::atmfields_checkedCalc(ws);
  Method::cloudbox_checkedCalc(ws);
  Method::sensor_checkedCalc(ws);
  Method::propmat_clearsky_agenda_checkedCalc(ws);
  Method::abs_xsec_agenda_checkedCalc(ws);
  Method::lbl_checkedCalc(ws);

  std::cout << "OI\n";
  Method::yCalc(ws);
  std::cout << "AI\n";
}
};  // namespace Waspam::Forward

int main() { Waspam::Forward::compute(); }
