#include "chopper.h"

namespace Instrument::Chopper {
void Controller::startup() {
  std::visit([this](auto&& chop){chop.startup(dev_or_ip, offset, sleeptime);}, chop);
}

void Controller::initialize(bool manual) {
  std::visit([manual](auto&& chop){chop.init(manual);}, chop);
  init = true;
}

void Controller::close() {
  std::visit([](auto&& chop){chop.close();}, chop);
  init = false;
}

bool Controller::manual_run() {
  return std::visit([](auto&& chop){return chop.manual_run();}, chop);
}

void Controller::change_pos(ChopperPos pos) {
  std::visit([pos](auto&& chop){chop.run(pos);}, chop);
}

ChopperPos Controller::get_pos() {
  return std::visit([](auto&& chop){return chop.get_data();}, chop);
}

bool Controller::has_error() {
  return std::visit([](auto&& chop){return chop.has_error();}, chop);
}

const std::string& Controller::error_string() const {
  return std::visit([](auto&& chop) -> const std::string& {return chop.error_string();}, chop);
}

void Controller::delete_error() {
  std::visit([](auto&& chop){chop.delete_error();}, chop);
}

bool Controller::uses_ip() const {
  return std::visit([](auto&& chop){return not chop.using_device;}, chop);
}

void GuiSetup(Controller &ctrl,
              const std::vector<std::string> &devs) {
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
      ctrl.startup();
      ctrl.initialize(manual);
    } else {
      ctrl.close();
    }
  }

  if (ctrl.uses_ip()) {
    auto dev = ctrl.dev_or_ip;
    if (ImGui::InputText("IP", &ctrl.dev_or_ip, ImGuiInputTextFlags_EnterReturnsTrue)) {
      if (not ctrl.init) {
        ctrl.dev_or_ip = dev;
      }
    }
  } else {
    auto dev = ctrl.dev_or_ip.c_str();
    if (ImGui::BeginCombo("Device", dev)) {
      bool newselection = false;
      for (auto &d : devs) {
        auto dev2 = d.c_str();
        bool is_selected =
        (dev == dev2);  // You can store your selection however you want,
        // outside or inside your objects
        if (ImGui::Selectable(dev2, is_selected)) {
          newselection = true;
          dev = dev2;
        }
      }
      
      ImGui::EndCombo();
      
      if (not ctrl.init) {
        if (newselection) {
          ctrl.dev_or_ip = dev;
        }
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
  } else {
    float throwaway = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &throwaway, 0, 5);
  }
  ImGui::PopItemWidth();

  if (ctrl.init and ctrl.manual_run()) {
    if (ImGui::Button(" Cold ")) {
      ctrl.change_pos(ChopperPos::Cold);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Hot ")) {
      ctrl.change_pos(ChopperPos::Hot);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Antenna ")) {
      ctrl.change_pos(ChopperPos::Antenna);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Reference ")) {
      ctrl.change_pos(ChopperPos::Reference);
    }
    ImGui::SameLine();
    ImGui::Text("Expected state: %s", toString(ctrl.get_pos()).c_str());
  } else {
    ImGui::Text(" Cold ");
    ImGui::SameLine();
    ImGui::Text(" Hot ");
    ImGui::SameLine();
    ImGui::Text(" Antenna ");
    ImGui::SameLine();
    ImGui::Text(" Reference ");
  }

  if (ctrl.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

Controller parse(const File::ConfigParser& parser) try {
  const std::string_view type = parser("Chopper", "type");
  
  if (type == "PythonOriginal") {
    return  Controller(
      PythonOriginal(parser("Chopper", "path")),
      std::string(parser("Chopper", "dev")),
      std::stoi(std::string(parser("Chopper", "offset"))),
      std::stod(std::string(parser("Chopper", "sleeptime"))),
      ChopperPos::Cold,
      ChopperPos::Antenna,
      ChopperPos::Hot,
      ChopperPos::Antenna
    );
  } else if (type == "PythonTony") {
    return  Controller(
      PythonTony(parser("Chopper", "path")),
      std::string(parser("Chopper", "ip")),
      std::stoi(std::string(parser("Chopper", "offset"))),
      std::stod(std::string(parser("Chopper", "sleeptime"))),
      ChopperPos::Cold,
      ChopperPos::Antenna,
      ChopperPos::Hot,
      ChopperPos::Antenna
    );
  } else {
    std::ostringstream os;
    os << "Cannot understand type: " << type;
    throw std::runtime_error(os.str());
  }
} catch (std::exception& e) {
  std::ostringstream os;
  os << "Cannot parse Chopper with error:\n" << e.what();
  throw std::runtime_error(os.str());
}
}  // namespace Instrument::Chopper
