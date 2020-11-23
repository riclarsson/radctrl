#include <thread>

#include "plot.h"

#include <cli_parsing.h>
#include <cli_runtime.h>

#include <gui.h>
#include <gui_plotting.h>

namespace Plot {
AxisData read_raw(const std::string& file, std::size_t skiprows) {
  return File::parse_columndata(file, skiprows);
}

void menubar(AxisData& data, std::string& x, std::string& y) {
  if (ImGui::BeginMainMenuBar()) {
    ImGui::EndMainMenuBar();
  }
}

void plot(AxisData& data, std::string& x, std::string& y) {
  if (ImPlot::BeginPlot("PlotFrame", x.c_str(), y.c_str(), {-1, -1})) {
//     for (auto &line : frame)
//       ImPlot::PlotLine(line.name().c_str(), line.getter(), (void *)&line,
//                        line.size());
      ImPlot::EndPlot();
  }
}

void draw(AxisData& data, std::string& x, std::string& y) {
  
  // Start the window and give it a name
  InitializeGUI("Plot");
  
  // Our global states are stored in config
  GUI::Config config;
  
  // Our style
  GUI::LayoutAndStyleSettings();
  
  // Main loop
  BeginWhileLoopGUI;
  
  // Menubar to select which data is drawn
  GUI::MainMenu::fullscreen(config, window);
  GUI::MainMenu::quitscreen(config, window);
  menubar(data, x, y);
  
  // And plot the data
  if (GUI::Windows::full(window, ImGui::GetCursorPos(), "PlotWindow")) {
    plot(data, x, y);
    GUI::Windows::end();
  }
  
  // End of main loop
  EndWhileLoopGUI;
  
  CleanupGUI;
}
}

int main(int argc, char **argv) try {
  CommandLine::App plot("Plot some data");
  
  std::string filename;
  plot.NewRequiredOption("-f,--filename", filename, "File to load");
  
  std::string xlabel="X";
  plot.NewDefaultOption("-x,--xlabel", xlabel, "X-label");
  
  std::string ylabel="Y";
  plot.NewDefaultOption("-y,--ylabel", xlabel, "Y-label");
  
  int option=0;
  plot.NewDefaultOption("-o,--option", option,
                        "The type of data"
                        "\n\t0: Pure ascii file of format [x, y1, y2, ... yn] in columns");
  
  int skiprows=0;
  plot.NewDefaultOption("-s,--skiprows", skiprows,
                        "How many rows to skip in reading the file");
  
  // Parse input options
  plot.Parse(argc, argv);
  
  Plot::AxisData data;
  if (option == 0) {
    data = Plot::read_raw(filename, skiprows);
  } else {
    std::ostringstream os;
    os << "Cannot understand the option: " << option << '\n' << "See --help for valid options\n";
    throw std::runtime_error(os.str());
  }
  
  // Draw the data
  draw(data, xlabel, ylabel);
  
  // Finish
  return EXIT_SUCCESS;
} catch (std::exception& e) {
  std::cerr << "Error in execution:\n\n" << e.what() << '\n' << '\n';
  return EXIT_FAILURE;
}
