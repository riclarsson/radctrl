#include "xml_config.h"

namespace File {
std::filesystem::path relative_path(const std::string_view path, const std::filesystem::path& relpath) {
  // Adjust for possible relative paths
  if (path.size() and path[0] not_eq '/') {
    auto thispath = relpath;
    thispath /= path;
    return thispath.string();
  } else {
    return path;
  }
}

std::vector<std::string> split_list(const std::string& chars) {
  std::istringstream iss(chars);
  return std::vector<std::string>{std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
}

ConfigParser::ConfigParser(const std::string &filepath,
                            const std::vector<std::string> &targets) {
  File<Operation::Read, Type::Xml> file(filepath);

  const std::filesystem::path relpath{std::filesystem::path(filepath).remove_filename()};

  for (auto &key : targets) {
    if (key == std::string{"default"}) {
      throw std::runtime_error("Cannot give default values in config");
    }

    auto child = file.get_child(key.c_str());
    std::map<std::string, std::string> attr_map;

    bool has_vector=false;
    for (auto x = child.attributes_begin(); x not_eq child.attributes_end(); x++) {
      std::string attr_key{x->name()};
      std::string attr_val{x->value()};
    
      if (attr_key == "default") {
        throw std::runtime_error("Cannot give default values in config");
      } else if (attr_key.substr(0, 4) == "path" or attr_key.substr(0, 6) == "config") {
        attr_map[attr_key] = relative_path(attr_val, relpath);
      } else if (attr_key == "type" and attr_val == "VectorOfConfig") {
        if (auto vals = split_list(child.child_value()); 2*file.size() == vals.size()) {
          for (int i=0; i<vals.size(); i+=2) {
            attr_map[vals[i]] = relative_path(vals[i+1], relpath);
            attr_map[vals[i] + std::string(" pos")] = std::to_string(i/2);
          }
          has_vector = true;
        } else {
          throw std::runtime_error("Mismatch size and count of VectorOfConfig values");
        }
      } else {
        attr_map[x->name()] = attr_val;
      }
    }

    // Make sure no weird attributes have been defined
    if (has_vector) {
      if (std::size_t(2 * file.size() + 1) not_eq attr_map.size()) {
        throw std::runtime_error("A child with VectorOfConfig can only contain \"type\" and \"size\"\n"
        "attributes, and the names of the configurations must be unique");
      }
    } else {
      attr_map[key + std::string(" data")] = child.child_value();
    }
    
    data[key] = attr_map;
    file.leave_child();
  }
  file.close();
}

std::string_view ConfigParser::operator()(const std::string &childkey, const std::string &attrkey) const {
  auto attr = data.find(childkey);
  if (attr == data.end()) {
    return nodata;
  } else {
    auto val = attr->second.find(attrkey);
    if (val == attr->second.end()) {
      return nodata;
    } else {
      return val->second;
    }
  }
}

std::vector<std::string> ConfigParser::keys(const std::string &childkey) const {
  std::vector<std::string> out;
  auto attr = data.find(childkey);
  if (attr not_eq data.end()) {
    for (auto& pair: attr -> second) {
      out.emplace_back(pair.first);
    }
  }
  return out;
}
}
