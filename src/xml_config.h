#ifndef xml_config_h
#define xml_config_h

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "file.h"

namespace File {
class ConfigParser {
  std::map<std::string, std::map<std::string, std::string>> data;
  static constexpr std::string_view nodata = "__NODATA__PLACEHOLDER__";

 public:
  ConfigParser(const std::string &filepath,
               const std::vector<std::string> &targets);

  std::string_view operator()(const std::string &childkey, const std::string &attrkey) const;
  
  std::vector<std::string> keys(const std::string &childkey) const;

  friend std::ostream &operator<<(std::ostream &os, const ConfigParser &cp) {
    for (auto &cfg : cp.data) {
      os << cfg.first << ":\n";
      for (auto &attr : cfg.second)
        os << '\t' << attr.first << ":\t" << attr.second << '\n';
    }
    return os;
  }
};  // ConfigParser
}  // namespace File

#endif  // xml_config_h
