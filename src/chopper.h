#ifndef chopper_h
#define chopper_h

#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

#include "enums.h"
#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"

namespace Instrument {
namespace Chopper { 
ENUMCLASS(ChopperPos, unsigned char,
          Cold,
          Hot,
          Antenna,
          Reference
)

template <ChopperPos ... StartPos>
struct Controller {
  static constexpr size_t N = sizeof...(StartPos);
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;
  
  std::string dev;
  int offset;
  double sleeptime;
  
  ChopperPos lasttarget;
  std::array<ChopperPos, N> pos{StartPos...};
  Controller() noexcept : init(false), error(false), quit(false), run(false), operating(false), waiting(false), newdata(false),
  dev("/dev/chopper"), offset(1000), sleeptime(0), lasttarget(ChopperPos::FINAL) {}
};

template <class Chopper, ChopperPos ... Pos>
void GuiSetup(Chopper& chop, Controller<Pos...>& ctrl, const std::vector<std::string>& devs) {
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
      chop.startup(ctrl.dev, ctrl.offset, ctrl.sleeptime);
      chop.init(manual);
    } else {
     chop.close();
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
  
  ImGui::PushItemWidth(150.f);
  if (not ctrl.init) {
    ImGui::SliderInt("Offset [#]", &ctrl.offset, 0, 8000);
  } else {
    int throwaway = ctrl.offset;
    ImGui::SliderInt("Offset [#]", &throwaway, 0, 8000);
  }
  
  ImGui::SameLine();
  
  if (not ctrl.init) {
    float tmp = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &tmp, 0, 5);
    ctrl.sleeptime = tmp;
  }
  else {
    float throwaway = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &throwaway, 0, 5);
  }
  ImGui::PopItemWidth();
  
  if (ctrl.init and chop.manual_run()) {
    if (ImGui::Button(" Cold ")) {
      chop.run(ChopperPos::Cold);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Hot ")) {
      chop.run(ChopperPos::Hot);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Antenna ")) {
      chop.run(ChopperPos::Antenna);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Reference ")) {
      chop.run(ChopperPos::Reference);
    }
    ImGui::SameLine();
    ImGui::Text("Expected state: %s", toString(chop.get_data()).c_str());
  } else {
    ImGui::Text(" Cold ");
    ImGui::SameLine();
    ImGui::Text(" Hot ");
    ImGui::SameLine();
    ImGui::Text(" Antenna ");
    ImGui::SameLine();
    ImGui::Text(" Reference ");
  }
  
  if (chop.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

class Dummy {
  bool manual;
  ChopperPos pos;
  bool error_found;
  std::string error;
public:
  using DataType = ChopperPos;
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), pos(ChopperPos::Cold), error_found(false), error("") {}
  void startup(const std::string&, int, double) {}
  void init(bool manual_press) {manual=manual_press; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void close() {}
  void run(ChopperPos x) {Sleep(0.1); pos = x;}
  DataType get_data_raw() {return ChopperPos::FINAL;}
  DataType get_data() {return pos;}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // Dummy

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
  
  PythonOriginal(const std::filesystem::path& path) : manual(false), pos(ChopperPos::Cold), error_found(false), error("")  {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Chopper python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"chopper"};
  };
  
  void startup(const std::string& dev, int offset, double sleeptime) {
    PyInst = Python::ClassInstance{PyClass(dev, offset, sleeptime)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    hot = Python::Function{PyInst("set_hot")};
    ref = Python::Function{PyInst("set_ref")};
    ant = Python::Function{PyInst("set_ant")};
    cold = Python::Function{PyInst("set_cold")};
    get = Python::Function{PyInst("get_pos")};
  }
  
  // Call once before the operation
  void init(bool manual_press=false) {
    manual=manual_press;
    
    try {
      initfun();
    } catch (const std::exception& e) {
      error = e.what();
      error_found = true;
    }
  }
  
  // Call upon exit for clean getaway
  void close() {shutdown();}
  
  // Main operation
  void run(ChopperPos x) {
    if (pos not_eq x) {
      if (x == ChopperPos::Antenna) {
        ant();
      } else if (x == ChopperPos::Reference) {
        ref();
      } else if (x == ChopperPos::Hot) {
        hot();
      } else if (x == ChopperPos::Cold) {
        cold();
      }
    }
    
    pos = x;
  }
  
  // Queries
  DataType get_data_raw() {return toChopperPos((Python::Object<Python::Type::String>{get()}).toString());}
  DataType get_data() {return pos;}
  
  // Error handling
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // PythonOriginalChopper
}  // Chopper
}  // Instrument

#undef PATH
#endif  // chopper_h
