#include <array>
#include <vector>

#include "chopper.h"
#include "file.h"
#include "gui.h"
#include "instrument.h"
#include "python_interface.h"

template <class Wobbler,
          class Chopper,
          class Housekeeping,
          class Frontend,
          class Backends>
class IRAM {
  static constexpr unsigned BackendN = Backends::N;
  static_assert(Housekeeping::is_housekeeper);
  static_assert(Chopper::is_chopping);
  static_assert(Wobbler::is_wobbling);
  static_assert(Frontend::is_frontend);
  static_assert(Backends::is_backend);
  
  Housekeeping hk;
  Chopper chop;
  Wobbler wob;
  Frontend fr;
  Backends ba;

public:
  IRAM (Housekeeping&& h, Chopper&& c, Wobbler&& w, Frontend&& f, Backends&& b) noexcept :
  hk(std::move(h)), chop(std::move(c)), wob(std::move(w)), fr(std::move(f)), ba(std::move(ba)) {}
};


int main () {
  auto py = Python::createPython();  // Must be declared even if unused

  InitializeGUI("IRAM");
  
  // Our states
  GUI::Config config;
  
  // Setup TESTS
  config.tabs = {"CTS", "XFFTS", "Retrieval"};
  config.tabspos = 0;
  Instrument::Chopper::GUI chopper_ctrl;
  Instrument::Chopper::Dummy chop{"~/Work/radcalc/python/chopper/chopper.py"};
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
    Instrument::Chopper::GuiSetup(chop, chopper_ctrl, devices);
  } GUI::Windows::end();
  if (GUI::Windows::sub<3, 7, 1, 6, 2, 1>(window, startpos, "CTRL Tool 2")) {
    ImGui::Button("HOWDY");
  } GUI::Windows::end();
  
  // End of main loop
  EndWhileLoopGUI;
  CleanupGUI;
}
