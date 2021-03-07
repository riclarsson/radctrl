#ifndef frontend_h
#define frontend_h

#include <atomic>
#include <map>
#include <string>
#include <variant>

#include "gui.h"
#include "python_interface.h"

#include "frontend/frontend_cfg.h"

#include "frontend/frontend_waspam.h"
#include "frontend/frontend_dbr.h"

namespace Instrument::Frontend {
class Dummy {
  bool manual;
  std::string mname;
  std::map<std::string, double> database;
  bool error_found;
  std::string error;

 public:
  static constexpr bool has_cold_load = false;
  static constexpr bool has_hot_load = false;
  static constexpr bool has_config = false;
  using DataType = std::map<std::string, double>;

  template <typename... Whatever>
  constexpr Dummy(Whatever...)
      : manual(false),
        mname("FrontendDummy"),
        database({{"NODATA", -1}}),
        error_found(false),
        error("") {}
  template <typename... Whatever>
  void startup(Whatever...) {}
  void init(bool manual_press) {
    manual = manual_press;
    if (not manual) {
      error = "Must be manual, is dummy";
      error_found = true;
    }
  }
  void close() {}
  void run() {}
  void get_data() const {}
  DataType data() const { return database; }
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
  const std::string &name() const { return mname; }
};


typedef std::variant<Dummy, Waspam, DBR> Interface;


struct Controller {
  Interface frontend;
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;

  std::string server;
  int port;

  std::map<std::string, double> data;

  Controller(Interface&& iface, const std::string &s, int p) noexcept
      : frontend(iface),
        init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        server(s),
        port(p) {}
  
  bool has_error();
  
  const std::string& error_string() const;
  
  const std::string& name() const;
  
  void delete_error();
  
  void startup();
  
  void initialize(bool manual);
  
  void close();
  
  void run_machine();
  
  void set_cold(double& cold);
  
  void set_hot(double& hot);
  
  void get_data();
  
  std::map<std::string, double> data_load();
};

void GuiSetup(Controller &ctrl);
}  // namespace Frontend::Instrument

#endif  // frontend_h
