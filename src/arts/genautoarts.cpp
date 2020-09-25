#include <auto_md.h>
#include <global_data.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Group {
  std::string varname_group;
  std::string varname_desc;
  std::size_t artspos;
};

struct Method {
  struct Gin {
    std::vector<std::string> desc;
    std::vector<std::string> group;
    std::vector<std::string> name;
    std::vector<std::string> defs;
    std::vector<bool> hasdefs;
  };

  struct Gout {
    std::vector<std::string> desc;
    std::vector<std::string> group;
    std::vector<std::string> name;
  };

  struct In {
    std::vector<std::string> varname;
    std::vector<std::size_t> varpos;
  };

  struct Out {
    std::vector<std::string> varname;
    std::vector<std::size_t> varpos;
  };

  std::string desc;
  std::vector<std::string> authors;
  bool set_method;
  bool agenda_method;
  bool supergeneric;
  bool uses_templates;
  bool pass_workspace;
  bool pass_wsv_names;
  In in;
  Gin gin;
  Out out;
  Gout gout;
  std::vector<std::size_t> inoutvarpos;
};

struct AgendaData {
  std::size_t pos;
  std::string desc;
  std::vector<std::string> ins;
  std::vector<std::string> outs;
};

std::map<std::string, Group> groups() {
  std::map<std::string, std::size_t> group;
  for (auto& x : global_data::WsvGroupMap) group[x.first] = x.second;
  std::map<std::string, std::size_t> name;
  for (auto& x : Workspace::wsv_data) name[x.Name()] = x.Group();
  std::map<std::string, std::string> desc;
  for (auto& x : Workspace::wsv_data) desc[x.Name()] = x.Description();
  std::map<std::string, std::size_t> pos;
  for (auto& x : Workspace::WsvMap) pos[x.first] = x.second;

  std::map<std::string, Group> out;
  for (auto& x : name) {
    for (auto& y : group) {
      if (y.second == x.second) {
        out[x.first] = {y.first, desc[x.first], pos[x.first]};
        break;
      }
    }
  }
  return out;
}

std::pair<std::vector<std::string>, std::vector<bool>> fixed_defaults(
    const std::vector<std::string>& vargroups,
    const std::vector<std::string>& vardefaults) {
  std::vector<std::string> defaults(vargroups.size());
  std::vector<bool> hasdefaults(vargroups.size());
  for (size_t i = 0; i < vargroups.size(); i++) {
    if (vardefaults[i] == NODEF)
      hasdefaults[i] = false;
    else
      hasdefaults[i] = true;

    if (vargroups[i] == "String") {
      defaults[i] = std::string("\"") + vardefaults[i] + std::string("\"");
    } else if (vargroups[i] == "Numeric") {
      if ("NaN" == vardefaults[i] or "nan" == vardefaults[i]) {
        defaults[i] = "std::numeric_limits<Numeric>::quiet_NaN()";
      } else if ("Inf" == vardefaults[i] or "inf" == vardefaults[i]) {
        defaults[i] = "std::numeric_limits<Numeric>::infinity()";
      } else if ("-Inf" == vardefaults[i] or "-inf" == vardefaults[i]) {
        defaults[i] = "-std::numeric_limits<Numeric>::infinity()";
      } else {
        defaults[i] = vardefaults[i];
      }
    } else if (vardefaults[i] == "[]") {
      defaults[i] = "{}";
    } else {
      defaults[i] = vardefaults[i];
    }

    if (defaults[i] == "") defaults[i] = "{}";

    for (auto& x : defaults[i]) {
      if (x == '[')
        x = '{';
      else if (x == ']')
        x = '}';
    }
  }

  return {defaults, hasdefaults};
}

