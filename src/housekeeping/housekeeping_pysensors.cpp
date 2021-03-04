#include "housekeeping_pysensors.h"

namespace Instrument::Housekeeping {
  PythonSensors::PythonSensors(const std::filesystem::path &path)
  : manual(false), error_found(false), new_data(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find python sensors file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"sensors"};
  };
  
  void PythonSensors::startup(const std::string &dev, int /*baud*/) {
    PyInst = Python::ClassInstance{PyClass(dev)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    download = Python::Function{PyInst("get_status_as_dict")};
  }
  
  void PythonSensors::init(bool manual_press) {
    manual = manual_press;
    try {
      initfun();
    } catch (const std::exception &e) {
      error_found = true;
      error += std::string_view{e.what()};
    }
  }
  
  void PythonSensors::get_data() {
    auto keys = status.keysDict();
    for (auto &key : keys)
      database[key] = status.fromDict<Python::Type::Double>(key).toDouble();
  }
}
