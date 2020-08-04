#ifndef wobbler_h
#define wobbler_h

#include <string>

#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Wobbler {
struct GUI {
  bool init;
  bool error;
  std::string dev;
  int baudrate;
  char address;
  int pos;
  int delta_pos;
  GUI() noexcept : init(false), error(false), dev("/dev/wobbler"), baudrate(115200), address('0'), pos(4000), delta_pos(3000) {}
};

template <class Wobbler>
void GuiSetup(Wobbler& wob, GUI& ctrl, const std::vector<std::string>& devs) {
  bool change=false;
  bool manual=false;
  if (not ctrl.init) {
    if (ImGui::Button(" Manual Init ")) {
      change = true;
      manual = true;
      ctrl.init = true;
    } ImGui::SameLine();
    if (ImGui::Button(" Automatic Init ")) {
      change = true;
      manual = false;
      ctrl.init = true;
    } ImGui::SameLine();
    ImGui::Text(" Close ");
  } else  {
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
      wob.startup(ctrl.dev, ctrl.baudrate, ctrl.address);
      wob.init(ctrl.pos, manual);
    } else {
      wob.close();
    }
  }
  
  auto dev = ctrl.dev.c_str();
  if (ImGui::BeginCombo("Device", dev)) {
    bool newselection=false;
    for (auto& d: devs) {
      auto dev2 = d.c_str();
      bool is_selected = (dev == dev2); // You can store your selection however you want, outside or inside your objects
      if (ImGui::Selectable(dev2, is_selected)) {
        newselection = true;
        dev = dev2;
      }
    }
    
    ImGui::EndCombo();
    
    if (not ctrl.init) {
      if (newselection) {
        ctrl.dev = dev;
      }
    }
  }
  
  if (not ctrl.init) {
    ImGui::SliderInt("Start Position [#]", &ctrl.pos, 0, 40000);
  } else {
    int throwaway = ctrl.pos;
    ImGui::SliderInt("Start Position [#]", &throwaway, 0, 40000);
  }
  
  if (not ctrl.init) {
    ImGui::SliderInt("Delta Position [#]", &ctrl.delta_pos, -40000, 40000);
  } else {
    int throwaway = ctrl.delta_pos;
    ImGui::SliderInt("Delta Position [#]", &throwaway, -40000, 40000);
  }
  
  if (not ctrl.init) {
    ImGui::Text(" Move Back ");
    ImGui::SameLine();
    ImGui::Text(" Move Forward ");
  } else {
    if (ImGui::Button(" Move Back ")) {
      wob.move(wob.get_data() - ctrl.delta_pos);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Move Forward ")) {
      wob.move(wob.get_data() + ctrl.delta_pos);
    }
    ImGui::SameLine();
    ImGui::Text("Expected state: %i", wob.get_data());
  }
  
  if (wob.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

class Dummy {
  bool manual;
  bool error_found;
  int position;
  std::string error;
  
public:
  using DataType = int;
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), error_found(false), position(4000), error("") {}
  void startup(const std::string&, int, char) {}
  void init(int pos, bool manual_press=false) {manual=manual_press; position=pos; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void close() {}
  void move(int pos) {position=pos;}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  DataType get_data() const {return position;}
};  // Dummy

class PythonOriginalWASPAM {
  bool manual;
  bool error_found;
  int position;
  std::string error;
  
  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function movefun;
  Python::Function shutdown;
  Python::Function query;
  
public:
  using DataType = int;
  
  PythonOriginalWASPAM(const std::filesystem::path& path) : manual(false), error_found(false), position(4000), error("")  {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Wobbler python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"wobbler"};
  };
  
  void startup(std::string& dev, int baudrate, char address) {
    PyInst = Python::ClassInstance{PyClass(dev, baudrate, address)};
    initfun = Python::Function{PyInst("init")};
    movefun = Python::Function{PyInst("move")};
    query = Python::Function{PyInst("getPosition")};
    shutdown = Python::Function{PyInst("close")};
  }
  
  // Call once before the operation
  void init(int pos, bool manual_press=false) {
    manual=manual_press;
    
    try {
      initfun(pos);
      position=pos;
    } catch (const std::exception& e) {
      error = e.what();
      error_found = true;
    }
  }
  
  // Call upon exit for clean getaway
  void close() {shutdown();}
  
  // Main operation
  void move(int pos) {movefun(pos); position=pos;}
  
  // Queries
  DataType get_data() const {return position;}
  DataType get_data_raw() const {return Python::Object<Python::Type::Int>(query()).toInt();}

  // Error handling
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // PythonOriginalWASPAM

class PythonOriginalIRAM {
  bool manual;
  bool error_found;
  int position;
  std::string error;
  
  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function movefun;
  Python::Function shutdown;
  Python::Function query;
  
public:
  using DataType = int;
  
  PythonOriginalIRAM(const std::filesystem::path& path) : manual(false), error_found(false), position(4000), error("")  {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Wobbler python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"wobbler"};
  };
  
  void startup(std::string& dev, int baudrate, char address) {
    PyInst = Python::ClassInstance{PyClass(dev, baudrate, address)};
    initfun = Python::Function{PyInst("init")};
    movefun = Python::Function{PyInst("move")};
    query = Python::Function{PyInst("query_position")};
    shutdown = Python::Function{PyInst("close")};
  }
  
  // Call once before the operation
  void init(int pos, bool manual_press=false) {
    manual=manual_press;
    
    try {
      initfun(pos);
      position=pos;
    } catch (const std::exception& e) {
      error = e.what();
      error_found = true;
    }
  }
  
  // Call upon exit for clean getaway
  void close() {shutdown();}
  
  // Main operation
  void move(int pos) {movefun(pos); position=pos;}
  
  // Queries
  DataType get_data() const {return position;}
  DataType get_data_raw() const {return Python::Object<Python::Type::Int>(query()).toInt();}
  
  // Error handling
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // PythonOriginalIRAM
}  // Wobbler
}  // Instrument

#endif  // wobbler_h
