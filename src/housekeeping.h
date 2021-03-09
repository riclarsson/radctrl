#ifndef housekeeping_h
#define housekeeping_h

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "asio_interface.h"
#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"
#include "xml_config.h"

#include "housekeeping/housekeeping_agilent.h"
#include "housekeeping/housekeeping_pysensors.h"

namespace Instrument::Housekeeping {
class Dummy {
  bool manual;
  bool error_found;
  bool new_data;
  std::map<std::string, double> database;
  std::string error;

 public:
  using DataType = std::map<std::string, double>;
  template <typename... Whatever>
  constexpr Dummy(Whatever...)
      : manual(false),
        error_found(false),
        new_data(false),
        database(
            {{"Cold Load Temperature", 18}, {"Hot Load Temperature", 297}}),
        error("") {}
  void startup(const std::string &, int) {}
  void init(bool manual_press = false) {
    manual = manual_press;
    if (not manual) {
      error = "Must be manual, is dummy";
      error_found = true;
    }
  }
  void run() {}
  void close() {}
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
  void get_data() const {}
  DataType data() const { return database; }
};  // Dummy

typedef std::variant<Dummy, AgilentPython, Agilent34970A, PythonSensors> Interface;
  
struct Controller {
  Interface hk;
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;

  std::string dev;
  int baudrate;

  std::map<std::string, double> data;

  Controller(Interface&& iface, const std::string &d, int b) noexcept
      : hk(iface),
        init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        dev(d),
        baudrate(b),
        data() {}
  
  void startup();
  
  void initialize(bool manual);
  
  void close();
  
  bool has_error();
  
  void run_machine();
  
  void get_data();
  
  std::map<std::string, double> data_load();
  
  void delete_error();
  
  const std::string& error_string() const;
};

void GuiSetup(Controller &ctrl, const std::vector<std::string> &devs);

Controller parse(const File::ConfigParser& parser);
}  // namespace Instrument::Housekeeping

#endif  // housekeeping_h
