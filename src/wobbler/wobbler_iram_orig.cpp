#include "wobbler_iram_orig.h"

namespace Instrument::Wobbler {
  PythonOriginalIRAM::PythonOriginalIRAM(const std::filesystem::path &path)
  : manual(false), error_found(false), position(4000), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Wobbler python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"wobbler"};
  };
  
  void PythonOriginalIRAM::startup(std::string &dev, int baudrate, char address) {
    PyInst = Python::ClassInstance{PyClass(dev, baudrate, address)};
    initfun = Python::Function{PyInst("init")};
    movefun = Python::Function{PyInst("move")};
    query = Python::Function{PyInst("query_position")};
    waitfun = Python::Function{PyInst("wait")};
    shutdown = Python::Function{PyInst("close")};
  }
  
  void PythonOriginalIRAM::init(int pos, bool manual_press) {
    manual = manual_press;
    
    try {
      initfun(pos);
      position = pos;
    } catch (const std::exception &e) {
      error = e.what();
      error_found = true;
    }
  }
  
  void PythonOriginalIRAM::close() { shutdown(); }
  
  void PythonOriginalIRAM::move(int pos) {
    movefun(pos);
    position = pos;
  }
  
  void PythonOriginalIRAM::wait() const { waitfun(); }
  
  PythonOriginalIRAM::DataType PythonOriginalIRAM::get_data() const { return position; }
  
  PythonOriginalIRAM::DataType PythonOriginalIRAM::get_data_raw() const {
    return Python::Object<Python::Type::Int>(query()).toInt();
  }
  
  bool PythonOriginalIRAM::manual_run() { return manual; }
  
  const std::string &PythonOriginalIRAM::error_string() const { return error; }
  
  bool PythonOriginalIRAM::has_error() { return error_found; }
  
  void PythonOriginalIRAM::delete_error() {
    error_found = false;
    error = "";
  }
}
