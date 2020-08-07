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
  
  std::map<std::string, std::string> data;
  
  Controller() noexcept : init(false), error(false), quit(false), run(false), operating(false), waiting(false), newdata(false) {}
};

class Dummy {
  bool manual;
  std::string mname;
  std::map<std::string, std::string> database;
  bool error_found;
  std::string error;
  
public:
  using DataType = std::map<std::string, std::string>;
  
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), mname("FrontendDummy"), database({{"NODATA", "NODATA"}}), error_found(false), error("") {}
  template <typename ... Whatever> void startup(Whatever...) {}
  void init(bool manual_press) {manual=manual_press; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void close() {}
  void run() {}
  void get_data() const {}
  DataType data() const {return database;}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  void gui_setup(Controller&) {ImGui::Text("There is no setup, this is a dummy class");}
  const std::string& name() const {return mname;}
};

template <class Frontend>
void GuiSetup(Frontend& frontend, Controller& ctrl) {
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
      frontend.startup(/*FIXME????FIXME*/);
      frontend.init(manual);
    } else {
      frontend.close();
    }
  }
  
  // Each frontend will be very different so the setup must be dealt with individually
  frontend.gui_setup(ctrl);
  
  if (frontend.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

}  // Frontend
}  // Instrument

#endif  // frontend_h
