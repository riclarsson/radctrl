#ifndef frontend_dbr_h
#define frontend_dbr_h

#include <filesystem>
#include <string>

#include "../python_interface.h"

namespace Instrument::Frontend {
class DBR {
  bool manual;
  std::string mname;
  std::map<std::string, double> database;
  bool error_found;
  std::string error;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function set_frequency;
  Python::Function get_status;

  Python::Object<Python::Type::Dict> status;
 public:
  static constexpr bool has_cold_load = true;
  static constexpr bool has_hot_load = false;
  static constexpr bool has_config = true;
  using DataType = std::map<std::string, double>;

  DBR(const std::filesystem::path &path)
      : manual(false), mname("DBR"), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find frontend file python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"dbr"};
  }

  void startup(const std::string &server, int port) {
    PyInst = Python::ClassInstance{PyClass(server, port)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    set_frequency = Python::Function{PyInst("set_frequency")};
    get_status = Python::Function{PyInst("get_status_as_dict")};
  }

  void init(bool manual_press = false) {
    manual = manual_press;
    initfun();
  }

  void close() { shutdown(); }

  void run() { status = get_status(); }

  void get_data() {
    auto keys = status.keysDict();
    for (auto &key : keys)
      database[key] = status.fromDict<Python::Type::Double>(key).toDouble();
  }

  DataType data() const { return database; }
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }

  const std::string &name() const { return mname; }

  double cold_load() const { return database.at("cryo.ColdLd.val"); }
};
}

#endif  // frontend_dbr_h
