#include "interface.h"

#include <limits>
#include <memory>

#include "autoarts.h"

namespace ARTS {
void init() {
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

  init();

  Workspace ws;
  ws.initialize();
  Var::verbosity(ws).set_screen_verbosity(3);
  Var::verbosity(ws).set_agenda_verbosity(3);
  Var::verbosity(ws).set_file_verbosity(3);
  Var::verbosity(ws).set_main_agenda(1);
  Var::aa_grid(ws) = Vector(5, 1);
  std::cout << Var::aa_grid(ws) << '\n';
  Method::nelemGet(ws, Var::aa_grid(ws));
  std::cout << Var::nelem(ws) << '\n';
  Method::Print(ws, Var::aa_grid(ws), 0);
  std::cout << "HEJ\n";
  Var::output_file_format(ws) = "ascii";
  Method::WriteXML(ws, std::make_pair(Var::aa_grid(ws), String("aa_grid")),
                   std::make_pair(String("TEST.xml"), String("TEST.xml")));

  auto numout = AgendaVar::g0(ws);
  auto numin = AgendaVar::lat(ws);
  auto numign = AgendaVar::lon(ws);
  Method::NumericSet(ws, Var::lat(ws), 1336.0);
  Method::NumericSet(ws, Var::lon(ws), 3336.0);
  std::cout << Var::lat(ws) << "\n";
  AgendaDefine::g0_agenda(ws, AgendaMethod::NumericSet(ws, numout, numin), AgendaMethod::NumericAdd(ws, numout, numout, numign));
  Var::g0_agenda(ws).check(ws, Var::verbosity(ws));
  AgendaExecute::g0_agenda(ws);
  std::cout << Var::g0(ws) << '\n';

  std::cout << "\n\n\n\n\n\n\n\n\n";

  return 1;
}

}  // namespace ARTS
