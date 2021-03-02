#include "chopper.h"

namespace Instrument::Chopper {
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
      std::visit([&ctrl](auto&& chop){chop.startup(ctrl.dev, ctrl.offset, ctrl.sleeptime);}, ctrl.chop);
      std::visit([manual](auto&& chop){chop.init(manual);}, ctrl.chop);
    } else {
      std::visit([](auto&& chop){chop.close();}, ctrl.chop);
    }
  }

  auto dev = ctrl.dev.c_str();
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
  } else {
    float throwaway = float(ctrl.sleeptime);
    ImGui::SliderFloat("Sleeptime [s]", &throwaway, 0, 5);
  }
  ImGui::PopItemWidth();

  if (ctrl.init and std::visit([](auto&& chop){return chop.manual_run();}, ctrl.chop)) {
    if (ImGui::Button(" Cold ")) {
      std::visit([](auto&& chop){chop.run(ChopperPos::Cold);}, ctrl.chop);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Hot ")) {
      std::visit([](auto&& chop){chop.run(ChopperPos::Hot);}, ctrl.chop);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Antenna ")) {
      std::visit([](auto&& chop){chop.run(ChopperPos::Antenna);}, ctrl.chop);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Reference ")) {
      std::visit([](auto&& chop){chop.run(ChopperPos::Reference);}, ctrl.chop);
    }
    ImGui::SameLine();
    ImGui::Text("Expected state: %s", toString(std::visit([](auto&& chop){return chop.get_data();}, ctrl.chop)).c_str());
  } else {
    ImGui::Text(" Cold ");
    ImGui::SameLine();
    ImGui::Text(" Hot ");
    ImGui::SameLine();
    ImGui::Text(" Antenna ");
    ImGui::SameLine();
    ImGui::Text(" Reference ");
  }

  if (std::visit([](auto&& chop){return chop.has_error();}, ctrl.chop)) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}
}  // namespace Instrument::Chopper
