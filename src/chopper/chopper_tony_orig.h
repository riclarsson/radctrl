#ifndef chopper_tony_orig_h
#define chopper_tony_orig_h

#include <filesystem>
#include <string>

#include "chopper_cfg.h"
#include "../python_interface.h"

namespace Instrument::Chopper {
class PythonTony {
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
  static constexpr bool using_device=true;
  
  using DataType = ChopperPos;

  PythonTony(const std::filesystem::path &path);

  void startup(const std::string &ip, int offset, double sleeptime);

  void init(bool manual_press = false);

  void close();

  void run(ChopperPos x);

  DataType get_data_raw();
  
  DataType get_data();

  bool manual_run();
  
  const std::string &error_string() const;
  
  bool has_error();
  
  void delete_error();
};  // PythonTony
}  // Instrument::Chopper

#endif  // chopper_tony_orig_h
