#include <iostream>

#include "arts/interface.h"

int main() try {
  using namespace ARTS;
  
  auto ws = init(0, 0, 0);

  ARTS::Agenda::iy_main_agenda_emission(ws);
  
  ARTS::Agenda::iy_space_agenda_cosmic_background(ws);
  
  ARTS::Agenda::iy_surface_agenda_use_surface_property(ws);
  
  ARTS::Agenda::ppath_agenda_follow_sensor_los(ws);
  
  ARTS::Agenda::ppath_step_agenda_geometric_path(ws);
  
  ARTS::Agenda::propmat_clearsky_agenda_on_the_fly(ws);
  
  ARTS::Agenda::abs_xsec_agenda_standard_with_cia(ws);
  
  ARTS::Agenda::surface_rtprop_agenda_blackbody_from_surface(ws);
  
  ARTS::Agenda::geo_pos_agenda_empty(ws);
  
  ARTS::Agenda::water_p_eq_agenda_default(ws);
  
  Method::jacobianOff(ws);
  Method::nlteOff(ws);
  Var::iy_unit(ws) = "PlanckBT";
  Method::Touch(ws, Var::iy_aux_vars(ws));
  Method::Touch(ws, Var::abs_cont_names(ws));
  Method::Touch(ws, Var::abs_cont_parameters(ws));
  Method::Touch(ws, Var::abs_cont_models(ws));
  Method::Touch(ws, Var::surface_props_names(ws));
  
  Method::abs_speciesSet(ws, ArrayOfString{"PH3", "SO2", "CO2", "CO", "HCl", "HF", "N2", "O", "O2", "OCS", "SO", "H2O",
                                           "N2-CIA-N2-0", "CO2-CIA-CO2-0"});
  Var::nelem(ws) = 501;
  Method::VectorNLogSpace(ws, Var::p_grid(ws), 9.2100000e+06, 9.4409618e-14);
    
  Method::AtmosphereSet1D(ws);
  Var::lat_true(ws) = Var::lat_grid(ws);
  Var::lon_true(ws) = Var::lon_grid(ws);
  Method::Touch(ws, Var::wind_u_field(ws));
  Method::Touch(ws, Var::wind_v_field(ws));
  Method::Touch(ws, Var::wind_w_field(ws));
  Method::Touch(ws, Var::mag_u_field(ws));
  Method::Touch(ws, Var::mag_v_field(ws));
  Method::Touch(ws, Var::mag_w_field(ws));
  Method::Touch(ws, Var::nlte_field(ws));
  Method::Touch(ws, Var::rte_alonglos_v(ws));
  Method::Touch(ws, Var::surface_props_data(ws));
  Method::AtmRawRead(ws, String{"/home/larsson/Work/arts-xml-data/planets/Venus/MPS/Venus.vira.day/Venus.vira.day"});
  Method::ReadXML(ws, Var::abs_cia_data(ws), String{"/home/larsson/Work/arts-xml-data/spectroscopy/cia/hitran2011/hitran_cia2012_adapted.xml.gz"});
  
  Method::AtmFieldsCalc(ws, 1, 1);
  
  Method::refellipsoidVenus(ws, String{"Sphere"});
  Method::z_surfaceConstantAltitude(ws);
  Var::t_surface(ws) = Matrix(1, 1, 735.29999);
  
  Method::ReadXML(ws, Var::partition_functions(ws), String{"/home/larsson/Work/arts-xml-data/spectroscopy/PartitionSums/TIPS/tips.xml"});
  Method::ReadXML(ws, Var::isotopologue_ratios(ws), String{"/home/larsson/Work/arts-xml-data/planets/Venus/isotopratio_Venus.xml"});
  Method::ReadARTSCAT(ws, String{"/home/larsson/Work/arts-xml-data/spectroscopy/Perrin/PH3.xml.gz"});
  Method::abs_linesDeleteBadF0(ws, 400e9);
  Method::abs_linesDeleteBadF0(ws, 1300e9, 0);
  const auto phoscat = Var::abs_lines(ws);
  Method::ReadSplitARTSCAT(ws, String{"/home/larsson/Work/arts-xml-data/spectroscopy/Perrin/"}, 400e9, 1300e9);
  Method::cloudboxOff(ws);
  Method::abs_linesDeleteBadF0(ws, 400e9);
  Method::abs_linesDeleteBadF0(ws, 1300e9, 0);
  Method::abs_lines_per_speciesCreateFromLines(ws);
  
  Var::abs_f_interp_order(ws) = 1;
  Var::stokes_dim(ws) = 1;
  Var::ppath_lraytrace(ws) = 1e3;
  Var::ppath_lmax(ws) = 1e3;
  
  for (auto& line: phoscat.value()[0].AllLines()) {
    if (line.F0() < 500e9 or line.F0() > 1200e9 or (line.F0() < 1000e9 and line.F0() > 650e9)) continue;
    
    Var::nelem(ws) = 1001;
    Method::VectorNLinSpace(ws, Var::f_grid(ws).value(), line.F0() - 500e9, line.F0() + 500e9);
    
    const Numeric za = 119.8;
    
    Var::sensor_pos(ws) = Matrix(1, 1, 1000000);
    Var::sensor_los(ws) = Matrix(1, 1, za);
    Method::Touch(ws, Var::transmitter_pos(ws));
    Method::sensorOff(ws);
    
    Method::atmgeom_checkedCalc(ws);
    Method::atmfields_checkedCalc(ws);
    Method::cloudbox_checkedCalc(ws);
    Method::sensor_checkedCalc(ws);
    Method::propmat_clearsky_agenda_checkedCalc(ws);
    Method::abs_xsec_agenda_checkedCalc(ws);
    Method::lbl_checkedCalc(ws);
    
    std::cout << "STARTING YCALC\n";
    Method::yCalc(ws);
    
    std::cout << Var::y(ws).value() << '\n';
  }
    
  std::cout << "Hello!\n";
} catch (const std::exception& e) {
  std::cerr << "EXITING WITH ERROR:\n" << e.what() << '\n';
  return 1;
}
