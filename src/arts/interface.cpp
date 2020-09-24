#include <memory>
#include <limits>

#include "interface.h"

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
    using global_data::md_data_raw;
    using global_data::MdRawMap;
    using global_data::agenda_data;
    using global_data::AgendaMap;
    using global_data::wsv_group_names;
    using global_data::WsvGroupMap;
    using global_data::species_data;
    using global_data::SpeciesMap;
    
    init();
    
    Workspace ws;
    ws.initialize();
    Var::aa_grid(ws) = Vector(5, 1);
    std::cout << Var::aa_grid(ws) << '\n';
    
    return 1;
  }


std::map<std::string, NameMaps::Group> groups() {
  std::map<std::string, std::size_t> group;
  for (auto& x: global_data::WsvGroupMap) group[x.first] = x.second;
  std::map<std::string, std::size_t> name;
  for (auto& x: Workspace::wsv_data) name[x.Name()] = x.Group();
  std::map<std::string, std::string> desc;
  for (auto& x: Workspace::wsv_data) desc[x.Name()] = x.Description();
  std::map<std::string, std::size_t> pos;
  for (auto& x: Workspace::WsvMap) pos[x.first] = x.second;
  
  std::map<std::string, NameMaps::Group> out;
  for (auto& x: name) {
    for (auto& y: group) {
      if (y.second == x.second) {
        out[x.first] = {y.first, desc[x.first], pos[x.first]};
        break;
      }
    }
  }
  return out;
}


std::pair<std::vector<std::string>, std::vector<bool>> fixed_defaults(const std::vector<std::string>& vargroups,
                                                                      const std::vector<std::string>& vardefaults) {
  std::vector<std::string> defaults(vargroups.size());
  std::vector<bool> hasdefaults(vargroups.size());
  for (size_t i=0; i<vargroups.size(); i++) {
    if (vardefaults[i] == NODEF) hasdefaults[i] = false;
    else hasdefaults[i] = true;
    
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
    } else if (vardefaults[i]=="[]") {
      defaults[i] = "{}";
    } else {
      defaults[i] = vardefaults[i];
    }
    
    if (defaults[i] == "") defaults[i] = "{}";
    
    for (auto& x: defaults[i]) {
      if (x == '[') x = '{';
      else if (x == ']') x = '}';
    }
  }
  
  return {defaults, hasdefaults};
}

