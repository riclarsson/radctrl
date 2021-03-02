#ifndef chopper_orig_h
#define chopper_orig_h

#include <filesystem>
#include <string>

#include "chopper_cfg.h"
#include "../python_interface.h"

namespace Instrument::Chopper {
class PythonOriginal {
  bool manual;
  ChopperPos pos;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function hot;
  Python::Function cold;
  Python::Function ant;
  Python::Function ref;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function get;

  bool error_found;
  std::string error;

 public:
  using DataType = ChopperPos;

  PythonOriginal(const std::filesystem::path &path);

  void startup(const std::string &dev, int offset, double sleeptime);

  void init(bool manual_press = false);

  void close();

  void run(ChopperPos x);

  DataType get_data_raw();
  
  DataType get_data();

  bool manual_run();
  
  const std::string &error_string() const;
  
  bool has_error();
  
  void delete_error();
};  // PythonOriginal
}  // Instrument::Chopper

#endif  // chopper_orig_h
