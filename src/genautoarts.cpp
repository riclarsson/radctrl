#include <algorithm>
#include <iostream>

#include "arts/interface.h"

int main() {
  ARTS::init();
  
  std::cout <<
  "#include <auto_md.h>" << '\n' <<
  "#include <auto_workspace.h>" << '\n' <<
  "#include \"../../3rdparty/arts/src/arts.h\"" << '\n' <<
  "#include \"../../3rdparty/arts/src/global_data.h\"" << '\n' <<
  "#include \"../../3rdparty/arts/src/agenda_record.h\"" << '\n' <<
  "#include \"../../3rdparty/arts/src/interactive_workspace.h\"" << '\n' << '\n' << '\n';
  
  const auto artsname = ARTS::NameMaps();
  
  std::cout << "namespace ARTS::Var {\n";
  for (auto& x: artsname.varname_group) {
    std::cout << "/*! " << x.second.varname_desc << "*/\n";
    std::cout << x.second.varname_group << '&' << ' ' << x.first << "(Workspace& ws) "
                 "noexcept { "
                 "return *static_cast<" << x.second.varname_group <<" *>(ws[" << x.second.artspos << "]); "
                 "}\n\n";
  }
  std::cout << "}  // ARTS::Var \n\n";
  
  std::cout << "namespace ARTS::Method {\n";
  for (auto& x: artsname.methodname_method) {
    // Skip Agenda and Template methods and also methods that take verbosity
    if (x.second.agenda_method or x.second.uses_templates) continue;
    else if (std::any_of(x.second.in.varname.cbegin(), x.second.in.varname.cend(), [](auto& name){return name == "verbosity";})) continue;
    
    // Describe the method
    std::cout << "/*! " << x.second.desc << '\n';
    for (auto a: x.second.authors) std::cout << "@author " << a << '\n';
    std::cout << "\n" "@param[in,out] Workspace ws - An ARTS workspace\n";
    for (std::size_t i=0; i<x.second.gout.name.size(); i++) std::cout << "@param[out] " << x.second.gout.name[i] << " - " << x.second.gout.desc[i] << "\n";
    for (std::size_t i=0; i<x.second.gin.name.size(); i++) {
      std::cout << "@param[in] " << x.second.gin.name[i] << " - " << x.second.gin.desc[i];
      if (x.second.gin.hasdefs[i])
        std::cout << " (default: " << x.second.gin.defs[i] << ")";
      std::cout << '\n';
    }
    std::cout << "\nUse the ARTS documentation to read more on how the workspace is manipulated\n";
    std::cout << "*/" << '\n';
    
    // Count how many anys we need
    std::size_t anys=0;
    for (auto &name: x.second.gout.group) if (name == "Any") anys++;
    for (auto &name: x.second.gin.group) if (name == "Any") anys++;
    
    // Make it a template if we need any anys
    if (anys) {
      std::cout << "template <";
      for (std::size_t i=0; i<anys; i++) {
        std::cout << "typename Any_"<<i;
        if (i not_eq anys-1)
          std::cout <<", ";
      }
      std::cout << ">\n";
    }
    
    // Now we keep track of our current any count
    std::size_t iany = 0;

    // Make the function
    std::cout << "void " << x.first << "(Workspace& ws";
    
    // First put all GOUT variables
    for (std::size_t i=0; i<x.second.gout.group.size(); i++) {
      if (x.second.gout.group[i] == "Any") {
        std::cout << ',' << '\n' << "Any_" << iany << '&' << ' ' << x.second.gout.name[i];
        iany++;
      } else {
        std::cout << ',' << '\n' << x.second.gout.group[i] << '&' << ' ' << x.second.gout.name[i];
      }
    }
    
    // Second put all GIN variables that have no default argument
    for (std::size_t i=0; i<x.second.gin.group.size(); i++) {
      if (not x.second.gin.hasdefs[i]) {
        if (x.second.gin.group[i] == "Any") {
          std::cout << ',' << '\n' << "const Any_" << iany << '&' << ' ' << x.second.gin.name[i];
          iany++;
        } else {
          std::cout << ',' << "\nconst " << x.second.gin.group[i] << '&' << ' ' << x.second.gin.name[i];
        }
      }
    }
    
    // Lastly put all GIN variables that have a default argument
    for (std::size_t i=0; i<x.second.gin.group.size(); i++) {
      if (x.second.gin.hasdefs[i]) {
        if (x.second.gin.group[i] == "Any") {
          std::cout << ',' << "\nconst Any_" << iany << '&' << ' ' << x.second.gin.name[i] << '=' << x.second.gin.defs[i];
          iany++;
        } else {
          std::cout << ',' << "\nconst " << x.second.gin.group[i] << '&' << ' ' << x.second.gin.name[i] << '=' << x.second.gin.defs[i];
        }
      }
    }
    std::cout << ')' << ' ' << '{' << '\n';
    
    // Call the ARTS auto_md.h function
    std::cout << x.first << '(';
    
    // We need the workspace if we input an Agenda
    if (x.second.pass_workspace or
      std::any_of(x.second.gin.group.cbegin(), x.second.gin.group.cend(), [](auto& g){return g == "Agenda" or g == "ArrayOfAgenda";}) or
      std::any_of(x.second.in.varname.cbegin(), x.second.in.varname.cend(), [&](auto& g){return artsname.varname_group.at(g).varname_group == "Agenda" or artsname.varname_group.at(g).varname_group == "ArrayOfAgenda";}))
      std::cout << "ws,\n";
    for (std::size_t i=0; i<x.second.out.varname.size(); i++) {
      std::cout << "Var::" << x.second.out.varname[i] << "(ws),\n";
    }
    for (std::size_t i=0; i<x.second.gout.name.size(); i++) {
      std::cout << x.second.gout.name[i]<<",\n";
    }
    for (std::size_t i=0; i<x.second.in.varname.size(); i++) {
      if (std::any_of(x.second.out.varname.cbegin(), x.second.out.varname.cend(), [in=x.second.in.varname[i]](const auto& out){return in==out;}))
        continue;
      std::cout << "Var::" << x.second.in.varname[i] << "(ws),\n";
    }
    for (std::size_t i=0; i<x.second.gin.name.size(); i++) {
      std::cout << x.second.gin.name[i]<<",\n";
    }
    
    std::cout << "Var::verbosity(ws)" << ')' << ';' << '\n' << '}' << '\n' << '\n' << '\n';
  }
  std::cout << "}  // ARTS::Method \n\n";
}
