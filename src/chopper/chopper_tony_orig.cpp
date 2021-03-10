#include "chopper_tony_orig.h"

namespace Instrument::Chopper {
  PythonTony::PythonTony(const std::filesystem::path &path)
  : manual(false), pos(ChopperPos::Cold), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Chopper python file at:\n\t" << path.relative_path()
      << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"chopper"};
  };
  
  void PythonTony::startup(const std::string &ip, int offset, double sleeptime) {
    PyInst = Python::ClassInstance{PyClass(ip, offset, sleeptime)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    hot = Python::Function{PyInst("set_hot")};
    ref = Python::Function{PyInst("set_ref")};
    ant = Python::Function{PyInst("set_ant")};
    cold = Python::Function{PyInst("set_cold")};
    get = Python::Function{PyInst("get_pos")};
  }
  
  void PythonTony::init(bool manual_press) {
    manual = manual_press;
    
    try {
      initfun();
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  void PythonTony::close() {
    try {
      shutdown();
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  void PythonTony::run(ChopperPos x) {
    try {
      if (x == ChopperPos::Antenna) {
        ant();
      } else if (x == ChopperPos::Reference) {
        ref();
      } else if (x == ChopperPos::Hot) {
        hot();
      } else if (x == ChopperPos::Cold) {
        cold();
      }
      
      pos = x;
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  PythonTony::DataType PythonTony::get_data_raw() {
    try {
      return toChopperPos(
        (Python::Object<Python::Type::String>{get()}).toString());
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
      return ChopperPos::FINAL;
    }
  }
  
  PythonTony::DataType PythonTony::get_data() { return pos; }
  
  bool PythonTony::manual_run() { return manual; }
  
  const std::string &PythonTony::error_string() const { return error; }
  
  bool PythonTony::has_error() { return error_found; }
  
  void PythonTony::delete_error() {
    error_found = false;
    error = "";
  }
}
