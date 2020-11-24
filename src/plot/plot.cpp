#include <thread>

#include "plot.h"

#include <cli_parsing.h>
#include <cli_runtime.h>

#include <gui.h>
#include <gui_plotting.h>

namespace Plot {
AxisData read_raw(const std::string& file, int skiprows) {
  return File::parse_columndata(file, skiprows);
}
AxisData read_json(const std::string& file) {
  const File::json a = File::Json(file).read();
  std::vector<std::vector<double>> data(0);
  std::vector<std::string> keys(0);
  for(auto& x: a.items()) {
    if (x.value().is_array()) {
      keys.push_back(x.key());
      data.push_back(x.value().get<std::vector<double>>());
    }
  }
  return AxisData(std::move(keys), std::move(data));
}

void menubar(AxisData& data, std::string& x, std::string& y) {
  const int n=data.size();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Plot")) {
      ImGui::InputText("X label", &x);
      ImGui::InputText("Y label", &y);
      for (int i=0; i<n; i++) {
        if (ImGui::BeginMenu(data.line(i).c_str())) {
          std::string line = data.line(i);
          if (ImGui::InputText("Line label", &line, ImGuiInputTextFlags_EnterReturnsTrue)) {
            data.line(i) = line;
          }
          bool testx = data.isx(i);
          if (ImGui::Checkbox("Make X-grid", &testx)) data.setx(i);
          bool testy = data.isy(i);
          if (ImGui::Checkbox("Make Y-grid", &testy)) data.sety(i);
          ImGui::EndMenu();
        }
        ImGui::Separator();
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void plot(AxisData& data, std::string& x, std::string& y) {
  auto xaxis = data.xaxis();
  const int n = data.size();
  if (xaxis.first not_eq nullptr) {
    if (ImPlot::BeginPlot("PlotFrame", x.c_str(), y.c_str(), {-1, -1})) {
      for (int i=0; i<n; i++) {
        if (auto yaxis = data.yaxis(i); yaxis.first not_eq nullptr) {
          ImPlot::PlotLine(data.line(i).c_str(), xaxis.first, yaxis.first, std::min(xaxis.second, yaxis.second));
        }
      }
      ImPlot::EndPlot();
    }
  } else {
    if (ImPlot::BeginPlot("Plot Frame", x.c_str(), y.c_str(), {-1, -1})) {
      for (int i=0; i<n; i++) {
        if (auto yaxis = data.yaxis(i); yaxis.first not_eq nullptr) {
          ImPlot::PlotLine(data.line(i).c_str(), yaxis.first, yaxis.second);
        }
      }
      ImPlot::EndPlot();
    }
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
  CommandLine::App plot("Plot some data and explore it\n\nKeywords:\n"
    "\tRegular data: all data have the same length\n"
  );
  
  std::string filename;
  plot.NewRequiredOption("-f,--filename", filename, "File to load");
  
  bool printdata=false;
  plot.NewFlag("-p,--print", printdata, "Print the data to regular data columns without drawing");
  
  std::string xlabel="X";
  plot.NewDefaultOption("-x,--xlabel", xlabel, "X-label");
  
  std::string ylabel="Y";
  plot.NewDefaultOption("-y,--ylabel", ylabel, "Y-label");
  
  int option=1;
  plot.NewDefaultOption("-o,--option", option,
                        "The type of data:\n"
                        "\toption 0: Will guess the file-type (NOT IMPLEMENTED)\n"
                        "\toption 1: Pure ascii file of format [f0, f1, f2, ... fn] in regular data columns\n"
                        "\toption 2: JSON file (maybe regular data; will only parse array values)\n"
                       );
  
  int skiprows=0;
  plot.NewDefaultOption("-s,--skiprows", skiprows, "How many rows to skip in reading the file");
  
  // Parse input options
  plot.Parse(argc, argv);
  
  Plot::AxisData data;
  if (option == 1) {
    data = Plot::read_raw(filename, skiprows);
  } else if (option == 2) {
    data = Plot::read_json(filename);
  } else {
    std::ostringstream os;
    os << "Cannot understand the option: " << option << '\n';
    throw std::runtime_error(os.str());
  }
  
  // Deal with the data
  if (printdata) {
    if (not data.size()) {
      throw std::runtime_error("No data");
    } else if (not data.is_regular()) {
      throw std::runtime_error("Irregular data");
    } else {
      for (int j=0; j<data.datasize(0); j++) {
        for (int i=0; i<data.size(); i++) {
          if (i) {
            std::cout << ' ';
          }
          std::cout << data.get(i)[j];
        }
        std::cout << '\n';
      }
    }
  } else {
    // Draw the data
    draw(data, xlabel, ylabel);
  }
  
  // Finish
  return EXIT_SUCCESS;
} catch (std::exception& e) {
  std::cerr << "Error in execution:\n\n" << e.what() << "\n\nRun with --help for more information.\n\n";
  return EXIT_FAILURE;
}
