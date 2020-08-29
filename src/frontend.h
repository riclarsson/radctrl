#ifndef frontend_h
#define frontend_h

#include <atomic>
#include <map>
#include <string>

#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Frontend {
struct Controller {
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

  Controller(const std::string &s, int p) noexcept
      : init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        server(s),
        port(p) {}
};

template <class Frontend>
void GuiSetup(Frontend &frontend, Controller &ctrl) {
  bool change = false;
  bool manual = false;
  if (not ctrl.init) {
    if (ImGui::Button(" Manual Init ")) {
      change = true;
      manual = true;
      ctrl.init = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Automatic Init ")) {
      change = true;
      manual = false;
      ctrl.init = true;
    }
    ImGui::SameLine();
    ImGui::Text(" Close ");
  } else {
    ImGui::Text(" Manual Init ");
    ImGui::SameLine();
    ImGui::Text(" Automatic Init ");
    ImGui::SameLine();
    if (ImGui::Button(" Close ")) {
      change = true;
      ctrl.init = false;
    }
  }

  if (change) {
    if (ctrl.init) {
      frontend.startup(ctrl.server, ctrl.port);
      frontend.init(manual);
    } else {
      frontend.close();
    }
  }

  // Each frontend will be very different so the setup must be dealt with
  // individually
  frontend.gui_setup(ctrl);

  if (frontend.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

class Dummy {
  bool manual;
  std::string mname;
  std::map<std::string, double> database;
  bool error_found;
  std::string error;

 public:
  static constexpr bool has_cold_load = false;
  static constexpr bool has_hot_load = false;
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
  void gui_setup(Controller &) {
    ImGui::Text("There is no setup, this is a dummy class");
  }
  const std::string &name() const { return mname; }
};

class Waspam {
  bool manual;
  std::string mname;
  std::map<std::string, double> database;
  bool error_found;
  std::string error;

 public:
  static constexpr bool has_cold_load = false;
  static constexpr bool has_hot_load = false;
  using DataType = std::map<std::string, double>;

  template <typename... Whatever>
  constexpr Waspam(Whatever...)
      : manual(false), mname("Waspam"), error_found(false), error("") {}
  template <typename... Whatever>
  void startup(Whatever...) {}
  void init(bool manual_press) { manual = manual_press; }
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
  void gui_setup(Controller &) {
    ImGui::Text("There is no setup for Waspam frontend");
  }
  const std::string &name() const { return mname; }
};

class DBR {
  bool manual;
  std::string mname;
  std::map<std::string, double> database;
  bool error_found;
  std::string error;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function set_frequency;
  Python::Function get_status;

  Python::Object<Python::Type::Dict> status;

 public:
  static constexpr bool has_cold_load = true;
  static constexpr bool has_hot_load = false;
  using DataType = std::map<std::string, double>;

  DBR(const std::filesystem::path &path)
      : manual(false), mname("DBR"), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find frontend file python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"dbr"};
  }

  void startup(const std::string &server, int port) {
    PyInst = Python::ClassInstance{PyClass(server, port)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    set_frequency = Python::Function{PyInst("set_frequency")};
    get_status = Python::Function{PyInst("get_status_as_dict")};
  }

  void init(bool manual_press = false) {
    manual = manual_press;
    initfun();
  }

  void close() { shutdown(); }

  void run() { status = get_status(); }

  void get_data() {
    auto keys = status.keysDict();
    for (auto &key : keys)
      database[key] = status.fromDict<Python::Type::Double>(key).toDouble();
  }

  DataType data() const { return database; }
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
  void gui_setup(Controller &ctrl) {
    ImGui::PushItemWidth(180.0f);
    if (ctrl.init) {
      ImGui::InputText("Server", &ctrl.server,
                       ImGuiInputTextFlags_CharsNoBlank |
                           ImGuiInputTextFlags_AutoSelectAll |
                           ImGuiInputTextFlags_NoHorizontalScroll |
                           ImGuiInputTextFlags_ReadOnly);
    } else {
      ImGui::InputText("Server", &ctrl.server,
                       ImGuiInputTextFlags_CharsNoBlank |
                           ImGuiInputTextFlags_AutoSelectAll |
                           ImGuiInputTextFlags_NoHorizontalScroll);
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    if (ctrl.init) {
      ImGui::InputInt("port", &ctrl.port, 1, 100,
                      ImGuiInputTextFlags_AutoSelectAll |
                          ImGuiInputTextFlags_NoHorizontalScroll |
                          ImGuiInputTextFlags_ReadOnly);
    } else {
      ImGui::InputInt("port", &ctrl.port, 1, 100,
                      ImGuiInputTextFlags_AutoSelectAll |
                          ImGuiInputTextFlags_NoHorizontalScroll);
    }
    ImGui::PopItemWidth();

    if (ctrl.init and manual_run()) {
      if (ImGui::Button(" Run ")) {
        run();
        get_data();
        ctrl.data = data();
      }
    } else {
      ImGui::Text(" Run ");
    }
  }

  const std::string &name() const { return mname; }

  double cold_load() const { return database.at("cryo.ColdLd.val"); }
};

}  // namespace Frontend

}  // namespace Instrument

#endif  // frontend_h