std::map<std::string, NameMaps::Method> methods() {
  std::map<std::string, std::size_t> vargroup;
  for (auto& x: global_data::WsvGroupMap) vargroup[x.first] = x.second;
  std::map<std::string, std::size_t> varpos;
  for (auto& x: Workspace::WsvMap) varpos[x.first] = x.second;
  
  std::map<std::string, std::vector<std::size_t>> gin_group;
  for (auto& x : global_data::md_data_raw) gin_group[x.Name()] = {x.GInType().cbegin(), x.GInType().cend()};
  std::map<std::string, std::vector<std::string>> gin_names;
  for (auto& x : global_data::md_data_raw) gin_names[x.Name()] = {x.GIn().cbegin(), x.GIn().cend()};
  std::map<std::string, std::vector<std::string>> gin_defaults;
  for (auto& x : global_data::md_data_raw) gin_defaults[x.Name()] = {x.GInDefault().cbegin(), x.GInDefault().cend()};
  std::map<std::string, std::vector<std::string>> gin_desc;
  for (auto& x : global_data::md_data_raw) gin_desc[x.Name()] = {x.GInDescription().cbegin(), x.GInDescription().cend()};
  std::map<std::string, std::vector<std::size_t>> gout_group;
  for (auto& x : global_data::md_data_raw) gout_group[x.Name()] = {x.GOutType().cbegin(), x.GOutType().cend()};
  std::map<std::string, std::vector<std::string>> gout_names;
  for (auto& x : global_data::md_data_raw) gout_names[x.Name()] = {x.GOut().cbegin(), x.GOut().cend()};
  std::map<std::string, std::vector<std::string>> gout_desc;
  for (auto& x : global_data::md_data_raw) gout_desc[x.Name()] = {x.GOutDescription().cbegin(), x.GOutDescription().cend()};
  std::map<std::string, std::vector<std::size_t>> in_wspace;
  for (auto& x : global_data::md_data_raw) in_wspace[x.Name()] = {x.In().cbegin(), x.In().cend()};
  std::map<std::string, std::vector<std::size_t>> out_wspace;
  for (auto& x : global_data::md_data_raw) out_wspace[x.Name()] = {x.Out().cbegin(), x.Out().cend()};
  std::map<std::string, std::string> desc;
  for (auto& x : global_data::md_data_raw) desc[x.Name()] = x.Description();
  std::map<std::string, std::vector<std::string>> authors;
  for (auto& x : global_data::md_data_raw) authors[x.Name()] = {x.Authors().cbegin(), x.Authors().cend()};
  
  std::map<std::string, bool> set_method;
  for (auto& x : global_data::md_data_raw) set_method[x.Name()] = x.SetMethod();
  std::map<std::string, bool> agenda_method;
  for (auto& x : global_data::md_data_raw) agenda_method[x.Name()] = x.AgendaMethod();
  std::map<std::string, bool> supergeneric;
  for (auto& x : global_data::md_data_raw) supergeneric[x.Name()] = x.Supergeneric();
  std::map<std::string, bool> uses_templates;
  for (auto& x : global_data::md_data_raw) uses_templates[x.Name()] = x.UsesTemplates();
  std::map<std::string, bool> pass_workspace;
  for (auto& x : global_data::md_data_raw) pass_workspace[x.Name()] = x.PassWorkspace();
  std::map<std::string, bool> pass_wsv_names;
  for (auto& x : global_data::md_data_raw) pass_wsv_names[x.Name()] = x.PassWsvNames();
  
  std::map<std::string, std::vector<std::size_t>> inoutvarpos;
  for (auto& x : global_data::md_data_raw) inoutvarpos[x.Name()] = {x.InOut().cbegin(), x.InOut().cend()};
  std::map<std::string, std::vector<std::size_t>> invarpos;
  for (auto& x : global_data::md_data_raw) invarpos[x.Name()] = {x.InOnly().cbegin(), x.InOnly().cend()};
  std::map<std::string, std::vector<std::size_t>> outvarpos;
  for (auto& x : global_data::md_data_raw) outvarpos[x.Name()] = {x.OutOnly().cbegin(), x.OutOnly().cend()};
  
  std::map<std::string, NameMaps::Method> retval;
  for (auto& x: desc) {
    NameMaps::Method m;
    m.desc = x.second;
    m.authors = authors[x.first];
    
    NameMaps::Method::Gin gin;
    gin.desc = gin_desc[x.first];
    for (auto g: gin_group[x.first]) {
      bool found=false;
      for (auto& y: vargroup) {
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
    
    NameMaps::Method::Gout gout;
    gout.desc = gout_desc[x.first];
    for (auto g: gout_group[x.first]) {
      bool found=false;
      for (auto& y: vargroup) {
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
    
    NameMaps::Method::In in;
    for (auto v: in_wspace[x.first]) {
      bool found=false;
      for (auto& y: varpos) {
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
    
    NameMaps::Method::Out out;
    for (auto v: out_wspace[x.first]) {
      bool found=false;
      for (auto& y: varpos) {
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
    m.agenda_method =  agenda_method[x.first];
    m.supergeneric =  supergeneric[x.first];
    m.uses_templates =  uses_templates[x.first];
    m.pass_workspace =  pass_workspace[x.first];
    m.pass_wsv_names =  pass_wsv_names[x.first];
    m.inoutvarpos = inoutvarpos[x.first];
    retval[x.first] = m;
  }
  
  return retval;
}

std::map<std::string, std::vector<std::size_t>> methods_gin_group() {
  std::map<std::string, std::vector<std::size_t>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GInType().cbegin(), x.GInType().cend()};
  return out;
}

std::map<std::string, std::vector<std::string>> methods_gin_names() {
  std::map<std::string, std::vector<std::string>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GIn().cbegin(), x.GIn().cend()};
  return out;
}

std::map<std::string, std::vector<std::string>> methods_gin_defaults() {
  std::map<std::string, std::vector<std::string>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GInDefault().cbegin(), x.GInDefault().cend()};
  return out;
}

std::map<std::string, std::vector<std::string>> methods_gin_desc() {
  std::map<std::string, std::vector<std::string>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GInDescription().cbegin(), x.GInDescription().cend()};
  return out;
}

std::map<std::string, std::vector<std::size_t>> methods_gout_group() {
  std::map<std::string, std::vector<std::size_t>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GOutType().cbegin(), x.GOutType().cend()};
  return out;
}

std::map<std::string, std::vector<std::string>> methods_gout_names() {
  std::map<std::string, std::vector<std::string>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GOut().cbegin(), x.GOut().cend()};
  return out;
}

std::map<std::string, std::vector<std::string>> methods_gout_desc() {
  std::map<std::string, std::vector<std::string>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.GOutDescription().cbegin(), x.GOutDescription().cend()};
  return out;
}

std::map<std::string, std::vector<std::size_t>> methods_in_wspace() {
  std::map<std::string, std::vector<std::size_t>> out;
  for (auto& x : global_data::md_data_raw) out[x.Name()] = {x.In().cbegin(), x.In().cend()};
  return out;
}

std::map<std::string, std::vector<std::size_t>> methods_out_wspace() {
  std::map<std::string, std::vector<std::size_t>> out;
  for (auto& x : global_data::md_data_raw) { out[x.Name()] = {x.Out().cbegin(), x.Out().cend()}; }
  return out;
}

NameMaps::NameMaps()
{
  varname_group = groups();
  methodname_method = methods();
}


}
