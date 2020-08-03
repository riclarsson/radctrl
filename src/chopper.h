#ifndef chopper_h
#define chopper_h

#include <filesystem>
#include <string>
#include <vector>

#include "enums.h"
#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Chopper { 
ENUMCLASS(ChopperPos, unsigned int,
          Cold,
          Hot,
          Antenna,
          Reference
)
struct GUI {
  bool init;
  std::string dev;
  int offset;
  double sleeptime;
  GUI() noexcept : init(false), dev("/dev/chopper"), offset(1000), sleeptime(0) {}
};

template <class Chopper>
void GuiSetup(Chopper& chop, GUI& ctrl, const std::vector<std::string>& devs) {
  ImGui::Text("Chopper control");
  
  bool change=false;
  if (not ctrl.init) {
    if (ImGui::Button(" Initialize ")) {
      change = true;
      ctrl.init = true;
    } ImGui::SameLine();
    ImGui::Text(" Close ");
  } else  {
    ImGui::Text(" Initialize ");
    ImGui::SameLine();
    if (ImGui::Button(" Close ")) {
      change = true;
      ctrl.init = false;
    }
  }
  
  if (change) {
    if (ctrl.init) {
      chop.startup(ctrl.dev, ctrl.offset, ctrl.sleeptime);
//       chop.init();  // FIXME: Must be here...
    } else {
//      chop.close();  // FIXME: Must be here...
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
    ImGui::SliderInt("Offset [#]", &ctrl.offset, 0, 8000);
  } else {
    int throwaway = ctrl.offset;
    ImGui::SliderInt("Offset [#]", &throwaway, 0, 8000);
  }
  
  if (not ctrl.init) {
    float tmp = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &tmp, 0, 5);
    ctrl.sleeptime = tmp;
  }
  else {
    float throwaway = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &throwaway, 0, 5);
  }
  
  if (ctrl.init) {
    if (ImGui::Button("Cold"))
      chop.run(ChopperPos::Cold);
    ImGui::SameLine();
    if (ImGui::Button("Hot"))
      chop.run(ChopperPos::Hot);
    ImGui::SameLine();
    if (ImGui::Button("Antenna"))
      chop.run(ChopperPos::Antenna);
    ImGui::SameLine();
    if (ImGui::Button("Reference"))
      chop.run(ChopperPos::Reference);
  } else {
    ImGui::Text("Cold");
    ImGui::SameLine();
    ImGui::Text("Hot");
    ImGui::SameLine();
    ImGui::Text("Antenna");
    ImGui::SameLine();
    ImGui::Text("Reference");
  }
}

class Dummy {
ChopperPos pos;
public:
using DataType = ChopperPos;
template <typename ... Whatever> Dummy(Whatever...) : pos(ChopperPos::Cold) {}
void startup(const std::string&, int, double) {}
void init() {}
void close() {}
void run(ChopperPos x) {pos = x;}
DataType get_data() {return pos;}
};  // Dummy

class __attribute__((visibility("hidden"))) PythonOriginalChopper {
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

public:
  using DataType = ChopperPos;
  
  PythonOriginalChopper(const std::filesystem::path& path) : pos(ChopperPos::Cold) {
    if (not std::filesystem::exists(path))
      throw std::runtime_error("Cannot find Chopper file");
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
  
  void init() {
    initfun();
  }
  
  void close() {
    shutdown();
  }
  
  void run(ChopperPos x) {
    if (x == ChopperPos::Antenna)
      ant();
    else if (x == ChopperPos::Reference)
      ref();
    else if (x == ChopperPos::Hot)
      hot();
    else if (x == ChopperPos::Cold)
      cold();
    
    pos = x;
  }
  
  
  
  DataType get_data() {return toChopperPos((Python::Object<Python::Type::String>{get()}).toString());}
};  // PythonOriginalChopper
}  // Chopper
}  // Instrument

#undef PATH
#endif  // chopper_h