std::map<std::string, Method> methods() {
  std::map<std::string, std::size_t> vargroup;
  for (auto& x : global_data::WsvGroupMap) vargroup[x.first] = x.second;
  std::map<std::string, std::size_t> varpos;
  for (auto& x : Workspace::WsvMap) varpos[x.first] = x.second;

  std::map<std::string, std::vector<std::size_t>> gin_group;
  for (auto& x : global_data::md_data_raw)
    gin_group[x.Name()] = {x.GInType().cbegin(), x.GInType().cend()};
  std::map<std::string, std::vector<std::string>> gin_names;
  for (auto& x : global_data::md_data_raw)
    gin_names[x.Name()] = {x.GIn().cbegin(), x.GIn().cend()};
  std::map<std::string, std::vector<std::string>> gin_defaults;
  for (auto& x : global_data::md_data_raw)
    gin_defaults[x.Name()] = {x.GInDefault().cbegin(), x.GInDefault().cend()};
  std::map<std::string, std::vector<std::string>> gin_desc;
  for (auto& x : global_data::md_data_raw)
    gin_desc[x.Name()] = {x.GInDescription().cbegin(),
                          x.GInDescription().cend()};
  std::map<std::string, std::vector<std::size_t>> gout_group;
  for (auto& x : global_data::md_data_raw)
    gout_group[x.Name()] = {x.GOutType().cbegin(), x.GOutType().cend()};
  std::map<std::string, std::vector<std::string>> gout_names;
  for (auto& x : global_data::md_data_raw)
    gout_names[x.Name()] = {x.GOut().cbegin(), x.GOut().cend()};
  std::map<std::string, std::vector<std::string>> gout_desc;
  for (auto& x : global_data::md_data_raw)
    gout_desc[x.Name()] = {x.GOutDescription().cbegin(),
                           x.GOutDescription().cend()};
  std::map<std::string, std::vector<std::size_t>> in_wspace;
  for (auto& x : global_data::md_data_raw)
    in_wspace[x.Name()] = {x.In().cbegin(), x.In().cend()};
  std::map<std::string, std::vector<std::size_t>> out_wspace;
  for (auto& x : global_data::md_data_raw)
    out_wspace[x.Name()] = {x.Out().cbegin(), x.Out().cend()};
  std::map<std::string, std::string> desc;
  for (auto& x : global_data::md_data_raw) desc[x.Name()] = x.Description();
  std::map<std::string, std::vector<std::string>> authors;
  for (auto& x : global_data::md_data_raw)
    authors[x.Name()] = {x.Authors().cbegin(), x.Authors().cend()};

  std::map<std::string, bool> set_method;
  for (auto& x : global_data::md_data_raw) set_method[x.Name()] = x.SetMethod();
  std::map<std::string, bool> agenda_method;
  for (auto& x : global_data::md_data_raw)
    agenda_method[x.Name()] = x.AgendaMethod();
  std::map<std::string, bool> supergeneric;
  for (auto& x : global_data::md_data_raw)
    supergeneric[x.Name()] = x.Supergeneric();
  std::map<std::string, bool> uses_templates;
  for (auto& x : global_data::md_data_raw)
    uses_templates[x.Name()] = x.UsesTemplates();
  std::map<std::string, bool> pass_workspace;
  for (auto& x : global_data::md_data_raw)
    pass_workspace[x.Name()] = x.PassWorkspace();
  std::map<std::string, bool> pass_wsv_names;
  for (auto& x : global_data::md_data_raw)
    pass_wsv_names[x.Name()] = x.PassWsvNames();

  std::map<std::string, std::vector<std::size_t>> inoutvarpos;
  for (auto& x : global_data::md_data_raw)
    inoutvarpos[x.Name()] = {x.InOut().cbegin(), x.InOut().cend()};
  std::map<std::string, std::vector<std::size_t>> invarpos;
  for (auto& x : global_data::md_data_raw)
    invarpos[x.Name()] = {x.InOnly().cbegin(), x.InOnly().cend()};
  std::map<std::string, std::vector<std::size_t>> outvarpos;
  for (auto& x : global_data::md_data_raw)
    outvarpos[x.Name()] = {x.OutOnly().cbegin(), x.OutOnly().cend()};

  std::map<std::string, Method> retval;
  for (auto& x : desc) {
    Method m;
    m.desc = x.second;
    m.authors = authors[x.first];

    Method::Gin gin;
    gin.desc = gin_desc[x.first];
    for (auto g : gin_group[x.first]) {
      bool found = false;
      for (auto& y : vargroup) {
        if (y.second == g) {
          gin.group.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find group\n";
        std::terminate();
      }
    }
    gin.name = gin_names[x.first];
    auto fixgin = fixed_defaults(gin.group, gin_defaults[x.first]);
    gin.defs = fixgin.first;
    gin.hasdefs = fixgin.second;
    m.gin = gin;

    Method::Gout gout;
    gout.desc = gout_desc[x.first];
    for (auto g : gout_group[x.first]) {
      bool found = false;
      for (auto& y : vargroup) {
        if (y.second == g) {
          gout.group.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find group\n";
        std::terminate();
      }
    }
    gout.name = gout_names[x.first];
    m.gout = gout;

    Method::In in;
    for (auto v : in_wspace[x.first]) {
      bool found = false;
      for (auto& y : varpos) {
        if (v == y.second) {
          in.varname.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find variable\n";
        std::terminate();
      }
    }
    in.varpos = invarpos[x.first];
    m.in = in;

    Method::Out out;
    for (auto v : out_wspace[x.first]) {
      bool found = false;
      for (auto& y : varpos) {
        if (v == y.second) {
          out.varname.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find variable\n";
        std::terminate();
      }
    }
    out.varpos = outvarpos[x.first];
    m.out = out;

    m.set_method = set_method[x.first];
    m.agenda_method = agenda_method[x.first];
    m.supergeneric = supergeneric[x.first];
    m.uses_templates = uses_templates[x.first];
    m.pass_workspace = pass_workspace[x.first];
    m.pass_wsv_names = pass_wsv_names[x.first];
    m.inoutvarpos = inoutvarpos[x.first];
    retval[x.first] = m;
  }

  return retval;
}

std::map<std::string, AgendaData> agendas() {
  auto g = groups();

  std::map<std::string, AgendaData> out;
  for (auto& x : global_data::agenda_data) {
    out[x.Name()].pos = global_data::AgendaMap.at(x.Name());
    out[x.Name()].desc = x.Description();

    for (std::size_t i : x.In()) {
      bool found = false;
      for (auto& y : g) {
        if (y.second.artspos == i) {
          out[x.Name()].ins.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find the variable\n";
        std::terminate();
      }
    }

    for (std::size_t i : x.Out()) {
      bool found = false;
      for (auto& y : g) {
        if (y.second.artspos == i) {
          out[x.Name()].outs.push_back(y.first);
          found = true;
          break;
        }
      }
      if (not found) {
        std::cerr << "Cannot find the variable\n";
        std::terminate();
      }
    }
  }
  return out;
}

struct NameMaps {
  std::map<std::string, AgendaData> agendaname_agenda;
  std::map<std::string, Method> methodname_method;
  std::map<std::string, Group> varname_group;
  std::map<std::string, std::size_t> group;

  NameMaps() {
    for (auto& x : global_data::WsvGroupMap) group[x.first] = x.second;
    varname_group = groups();
    methodname_method = methods();
    agendaname_agenda = agendas();
  }
};

int main() {
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

  const auto artsname = NameMaps();

  std::cout << "#ifndef autoarts_h\n"
            << "#define autoarts_h\n"
            << '\n'
            << "#include <auto_md.h>" << '\n'
            << "#include <auto_workspace.h>" << '\n'
            << "#include <arts.h>" << '\n'
            << "#include <global_data.h>" << '\n'
            << "#include <m_basic_types.h>" << '\n'
            << "#include <m_general.h>" << '\n'
            << "#include <m_append.h>" << '\n'
            << "#include <m_conversion.h>" << '\n'
            << "#include <m_copy.h>" << '\n'
            << "#include <m_gridded_fields.h>" << '\n'
            << "#include <m_xml.h>" << '\n'
            << "#include <m_select.h>" << '\n'
            << "#include <m_reduce.h>" << '\n'
            << "#include <m_nc.h>" << '\n'
            << "#include <m_delete.h>" << '\n'
            << "#include <m_extract.h>" << '\n'
            << "#include <m_ignore.h>" << '\n'
            << '\n'
            << '\n';

  std::cout << "namespace ARTS::Var {\n";
  for (auto& x : artsname.varname_group) {
    std::cout << "/*! " << x.second.varname_desc << '\n';
    std::cout << "@param[in,out] Workspace ws - An ARTS workspace\n*/\n";
    std::cout << "[[nodiscard]] ";
    std::cout << x.second.varname_group << '&' << ' ' << x.first
              << "(Workspace& ws) "
                 "noexcept { "
                 "return *static_cast<"
              << x.second.varname_group << " *>(ws[" << x.second.artspos
              << "]); "
                 "}\n\n";
  }
  for (auto& x : artsname.group) {
    std::cout << "/*! Creates in, and returns from, Workspace a/an " << x.first
              << '\n'
              << '\n';
    std::cout << "@param[in,out] Workspace ws - An ARTS workspace\n";
    std::cout << "@param[in] String name - The name the variable will have in "
                 "the workspace\n";
    std::cout << "@param[in] String desc - The description the variable will "
                 "have in the workspace (default: \"nodescription\")\n";
    std::cout << "*/\n";
    std::cout << "[[nodiscard]] ";
    std::cout << x.first << '&' << ' ' << x.first
              << "Create(Workspace& ws, const String& name, const String& "
                 "desc=\"nodescription\") {\n";
    std::cout << "return *static_cast<" << x.first
              << "*>(ws[ws.add_wsv({name.c_str(), desc.c_str(), " << x.second
              << "})]);\n"
              << "}\n\n";
  }
  std::cout << "}  // ARTS::Var \n\n";

  std::cout << "namespace ARTS::Method {\n";

  for (auto& x : artsname.methodname_method) {
    if (x.second.agenda_method) std::cerr << "Agenda Method; ";
    if (x.second.set_method) std::cerr << "Set Method; ";
    if (x.second.pass_workspace) std::cerr << "Pass Workspace Method; ";
    if (x.second.pass_wsv_names) std::cerr << "Pass Names Method; ";
    std::cerr << x.first << '\n';

    // Skip methods using verbosity and Agenda methods (for now)
    if (x.second.agenda_method) continue;

    // Also skip create methods since these are created in the groups above
    if (std::any_of(artsname.group.cbegin(), artsname.group.cend(),
                    [metname = x.first](auto& y) {
                      return (y.first + String("Create")) == metname;
                    }))
      continue;

    // Describe the method
    std::cout << "/*! " << x.second.desc << '\n';
    for (auto a : x.second.authors) std::cout << "@author " << a << '\n';
    std::cout << "\n"
                 "@param[in,out] Workspace ws - An ARTS workspace\n";
    for (std::size_t i = 0; i < x.second.gout.name.size(); i++)
      std::cout << "@param[out] " << x.second.gout.name[i] << " - "
                << x.second.gout.desc[i] << "\n";
    for (std::size_t i = 0; i < x.second.gin.name.size(); i++) {
      std::cout << "@param[in] " << x.second.gin.name[i] << " - "
                << x.second.gin.desc[i];
      if (x.second.gin.hasdefs[i])
        std::cout << " (default: " << x.second.gin.defs[i] << ")";
      std::cout << '\n';
    }
    std::cout << "\nUse the ARTS documentation to read more on how the "
                 "workspace is manipulated\n";
    std::cout << "This interface function has been automatically generated\n";
    std::cout << "*/" << '\n';

    // Count how many anys we need
    std::size_t anys = 0;
    for (auto& name : x.second.gout.group)
      if (name == "Any") anys++;
    for (auto& name : x.second.gin.group)
      if (name == "Any") anys++;

    // Make it a template if we need any anys
    if (anys) {
      std::cout << "template <";
      for (std::size_t i = 0; i < anys; i++) {
        std::cout << "typename Any_" << i;
        if (i not_eq anys - 1) std::cout << ", ";
      }
      std::cout << ">\n";
    }

    // Now we keep track of our current any count
    std::size_t iany = 0;

    // Make the function
    std::cout << "void " << x.first << "(Workspace& ws";

    // First put all GOUT variables
    if (x.second.pass_wsv_names) {
      for (std::size_t i = 0; i < x.second.gout.group.size(); i++) {
        if (x.second.gout.group[i] == "Any") {
          std::cout << ',' << '\n'
                    << "std::pair<Any_" << iany << ", String>" << '&' << ' '
                    << x.second.gout.name[i];
          iany++;
        } else {
          std::cout << ',' << '\n'
                    << "std::pair<" << x.second.gout.group[i] << ", String>"
                    << '&' << ' ' << x.second.gout.name[i];
        }
      }
    } else {
      for (std::size_t i = 0; i < x.second.gout.group.size(); i++) {
        if (x.second.gout.group[i] == "Any") {
          std::cout << ',' << '\n'
                    << "Any_" << iany << '&' << ' ' << x.second.gout.name[i];
          iany++;
        } else {
          std::cout << ',' << '\n'
                    << x.second.gout.group[i] << '&' << ' '
                    << x.second.gout.name[i];
        }
      }
    }

    // Second put all GIN variables that have no default argument
    if (x.second.pass_wsv_names) {
      for (std::size_t i = 0; i < x.second.gin.group.size(); i++) {
        if (not x.second.gin.hasdefs[i]) {
          if (x.second.gin.group[i] == "Any") {
            std::cout << ',' << '\n'
                      << "const std::pair<Any_" << iany << ", String>" << '&'
                      << ' ' << x.second.gin.name[i];
            iany++;
          } else {
            std::cout << ',' << "\nconst std::pair<" << x.second.gin.group[i]
                      << ", String>" << '&' << ' ' << x.second.gin.name[i];
          }
        }
      }
    } else {
      for (std::size_t i = 0; i < x.second.gin.group.size(); i++) {
        if (not x.second.gin.hasdefs[i]) {
          if (x.second.gin.group[i] == "Any") {
            std::cout << ',' << '\n'
                      << "const Any_" << iany << '&' << ' '
                      << x.second.gin.name[i];
            iany++;
          } else {
            std::cout << ',' << "\nconst " << x.second.gin.group[i] << '&'
                      << ' ' << x.second.gin.name[i];
          }
        }
      }
    }

    // Lastly put all GIN variables that have a default argument
    if (x.second.pass_wsv_names) {
      for (std::size_t i = 0; i < x.second.gin.group.size(); i++) {
        if (x.second.gin.hasdefs[i]) {
          if (x.second.gin.group[i] == "Any") {
            std::cout << ',' << "\nconst std::pair<Any_" << iany << ", String>"
                      << '&' << ' ' << x.second.gin.name[i] << '=' << '{'
                      << x.second.gin.defs[i] << ", \"" << x.second.gin.name[i]
                      << "\"}";
            iany++;
          } else {
            std::cout << ',' << "\nconst std::pair<" << x.second.gin.group[i]
                      << ", String>" << '&' << ' ' << x.second.gin.name[i]
                      << '=' << '{' << x.second.gin.defs[i] << ", \""
                      << x.second.gin.name[i] << "\"}";
          }
        }
      }
    } else {
      for (std::size_t i = 0; i < x.second.gin.group.size(); i++) {
        if (x.second.gin.hasdefs[i]) {
          if (x.second.gin.group[i] == "Any") {
            std::cout << ',' << "\nconst Any_" << iany << '&' << ' '
                      << x.second.gin.name[i] << '=' << x.second.gin.defs[i];
            iany++;
          } else {
            std::cout << ',' << "\nconst " << x.second.gin.group[i] << '&'
                      << ' ' << x.second.gin.name[i] << '='
                      << x.second.gin.defs[i];
          }
        }
      }
    }

    // End of function definition and open function block
    std::cout << ')' << ' ' << '{' << '\n';

    // Call the ARTS auto_md.h function
    std::cout << x.first << '(';

    // We need the workspace if we input an Agenda or simply pass the workspace
    bool has_any = false;
    if (x.second.pass_workspace or x.second.agenda_method or
        std::any_of(
            x.second.gin.group.cbegin(), x.second.gin.group.cend(),
            [](auto& g) { return g == "Agenda" or g == "ArrayOfAgenda"; }) or
        std::any_of(x.second.in.varname.cbegin(), x.second.in.varname.cend(),
                    [&](auto& g) {
                      return artsname.varname_group.at(g).varname_group ==
                                 "Agenda" or
                             artsname.varname_group.at(g).varname_group ==
                                 "ArrayOfAgenda";
                    })) {
      std::cout << "ws";
      has_any = true;
    }

    // First are all the outputs
    for (std::size_t i = 0; i < x.second.out.varname.size(); i++) {
      if (has_any) std::cout << ',';
      has_any = true;
      std::cout << "Var::" << x.second.out.varname[i] << "(ws)";
    }

    // Second comes all the generic outputs
    for (std::size_t i = 0; i < x.second.gout.name.size(); i++) {
      if (has_any) std::cout << ",";
      has_any = true;
      std::cout << x.second.gout.name[i];
      if (x.second.pass_wsv_names) std::cout << ".first";
    }

    // And their filenames if relevant
    if (x.second.pass_wsv_names) {
      for (std::size_t i = 0; i < x.second.gout.name.size(); i++) {
        if (has_any) std::cout << ",";
        has_any = true;
        std::cout << x.second.gout.name[i] << ".second";
      }
    }

    // Then come all the inputs that are not also outputs
    for (std::size_t i = 0; i < x.second.in.varname.size(); i++) {
      if (std::any_of(x.second.out.varname.cbegin(),
                      x.second.out.varname.cend(),
                      [in = x.second.in.varname[i]](const auto& out) {
                        return in == out;
                      }))
        continue;
      if (has_any) std::cout << ",";
      has_any = true;
      std::cout << "Var::" << x.second.in.varname[i] << "(ws)";
    }

    // Lastly are all the generic inputs, which cannot also be outputs
    for (std::size_t i = 0; i < x.second.gin.name.size(); i++) {
      if (has_any) std::cout << ",";
      has_any = true;
      std::cout << x.second.gin.name[i];
      if (x.second.pass_wsv_names) std::cout << ".first";
    }

    // And their filenames if relevant
    if (x.second.pass_wsv_names) {
      for (std::size_t i = 0; i < x.second.gin.name.size(); i++) {
        if (has_any) std::cout << ",";
        has_any = true;
        std::cout << x.second.gin.name[i] << ".second";
      }
    }

    // Check verbosity
    const bool has_verbosity =
        std::any_of(x.second.in.varname.cbegin(), x.second.in.varname.cend(),
                    [](auto& name) { return name == "verbosity"; });

    // Add verbosity of it does not exist
    if (not has_verbosity) {
      if (has_any) std::cout << ",";
      has_any = true;
      std::cout << "Var::verbosity(ws)";
    }

    // Close the function call and the function itself
    std::cout << ')' << ';' << '\n' << '}' << '\n' << '\n' << '\n';
  }

  std::cout << "}  // ARTS::Method \n\n";

  std::cout << "namespace ARTS::AgendaExecute { \n\n";
  for (auto& x : artsname.agendaname_agenda) {
    std::cout << "/*! " << x.second.desc << '\n'
              << "@param[in,out] Workspace ws - An ARTS workspace\n"
              << "*/\n"
              << "void " << x.first << "(Workspace& ws) {\n"
              << x.first << "Execute(ws";
    for (auto& name : x.second.outs) {
      std::cout << ',' << ' ' << "Var::" << name << "(ws)";
    }
    for (auto& name : x.second.ins) {
      if (not std::any_of(x.second.outs.cbegin(), x.second.outs.cend(),
                          [name](auto& outname) { return name == outname; }))
        std::cout << ',' << ' ' << "Var::" << name << "(ws)";
    }
    std::cout << ", Var::" << x.first << "(ws));}\n\n";
  }
  std::cout << "}  // ARTS::AgendaExecute \n\n";

  std::cout << "namespace ARTS::AgendaDefine { \n\n";
  std::cout << "}  // ARTS::AgendaDefine \n\n";

  std::cout << "#endif  // autoarts_h\n\n";
}
