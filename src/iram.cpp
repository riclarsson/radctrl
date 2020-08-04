#include <array>
#include <vector>

#include "chopper.h"
#include "file.h"
#include "gui.h"
#include "python_interface.h"
#include "wobbler.h"

int main () {
  auto py = Python::createPython();  // Must be declared even if unused

  InitializeGUI("IRAM");
  
  // Our states
  GUI::Config config;
  
  // Setup TESTS
  config.tabs = {" CTS ", " XFFTS ", " Retrieval "};
  config.tabspos = 0;
  Instrument::Chopper::GUI chopper_ctrl;
  Instrument::Chopper::Dummy chop{"/home/larsson/Work/radctrl/python/chopper/chopper.py"};
  Instrument::Wobbler::GUI wobbler_ctrl;
  Instrument::Wobbler::Dummy wob{"/home/larsson/Work/radctrl/python/wobbler/IRAM.py"};
  const std::vector<std::string> devices = File::Devices({"USB", "S", "chopper", "wobbler"});
  
  config.io.FontGlobalScale=1.0f;
  
  // Our style
  GUI::LayoutAndStyleSettings();
  
  // Main loop
  BeginWhileLoopGUI;
  
  // Main menu bar
  GUI::MainMenu::fullscreen(config, window);
  GUI::MainMenu::quitscreen(config, window);
  const size_t current_tab = GUI::MainMenu::tabselect(config);
  
  auto startpos = ImGui::GetCursorPos();
  if (current_tab == 0) {
    std::vector<double> y {1,2,3,4,5,6,7,6,5,4,3,2,1};
    if (GUI::Windows::sub<2, 7, 0, 0, 1, 3>(window, startpos, "Plot tool 1")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 1, 0, 1, 3>(window, startpos, "Plot tool 2")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 0, 3, 1, 3>(window, startpos, "Plot tool 3")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 1, 3, 1, 3>(window, startpos, "Plot tool 4")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
  } else if (current_tab == 1) {
    std::vector<double> y {5,2,3,4,5,6,7,6,5,4,3,2,5};
    if (GUI::Windows::sub<2, 7, 0, 0, 1, 3>(window, startpos, "Plot tool 5")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 1, 0, 1, 3>(window, startpos, "Plot tool 6")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 0, 3, 1, 3>(window, startpos, "Plot tool 7")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, 7, 1, 3, 1, 3>(window, startpos, "Plot tool 8")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", y.data(), y.size());
        ImPlot::EndPlot();
      }
    } GUI::Windows::end();
  }
  
  if (GUI::Windows::sub<3, 7, 0, 6, 1, 1>(window, startpos, "CTRL Tool 1")) {
    if (ImGui::BeginTabBar("GUI Control")) {
      if (ImGui::BeginTabItem(" Chopper ")) {
        Instrument::Chopper::GuiSetup(chop, chopper_ctrl, devices);
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Wobbler ")) {
        Instrument::Wobbler::GuiSetup(wob, wobbler_ctrl, devices);
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  } GUI::Windows::end();
  if (GUI::Windows::sub<3, 7, 1, 6, 2, 1>(window, startpos, "CTRL Tool 2")) {
    ImGui::Button("HOWDY");
  } GUI::Windows::end();
  
  // Error handling
  if (config.active_errors == 0) {
    if (chopper_ctrl.error) {
      ImGui::OpenPopup("Error");
      chopper_ctrl.error = false;
      config.active_errors++;
    }
    if (wobbler_ctrl.error) {
      ImGui::OpenPopup("Error");
      wobbler_ctrl.error = false;
      config.active_errors++;
    }
  }
  
  // Error popup
  if (ImGui::BeginPopupModal("Error")) {
    ImGui::Text("Found %i error(s). These are cleaned up by pressing OK, but they must be fixed\t", config.active_errors);
    
    ImGui::TextWrapped("Chopper: %s", chop.error_string().c_str());
    ImGui::TextWrapped("Wobbler: %s", wob.error_string().c_str());
    
    ImGui::NewLine();
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      chop.delete_error();
      wob.delete_error();
      ImGui::CloseCurrentPopup();
      config.active_errors=0;
    } ImGui::SameLine();
    if (ImGui::Button(" Quit ", {80.0f, 30.0f})) {
      glfwSetWindowShouldClose(window, 1);
    }
    ImGui::EndPopup();
  }
  
  // End of main loop
  EndWhileLoopGUI;
  CleanupGUI;
}
