#ifndef arts_interface_h
#define arts_interface_h

#include <map>
#include <string>
#include <vector>

namespace ARTS {
  void init();
  
  struct NameMaps {
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
    
    std::map<std::string, Method> methodname_method;
    
    struct Group {
      std::string varname_group;
      std::string varname_desc;
      std::size_t artspos;
    };
    
    std::map<std::string, Group> varname_group;
    
    NameMaps();
  };
  
  double test();
}

#endif  // arts_interface_h
