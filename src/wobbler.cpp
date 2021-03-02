#include "wobbler.h"

namespace Instrument::Wobbler {
Controller::Controller(Interface&& iface, const std::string &d, int b, char a) noexcept
: 
wob(iface),
init(false),
error(false),
quit(false),
run(false),
operating(false),
waiting(false),
dev(d),
baudrate(b),
address(a) {
  pos.fill(0);
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
      std::visit([&ctrl](auto&& wob) {wob.startup(ctrl.dev, ctrl.baudrate, ctrl.address);}, ctrl.wob);
      std::visit([&ctrl, manual](auto&& wob) {wob.init(ctrl.pos[0], manual);}, ctrl.wob);
    } else {
      std::visit([](auto&& wob) {wob.close();}, ctrl.wob);
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
  ImGui::PopItemWidth();

  ImGui::PushItemWidth(30.0f);
  ImGui::SameLine();
  if (ctrl.init) {
    ImGui::InputText("Addr", &ctrl.address, 2,
                     ImGuiInputTextFlags_CharsNoBlank |
                         ImGuiInputTextFlags_AutoSelectAll |
                         ImGuiInputTextFlags_NoHorizontalScroll |
                         ImGuiInputTextFlags_ReadOnly);
  } else {
    ImGui::InputText("Addr", &ctrl.address, 2,
                     ImGuiInputTextFlags_CharsNoBlank |
                         ImGuiInputTextFlags_AutoSelectAll |
                         ImGuiInputTextFlags_NoHorizontalScroll);
  }
  ImGui::PopItemWidth();
  
  ImGui::PushItemWidth(100.0f);
  ImGui::SameLine();
  if (ctrl.init) {
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,
                    ImGuiInputTextFlags_CharsDecimal |
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
  } else {
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,
                    ImGuiInputTextFlags_CharsDecimal |
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
  }
  ImGui::PopItemWidth();

  ImGui::PushItemWidth(400.f);
  ImGui::InputInt4("Positions", &ctrl.pos[0],
                   ImGuiInputTextFlags_CharsDecimal |
                   ImGuiInputTextFlags_CharsNoBlank |
                   ImGuiInputTextFlags_NoHorizontalScroll |
                   ImGuiInputTextFlags_AutoSelectAll);
  ImGui::PopItemWidth();

  ImGui::Text("Target Position: %i", 
              std::visit([](auto&& wob) {return wob.get_data();}, ctrl.wob));

  if (std::visit([](auto&& wob) {return wob.has_error();}, ctrl.wob)) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}
}  // namespace Instrument::Wobbler
