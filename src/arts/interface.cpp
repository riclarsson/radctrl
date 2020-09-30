#include "interface.h"

namespace ARTS {
Workspace init(std::size_t screen, std::size_t file, std::size_t agenda) {
  define_wsv_group_names();
  Workspace::define_wsv_data();
  Workspace::define_wsv_map();
  define_md_data_raw();
  expand_md_data_raw_to_md_data();
  define_md_map();
  define_agenda_data();
  define_agenda_map();
  define_species_data();
  define_species_map();

  Workspace ws;
  ws.initialize();
  Var::verbosity(ws).set_screen_verbosity(screen);
  Var::verbosity(ws).set_agenda_verbosity(agenda);
  Var::verbosity(ws).set_file_verbosity(file);
  Var::verbosity(ws).set_main_agenda(1);

#ifndef NDEBUG
  ws.context = "";
#endif

  return ws;
}

double test() {
  // Make global data visible:
  using global_data::agenda_data;
  using global_data::AgendaMap;
  using global_data::md_data_raw;
  using global_data::MdRawMap;
  using global_data::species_data;
  using global_data::SpeciesMap;
  using global_data::wsv_group_names;
  using global_data::WsvGroupMap;

  auto ws = init();
  Var::aa_grid(ws) = Vector(5, 1);
  std::cout << Var::aa_grid(ws) << '\n';
  Method::nelemGet(ws, Var::aa_grid(ws));
  std::cout << Var::nelem(ws) << '\n';
  Method::Print(ws, Var::aa_grid(ws), 0);
  std::cout << "HEJ\n";
  Var::output_file_format(ws) = "ascii";
  Method::WriteXML(ws, std::make_pair(Var::aa_grid(ws), String("aa_grid")),
                   std::make_pair(String("TEST.xml"), String("TEST.xml")));

  //   auto autovar = AgendaVar::lat(ws);
  Method::NumericSet(ws, Var::lat(ws), 1336.0);
  std::cout << Var::lat(ws) << "\n";
  AgendaDefine::g0_agenda(
      ws, AgendaMethod::NumericSet(ws, AgendaVar::g0(ws), AgendaVar::lat(ws)),
      AgendaMethod::Ignore(ws, AgendaVar::lon(ws)));
  AgendaExecute::g0_agenda(ws);
  std::cout << Var::g0(ws) << '\n';

  AgendaDefine::iy_main_agenda(ws, AgendaMethod::ppathCalc(ws),
                               AgendaMethod::iyEmissionStandard(ws));

  std::cout << "\n\n";

  return 1;
}

}  // namespace ARTS
