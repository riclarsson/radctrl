#include <vector>

#include "gui.h"

void test001() {
  
  InitializeGUI("Test");
  
  // Our states
  GUI::Config config;
  
  // Our style
  GUI::LayoutAndStyleSettings();
  
  // Main loop
  BeginWhileLoopGUI;
  
  // Main menu bar
  GUI::MainMenu::fullscreen(config, window);
  GUI::MainMenu::quitscreen(config, window);
  
  std::vector<double> y {1,2,3,4,5,6,7,6,5,4,3,2,1};
  auto startpos = ImGui::GetCursorPos();
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
  if (GUI::Windows::sub<3, 7, 0, 6, 1, 1>(window, startpos, "Plot tool 5")) {
    ImGui::Button("HIIIII");
  } GUI::Windows::end();
  if (GUI::Windows::sub<3, 7, 1, 6, 2, 1>(window, startpos, "Plot tool 6")) {
    ImGui::Button("HOWDY");
  } GUI::Windows::end();
  
  // End of main loop
  EndWhileLoopGUI;
  CleanupGUI;
}

int main() {
  test001();
}
