#ifndef housekeeping_h
#define housekeeping_h

#include <map>
#include <string>
#include <vector>

#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Housekeeping {
struct Controller {
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
  
  Controller() noexcept : init(false), error(false), quit(false), run(false), operating(false), waiting(false), newdata(false),
  dev("/dev/sensors"), baudrate(57600), data() {}
};

template <typename Housekeeping>
void GuiSetup(Housekeeping& hk, Controller& ctrl, const std::vector<std::string>& devs) {
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
      hk.startup(ctrl.dev, ctrl.baudrate);
      hk.init(manual);
    } else {
      hk.close();
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
  
  ImGui::PushItemWidth(100.0f);
  ImGui::SameLine();
  if (ctrl.init)
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_ReadOnly);
  else
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll);
  ImGui::PopItemWidth();
  
  if (hk.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

class Dummy {
  bool manual;
  bool error_found;
  bool new_data;
  std::map<std::string, double> database;
  std::string error;
  
public:
  using DataType = std::map<std::string, double>;
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), error_found(false), new_data(false),
  database({
    {"Cold Load Temperature", 18},
    {"Hot Load Temperature", 297}
  }), error("") {}
  void startup(const std::string&, int) {}
  void init(bool manual_press=false) {manual=manual_press; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void run() {}
  void close() {}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  void get_data() const {}
  DataType data() const {return database;}
};  // Dummy

}  // Housekeeping
}  // Instrument

#endif  // housekeeping_h
