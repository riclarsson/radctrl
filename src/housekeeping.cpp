#include "housekeeping.h"

namespace Instrument::Housekeeping {
void Controller::startup() {
  std::visit([this](auto && hk){hk.startup(dev, baudrate);}, hk);
}

void Controller::initialize(bool manual) {
  std::visit([manual](auto && hk){hk.init(manual);}, hk);
  init = true;
}

void Controller::close() {
  std::visit([](auto && hk){hk.close();}, hk);
  init = false;
}

bool Controller::has_error() {
  return std::visit([](auto && hk){return hk.has_error();}, hk);
}

void Controller::run_machine() {
  std::visit([](auto && hk){hk.run();}, hk);
}

void Controller::get_data() {
  std::visit([](auto && hk){hk.get_data();}, hk);
}

std::map<std::string, double> Controller::data_load() {
  return std::visit([](auto && hk){return hk.data();}, hk);
}

void Controller::delete_error() {
  std::visit([](auto && hk){hk.delete_error();}, hk);
}

const std::string& Controller::error_string() const {
  return std::visit([](auto && hk) -> const std::string& {return hk.error_string();}, hk);
}
  
void GuiSetup(Controller &ctrl, const std::vector<std::string> &devs) {
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

  ImGui::PushItemWidth(120.0f);
  if (auto dev = ctrl.dev.c_str(); ImGui::BeginCombo("Device", dev)) {
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

    if (not ctrl.init and newselection) ctrl.dev = dev;
  }

  ImGui::PushItemWidth(100.0f);
  ImGui::SameLine();
  if (ctrl.init)
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,
                    ImGuiInputTextFlags_CharsDecimal |
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
  else
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,
                    ImGuiInputTextFlags_CharsDecimal |
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
  ImGui::PopItemWidth();

  if (ctrl.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}
}  // namespace Instrument::Housekeeping
