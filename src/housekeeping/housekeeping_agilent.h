#ifndef housekeeping_agilent_h
#define housekeeping_agilent_h

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>

#include "../asio_interface.h"
#include "../python_interface.h"
#include "../timeclass.h"

namespace Instrument::Housekeeping {
class AgilentPython {
  bool manual;
  bool error_found;
  bool new_data;
  std::string error;

  std::map<std::string, double> database;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function runfun;
  Python::Function download;

  Python::Object<Python::Type::Dict> status;

 public:
  using DataType = std::map<std::string, double>;
  
  AgilentPython(const std::filesystem::path &path);

  void startup(const std::string &dev, int baud);
  
  void init(bool manual_press = false) {
    manual = manual_press;
    initfun();
  }
  
  void run() { runfun(); }
  
  void close() {}
  
  bool manual_run() { return manual; }
  
  const std::string &error_string() const { return error; }
  
  bool has_error() { return error_found; }
  
  void delete_error() {
    error_found = false;
    error = "";
  }
  
  void get_data();
  
  DataType data() const { return database; }
};  // AgilentPython

class Agilent34970A {
  bool manual;
  bool error_found;
  bool new_data;
  std::string error;

  std::map<std::string, double> database;

  Network::Serial port;

  std::string results;

  Time T0;
  bool must_read;
  bool has_first;
  bool has_new_data;

 public:
  using DataType = std::map<std::string, double>;
  
  template <typename... Whatever>
  Agilent34970A(Whatever...)
      : manual(false),
        error_found(false),
        new_data(false),
        error(""),
        must_read(true),
        has_first(false),
        has_new_data(false) {}

  void startup(const std::string &dev, unsigned int baud);

  void init(bool manual_press = false);
  
  void run();
  
  void close() { port.close(); }
  
  bool manual_run() { return manual; }
  
  const std::string &error_string() const { return error; }
  
  bool has_error() { return error_found; }
  
  void delete_error() {
    error_found = false;
    error = "";
  }
  
  void get_data();
  
  DataType data() const { return database; }
};  // Agilent34970A
}

#endif  // housekeeping_agilent_h
