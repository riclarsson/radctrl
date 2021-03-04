#include "frontend.h"

namespace Instrument::Frontend {
void GuiSetup(Controller &ctrl) {
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
      std::visit([&ctrl](auto && frontend){frontend.startup(ctrl.server, ctrl.port);}, ctrl.frontend);
      std::visit([manual](auto && frontend){frontend.init(manual);}, ctrl.frontend);
    } else {
      std::visit([](auto && frontend){frontend.close();}, ctrl.frontend);
    }
  }

  // Each frontend will be very different so the setup must be dealt with
  // individually
  std::visit([&ctrl](auto && frontend){
    if constexpr (frontend.has_config) {
      ImGui::PushItemWidth(180.0f);
      if (ctrl.init) {
        ImGui::InputText("Server", &ctrl.server,
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
      } else {
        ImGui::InputText("Server", &ctrl.server,
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
      }
      ImGui::PopItemWidth();
      ImGui::SameLine();
      ImGui::PushItemWidth(120.0f);
      if (ctrl.init) {
        ImGui::InputInt("port", &ctrl.port, 1, 100,
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
      } else {
        ImGui::InputInt("port", &ctrl.port, 1, 100,
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
      }
      ImGui::PopItemWidth();
      
      if (ctrl.init and frontend.manual_run()) {
        if (ImGui::Button(" Run ")) {
          frontend.run();
          frontend.get_data();
          ctrl.data = frontend.data();
        }
      } else {
        ImGui::Text(" Run ");
      }
    }
  }, ctrl.frontend);

  if (std::visit([](auto && frontend){return frontend.has_error();}, ctrl.frontend)) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}
}  // namespace Instrument::Frontend
