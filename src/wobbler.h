#ifndef wobbler_h
#define wobbler_h

#include <string>
#include <vector>

#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"

namespace Instrument {
namespace Wobbler {

template <size_t n>
struct Controller {
  static constexpr size_t N = n;
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  
  std::string dev;
  int baudrate;
  char address;

  std::array<int, N> pos;
  Controller(const std::string& d, int b, char a) noexcept : init(false), error(false), quit(false), run(false), operating(false), waiting(false),
  dev(d), baudrate(b), address(a) {pos.fill(0);}
};

template <class Wobbler, size_t N>
void GuiSetup(Wobbler& wob, Controller<N>& ctrl, const std::vector<std::string>& devs) {
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
      wob.init(ctrl.pos[0], manual);
    } else {
      wob.close();
    }
  }
  
  ImGui::PushItemWidth(120.0f);
  if (auto dev = ctrl.dev.c_str(); ImGui::BeginCombo("Device", dev)) {
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
    
    if (not ctrl.init and newselection)
      ctrl.dev = dev;
  }
  ImGui::PopItemWidth();
  
  ImGui::PushItemWidth(30.0f);
  ImGui::SameLine();
  if (ctrl.init)
    ImGui::InputText("Addr", &ctrl.address, 2,  ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_ReadOnly);
  else
    ImGui::InputText("Addr", &ctrl.address, 2,  ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll);
  ImGui::PopItemWidth();
  ImGui::PushItemWidth(100.0f);
  ImGui::SameLine();
  if (ctrl.init)
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_ReadOnly);
  else
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll);
  ImGui::PopItemWidth();
  
  ImGui::PushItemWidth(400.f);
  for (size_t i=0; i<N; i+=4) {
    if (N-i > 3) {
      std::string label = std::string{"Items "} + std::to_string(i) + std::string{"-"} + std::to_string(i+3);
      ImGui::InputInt4(label.c_str(), &ctrl.pos[i], ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
    }
    else if (N-i > 2) {
      std::string label = std::string{"Items "} + std::to_string(i) + std::string{"-"} + std::to_string(i+2);
      ImGui::InputInt3(label.c_str(), &ctrl.pos[i], ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
    }
    else if (N-i > 1) {
      std::string label = std::string{"Items "} + std::to_string(i) + std::string{"-"} + std::to_string(i+1);
      ImGui::InputInt2(label.c_str(), &ctrl.pos[i], ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
    }
    else {
      std::string label = std::string{"Item "} + std::to_string(i);
      ImGui::InputInt(label.c_str(), &ctrl.pos[i], 1, 100, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
    }
  }
  ImGui::PopItemWidth();
  
  ImGui::Text("Target Position: %i", wob.get_data()); 
  
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
  void wait() const {Sleep(0.1);}
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
  Python::Function waitfun;
  Python::Function query;
  
public:
  using DataType = int;
  
  PythonOriginalWASPAM(const std::filesystem::path& path) : manual(false), error_found(false), position(4000), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Wobbler python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"wobbler"};
  }
  
  void startup(std::string& dev, int baudrate, char address) {
    PyInst = Python::ClassInstance{PyClass(dev, baudrate, address)};
    initfun = Python::Function{PyInst("init")};
    movefun = Python::Function{PyInst("move")};
    query = Python::Function{PyInst("getPosition")};
    waitfun = Python::Function{PyInst("wait")};
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
  void wait() const {waitfun();}
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
  Python::Function waitfun;
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
    waitfun = Python::Function{PyInst("wait")};
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
  void wait() const {waitfun();}
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
