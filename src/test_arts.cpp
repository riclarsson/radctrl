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
  
  Method::jacobianOff(ws);
  Method::nlteOff(ws);
  Var::iy_unit(ws) = "PlanckBT";
  Method::Touch(ws, Var::iy_aux_vars(ws));
  Method::Touch(ws, Var::abs_cont_names(ws));
  Method::Touch(ws, Var::abs_cont_parameters(ws));
  Method::Touch(ws, Var::abs_cont_models(ws));
  Method::Touch(ws, Var::surface_props_names(ws));
  
  Method::abs_speciesSet(ws, {"PH3", "SO2", "CO2", "CO", "HCl", "HF", "N2", "O", "O2", "OCS", "SO", "H2O",
                              "N2-CIA-N2-0", "CO2-CIA-CO2-0"});
  
  Var::p_grid(ws) = {9.2100000e+06, 8.6342200e+06, 8.0944300e+06, 7.5883900e+06,
    7.1139800e+06, 6.6692300e+06, 6.2301200e+06, 5.8199200e+06,
    5.4367200e+06, 5.0787600e+06, 4.7443600e+06, 4.4142000e+06,
    4.1070100e+06, 3.8212000e+06, 3.5552800e+06, 3.3078600e+06,
    3.0634100e+06, 2.8370300e+06, 2.6273800e+06, 2.4332200e+06,
    2.2534100e+06, 2.0756500e+06, 1.9119200e+06, 1.7611100e+06,
    1.6221900e+06, 1.4942300e+06, 1.3677300e+06, 1.2519400e+06,
    1.1459600e+06, 1.0489500e+06, 9.6014500e+05, 8.7254800e+05,
    7.9294400e+05, 7.2060100e+05, 6.5485900e+05, 5.9511400e+05,
    5.3641600e+05, 4.8350800e+05, 4.3581700e+05, 3.9283100e+05,
    3.5408500e+05, 3.1607200e+05, 2.8214100e+05, 2.5185200e+05,
    2.2481500e+05, 2.0068000e+05, 1.7701500e+05, 1.5614100e+05,
    1.3772800e+05, 1.2148700e+05, 1.0716100e+05, 9.3220203e+04,
    8.1093203e+04, 7.0543703e+04, 6.1366699e+04, 5.3383500e+04,
    4.5727898e+04, 3.9170199e+04, 3.3552898e+04, 2.8741100e+04,
    2.4619400e+04, 2.0738500e+04, 1.7469301e+04, 1.4715400e+04,
    1.2395700e+04, 1.0441600e+04, 8.6485898e+03, 7.1634302e+03,
    5.9333101e+03, 4.9144302e+03, 4.0705200e+03, 3.3194199e+03,
    2.7069099e+03, 2.2074299e+03, 1.8001100e+03, 1.4679500e+03,
    1.1791899e+03, 9.4723199e+02, 7.6090302e+02, 6.1122699e+02,
    4.9099301e+02, 3.8852499e+02, 3.0744101e+02, 2.4328000e+02,
    1.9250800e+02, 1.5233299e+02, 1.1900700e+02, 9.2971298e+01,
    7.2631798e+01, 5.6742100e+01, 4.4328499e+01, 3.4338402e+01,
    2.6599600e+01, 2.0605000e+01, 1.5961300e+01, 1.2364100e+01,
    9.5484505e+00, 7.3739800e+00, 5.6947098e+00, 4.3978500e+00,
    3.3963301e+00, 2.6283000e+00, 2.0339501e+00, 1.5740100e+00,
    1.2180700e+00, 9.4262201e-01, 7.3313898e-01, 5.7021099e-01,
    4.4349101e-01, 3.4493199e-01, 2.6827699e-01, 2.1005400e-01,
    1.6446701e-01, 1.2877300e-01, 1.0082600e-01, 7.8944497e-02,
    6.2282700e-02, 4.9137499e-02, 3.8766701e-02, 3.0584799e-02,
    2.4129599e-02, 1.9226700e-02, 1.5320000e-02, 1.2207100e-02,
    9.7267600e-03, 7.7503799e-03, 6.2531000e-03, 5.0450899e-03,
    4.0704398e-03, 3.2840900e-03, 2.6496400e-03, 2.1718300e-03,
    1.7801800e-03, 1.4591600e-03, 1.1960299e-03, 9.8034402e-04,
    8.1890897e-04, 6.8405800e-04, 5.7141302e-04, 4.7731699e-04,
    3.9871701e-04, 3.3993201e-04, 2.8981399e-04, 2.4708500e-04,
    2.1065600e-04, 1.7959801e-04, 1.5614901e-04, 1.3576100e-04,
    1.1803500e-04, 1.0262300e-04, 8.9223897e-05, 7.8927202e-05,
    6.9818700e-05, 6.1761399e-05, 5.4634002e-05, 4.8328999e-05,
    4.3368800e-05, 3.8917598e-05, 3.4923301e-05, 3.1339001e-05,
    2.8122500e-05, 2.5527401e-05, 2.3171700e-05, 2.1033500e-05,
    1.9092600e-05, 1.7330700e-05, 1.5876900e-05, 1.4545000e-05,
    1.3324800e-05, 1.2207000e-05, 1.1183000e-05, 1.0316300e-05,
    9.5168498e-06, 8.7793296e-06, 8.0989603e-06, 7.4713198e-06,
    6.9307898e-06, 6.4293699e-06, 5.9642198e-06, 5.5327200e-06,
    5.1324400e-06, 4.7821300e-06, 4.4557300e-06, 4.1516100e-06,
    3.8682401e-06, 3.6042200e-06, 3.3693400e-06, 3.1497700e-06,
    2.9445100e-06, 2.7526200e-06, 2.5732400e-06, 2.4114900e-06,
    2.2598999e-06, 2.1178400e-06, 1.9847200e-06, 1.8599600e-06,
    1.7465800e-06, 1.6401100e-06, 1.5401300e-06, 1.4462400e-06,
    1.3580800e-06, 1.2773600e-06, 1.2014300e-06, 1.1300200e-06,
    1.0628499e-06, 9.9967201e-07, 9.4140199e-07, 8.8652899e-07,
    8.3485497e-07, 7.8619200e-07, 7.4036598e-07, 6.9792497e-07,
    6.5791602e-07, 6.2020098e-07, 5.8464798e-07, 5.5113298e-07,
    5.1997102e-07, 4.9057201e-07, 4.6283401e-07, 4.3666401e-07,
    4.1197501e-07, 3.8899901e-07, 3.6730401e-07, 3.4682000e-07,
    3.2747701e-07, 3.0921399e-07, 2.9215300e-07, 2.7603400e-07,
    2.6080400e-07, 2.4641500e-07, 2.3281900e-07, 2.2011100e-07,
    2.0809701e-07, 1.9673800e-07, 1.8599999e-07, 1.7584701e-07,
    1.6635200e-07, 1.5737000e-07, 1.4887300e-07, 1.4083400e-07,
    1.3323000e-07, 1.2609200e-07, 1.1933700e-07, 1.1294300e-07,
    1.0689200e-07, 1.0116600e-07, 9.5804602e-08, 9.0727603e-08,
    8.5919503e-08, 8.1366302e-08, 7.7054402e-08, 7.2971005e-08,
    6.9104003e-08, 6.5441927e-08, 6.1973918e-08, 5.8689692e-08,
    5.5579509e-08, 5.2634146e-08, 4.9844869e-08, 4.7203405e-08,
    4.4701923e-08, 4.2333003e-08, 4.0089621e-08, 3.7965125e-08,
    3.5953213e-08, 3.4047920e-08, 3.2243595e-08, 3.0534888e-08,
    2.8916732e-08, 2.7384328e-08, 2.5933131e-08, 2.4558839e-08,
    2.3257376e-08, 2.2024882e-08, 2.0857702e-08, 1.9752375e-08,
    1.8705624e-08, 1.7714344e-08, 1.6775596e-08, 1.5886595e-08,
    1.5044706e-08, 1.4247432e-08, 1.3492408e-08, 1.2777395e-08,
    1.2100274e-08, 1.1459036e-08, 1.0851779e-08, 1.0276704e-08,
    9.7321033e-09, 9.2163633e-09, 8.7279542e-09, 8.2654278e-09,
    7.8274123e-09, 7.4126089e-09, 7.0197874e-09, 6.6477830e-09,
    6.2954925e-09, 5.9618712e-09, 5.6459296e-09, 5.3467310e-09,
    5.0633880e-09, 4.7950604e-09, 4.5409525e-09, 4.3003107e-09,
    4.0724214e-09, 3.8566088e-09, 3.6522329e-09, 3.4586876e-09,
    3.2753990e-09, 3.1018235e-09, 2.9374465e-09, 2.7817804e-09,
    2.6343636e-09, 2.4947589e-09, 2.3625525e-09, 2.2373521e-09,
    2.1187866e-09, 2.0065043e-09, 1.9001722e-09, 1.7994751e-09,
    1.7041143e-09, 1.6138070e-09, 1.5282854e-09, 1.4472959e-09,
    1.3705984e-09, 1.2979653e-09, 1.2291814e-09, 1.1640425e-09,
    1.1023556e-09, 1.0439377e-09, 9.8861561e-10, 9.3622523e-10,
    8.8661120e-10, 8.3962641e-10, 7.9513151e-10, 7.5299456e-10,
    7.1309061e-10, 6.7530131e-10, 6.3951460e-10, 6.0562437e-10,
    5.7353010e-10, 5.4313663e-10, 5.1435382e-10, 4.8709631e-10,
    4.6128328e-10, 4.3683818e-10, 4.1368852e-10, 3.9176564e-10,
    3.7100453e-10, 3.5134364e-10, 3.3272464e-10, 3.1509234e-10,
    2.9839443e-10, 2.8258141e-10, 2.6760638e-10, 2.5342493e-10,
    2.3999501e-10, 2.2727679e-10, 2.1523256e-10, 2.0382659e-10,
    1.9302507e-10, 1.8279596e-10, 1.7310893e-10, 1.6393525e-10,
    1.5524772e-10, 1.4702057e-10, 1.3922941e-10, 1.3185113e-10,
    1.2486385e-10, 1.1824685e-10, 1.1198052e-10, 1.0604626e-10,
    1.0042648e-10, 9.5104506e-11, 9.0064568e-11, 8.5291715e-11,
    8.0771793e-11, 7.6491398e-11, 7.2437837e-11, 6.8599089e-11,
    6.4963770e-11, 6.1521101e-11, 5.8260871e-11, 5.5173413e-11,
    5.2249570e-11, 4.9480673e-11, 4.6858510e-11, 4.4375305e-11,
    4.2023694e-11, 3.9796703e-11, 3.7687729e-11, 3.5690518e-11,
    3.3799146e-11, 3.2008004e-11, 3.0311782e-11, 2.8705449e-11,
    2.7184242e-11, 2.5743649e-11, 2.4379398e-11, 2.3087444e-11,
    2.1863955e-11, 2.0705303e-11, 1.9608053e-11, 1.8568950e-11,
    1.7584913e-11, 1.6653024e-11, 1.5770519e-11, 1.4934781e-11,
    1.4143332e-11, 1.3393824e-11, 1.2684036e-11, 1.2011862e-11,
    1.1375310e-11, 1.0772490e-11, 1.0201616e-11, 9.6609949e-12,
    9.1490232e-12, 8.6641827e-12, 8.2050357e-12, 7.7702207e-12,
    7.3584480e-12, 6.9684968e-12, 6.5992104e-12, 6.2494940e-12,
    5.9183103e-12, 5.6046772e-12, 5.3076647e-12, 5.0263920e-12,
    4.7600249e-12, 4.5077737e-12, 4.2688901e-12, 4.0426659e-12,
    3.8284302e-12, 3.6255475e-12, 3.4334164e-12, 3.2514670e-12,
    3.0791598e-12, 2.9159838e-12, 2.7614551e-12, 2.6151154e-12,
    2.4765308e-12, 2.3452903e-12, 2.2210047e-12, 2.1033055e-12,
    1.9918436e-12, 1.8862885e-12, 1.7863271e-12, 1.6916630e-12,
    1.6020156e-12, 1.5171189e-12, 1.4367211e-12, 1.3605840e-12,
    1.2884816e-12, 1.2202002e-12, 1.1555373e-12, 1.0943012e-12,
    1.0363101e-12, 9.8139221e-13, 9.2938462e-13, 8.8013311e-13,
    8.3349161e-13, 7.8932182e-13, 7.4749275e-13, 7.0788035e-13,
    6.7036716e-13, 6.3484194e-13, 6.0119932e-13, 5.6933956e-13,
    5.3916816e-13, 5.1059565e-13, 4.8353730e-13, 4.5791288e-13,
    4.3364639e-13, 4.1066587e-13, 3.8890317e-13, 3.6829376e-13,
    3.4877652e-13, 3.3029356e-13, 3.1279009e-13, 2.9621419e-13,
    2.8051671e-13, 2.6565110e-13, 2.5157327e-13, 2.3824147e-13,
    2.2561618e-13, 2.1365994e-13, 2.0233732e-13, 1.9161472e-13,
    1.8146035e-13, 1.7184409e-13, 1.6273744e-13, 1.5411339e-13,
    1.4594635e-13, 1.3821212e-13, 1.3088775e-13, 1.2395152e-13,
    1.1738287e-13, 1.1116232e-13, 1.0527142e-13, 9.9692701e-14,
    9.4409618e-14};
    
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
  Method::AtmRawRead(ws, "/home/larsson/Work/arts-xml-data/planets/Venus/MPS/Venus.vira.day/Venus.vira.day");
  std::pair x1{Var::abs_cia_data(ws), String{"abs_cia_data"}};
  Method::ReadXML(ws, x1, {String{"/home/larsson/Work/arts-xml-data/spectroscopy/cia/hitran2011/hitran_cia2012_adapted.xml.gz"}, String{"/home/larsson/Work/arts-xml-data/spectroscopy/cia/hitran2011/hitran_cia2012_adapted.xml.gz"}});
  Method::AtmFieldsCalc(ws, 1, 1);
  
  Method::refellipsoidVenus(ws, "Sphere");
  Method::z_surfaceConstantAltitude(ws, 0.0);
  Var::t_surface(ws) = Matrix(1, 1, 735.29999);
  
  std::pair x2 = {Var::partition_functions(ws), String{"partition_functions"}};
  std::pair x3 = {Var::isotopologue_ratios(ws), String{"isotopologue_ratios"}};
  Method::ReadXML(ws, x2, {String{"/home/larsson/Work/arts-xml-data/spectroscopy/PartitionSums/TIPS/tips.xml"}, String{"/home/larsson/Work/arts-xml-data/spectroscopy/PartitionSums/TIPS/tips.xml"}});
  Method::ReadXML(ws, x3, {String{"/home/larsson/Work/arts-xml-data/planets/Venus/isotopratio_Venus.xml"}, String{"/home/larsson/Work/arts-xml-data/planets/Venus/isotopratio_Venus.xml"}});
  Var::partition_functions(ws) = x2.first;  // FIXME: This is ugly
  Var::isotopologue_ratios(ws) = x3.first;  // FIXME: This is ugly
  Method::ReadARTSCAT(ws, "/home/larsson/Work/arts-xml-data/spectroscopy/Perrin/PH3.xml.gz");
  Method::abs_linesDeleteBadF0(ws, 400e9);
  Method::abs_linesDeleteBadF0(ws, 1300e9, 0);
  const auto phoscat = Var::abs_lines(ws);
  Method::ReadSplitARTSCAT(ws, "/home/larsson/Work/arts-xml-data/spectroscopy/Perrin/", 400e9, 1300e9);
  Method::cloudboxOff(ws);
  Method::abs_linesDeleteBadF0(ws, 400e9);
  Method::abs_linesDeleteBadF0(ws, 1300e9, 0);
  Method::abs_lines_per_speciesCreateFromLines(ws);
  
  Var::abs_f_interp_order(ws) = 1;
  Var::stokes_dim(ws) = 1;
  Var::ppath_lraytrace(ws) = 1e3;
  Var::ppath_lmax(ws) = 1e3;
  
  for (auto& line: phoscat[0].AllLines()) {
    if (line.F0() < 500e9 or line.F0() > 1200e9 or (line.F0() < 1000e9 and line.F0() > 650e9)) continue;
    
    Var::nelem(ws) = 1001;
    Method::VectorNLinSpace(ws, Var::f_grid(ws), line.F0() - 500e9, line.F0() + 500e9);
    
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
    
    std::cout << "got here\n";
    Method::yCalc(ws);
    
    std::cout << Var::y(ws)<< '\n';
  }
    
  std::cout << "Hello!\n";
} catch (const std::exception& e) {
  std::cerr << "EXITING WITH ERROR:\n" << e.what() << '\n';
  return 1;
}
