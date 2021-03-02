#include "wobbler_waspam_orig.h"

namespace Instrument::Wobbler {
  PythonOriginalWASPAM::PythonOriginalWASPAM(const std::filesystem::path &path)
  : manual(false), error_found(false), position(4000), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Wobbler python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"wobbler"};
  };
  
  void PythonOriginalWASPAM::startup(std::string &dev, int baudrate, char address) {
    PyInst = Python::ClassInstance{PyClass(dev, baudrate, address)};
    initfun = Python::Function{PyInst("init")};
    movefun = Python::Function{PyInst("move")};
    query = Python::Function{PyInst("getPosition")};
    waitfun = Python::Function{PyInst("wait")};
    shutdown = Python::Function{PyInst("close")};
  }
  
  void PythonOriginalWASPAM::init(int pos, bool manual_press) {
    manual = manual_press;
    
    try {
      initfun(pos);
      position = pos;
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  void PythonOriginalWASPAM::close() { shutdown(); }
  
  void PythonOriginalWASPAM::move(int pos) {
    movefun(pos);
    position = pos;
  }
  
  void PythonOriginalWASPAM::wait() const { waitfun(); }
  
  PythonOriginalWASPAM::DataType PythonOriginalWASPAM::get_data() const { return position; }
  
  PythonOriginalWASPAM::DataType PythonOriginalWASPAM::get_data_raw() const {
    return Python::Object<Python::Type::Int>(query()).toInt();
  }
  
  bool PythonOriginalWASPAM::manual_run() { return manual; }
  
  const std::string &PythonOriginalWASPAM::error_string() const { return error; }
  
  bool PythonOriginalWASPAM::has_error() { return error_found; }
  
  void PythonOriginalWASPAM::delete_error() {
    error_found = false;
    error = "";
  }
}
