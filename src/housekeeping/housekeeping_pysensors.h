#ifndef housekeeping_pysensors_h
#define housekeeping_pysensors_h

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>

#include "../python_interface.h"
#include "../timeclass.h"

namespace Instrument::Housekeeping {
class PythonSensors {
  bool manual;
  bool error_found;
  bool new_data;
  std::map<std::string, double> database;
  std::string error;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function download;

  Python::Object<Python::Type::Dict> status;

 public:
  using DataType = std::map<std::string, double>;
  
  PythonSensors(const std::filesystem::path &path);

  void startup(const std::string &dev, int /*baud*/);
  
  void init(bool manual_press = false);
  
  void run() { status = download(); }

  void close() { shutdown(); }

  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }

  void get_data();

  DataType data() const { return database; }
};  // PythonSensors
}

#endif  // housekeeping_pysensors_h
