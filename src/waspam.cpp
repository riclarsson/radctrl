#include "backend.h"
#include "chopper.h"
#include "cli_parsing.h"
#include "frontend.h"
#include "gui.h"
#include "housekeeping.h"
#include "instrument.h"
#include "multithread.h"
#include "python_interface.h"
#include "wobbler.h"
#include "xml_config.h"

int run(File::ConfigParser parser) try {
  constexpr size_t height_of_window = 7;  // Any size larger than part_for_plot
  constexpr size_t part_for_plot = 6;     // multiple of 2 and 3
  static_assert(part_for_plot % 6 == 0, "part_of_plot must be a multiple of 6");

  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  // Start the window and give it a name
  InitializeGUI("WASPAM");

  // Our global states are stored in config
  GUI::Config config;

  // Chopper declaration
  Instrument::Chopper::PythonOriginal chop{parser("Chopper", "path")};
  Instrument::Chopper::Controller<Instrument::Chopper::ChopperPos::Cold,
                                  Instrument::Chopper::ChopperPos::Antenna,
                                  Instrument::Chopper::ChopperPos::Hot,
                                  Instrument::Chopper::ChopperPos::Antenna>
      chopper_ctrl{parser("Chopper", "dev"),
                   std::stoi(parser("Chopper", "offset")),
                   std::stod(parser("Chopper", "sleeptime"))};

  // Wobbler declaration
  Instrument::Wobbler::Controller wobbler_ctrl{
      Instrument::Wobbler::PythonOriginalWASPAM(parser("Wobbler", "path")),
      parser("Wobbler", "dev"), std::stoi(parser("Wobbler", "baudrate")),
      parser("Wobbler", "address")[0]};
  wobbler_ctrl.pos = {std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end")),
                      std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end"))};

  // Housekeeping declaration
  Instrument::Housekeeping::Agilent34970A hk{parser("Housekeeping", "path")};
  Instrument::Housekeeping::Controller housekeeping_ctrl{
      parser("Housekeeping", "dev"),
      std::stoi(parser("Housekeeping", "baudrate"))};

  // Frontend declaration
  Instrument::Frontend::Waspam frontend{parser("Frontend", "path")};
  Instrument::Frontend::Controller frontend_ctrl{
      parser("Frontend", "server"), std::stoi(parser("Frontend", "port"))};

  // Spectrometers declarations
  int integration_time_microseconds =
      std::stoi(parser("Operations", "integration_time"));
  int blank_time_microseconds = std::stoi(parser("Operations", "blank_time"));

  Instrument::Spectrometer::Backends backends{Instrument::Spectrometer::XFFTS(
      parser("Backends", "spectormeter1"), parser("Backends", "path1"))};
  std::array<Instrument::Spectrometer::Controller, backends.N> backend_ctrls{
      Instrument::Spectrometer::Controller(
          parser("Backends", "spectormeter1"), parser("Backends", "config1"),
          integration_time_microseconds, blank_time_microseconds)};
  std::array<Instrument::Data, backends.N> backend_data;
  std::array<GUI::Plotting::CAHA<height_of_window, part_for_plot>, backends.N>
      backend_frames{GUI::Plotting::CAHA<height_of_window, part_for_plot>{
          backend_ctrls[0].name, backend_ctrls[0].f}};
  if (std::stoi(parser("Backends", "size")) not_eq backends.N)
    throw std::runtime_error("Bad backend count");

  // Files chooser
  auto directoryBrowser = ImGui::FileBrowser(
      ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc |
      ImGuiFileBrowserFlags_CreateNewDir);
  directoryBrowser.SetTitle("Select Directory");
  std::filesystem::path save_path{parser("Savepath", "path")};
  directoryBrowser.SetPwd(save_path);
  directoryBrowser.SetTypeFilters({"[D]"});
  
  // Housekeeping data for long-term view
  GUI::Plotting::ListOfLines<height_of_window, part_for_plot> hk_frames(1'000'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  Instrument::DataSaver datasaver(save_path, "WASPAM");
  auto runner = AsyncRef(
      &Instrument::RunExperiment<decltype(chop), decltype(chopper_ctrl),
                                 decltype(wobbler_ctrl),
                                 decltype(hk), decltype(housekeeping_ctrl),
                                 decltype(frontend), decltype(frontend_ctrl),
                                 decltype(backends), decltype(backend_ctrls)>,
      chop, chopper_ctrl, wobbler_ctrl, hk, housekeeping_ctrl, frontend,
      frontend_ctrl, backends, backend_ctrls);

  // Start interchange between output data and operations on yet another thread
  auto saver = AsyncRef(
      &Instrument::ExchangeData<
          backends.N, decltype(chopper_ctrl), decltype(housekeeping_ctrl),
          decltype(frontend_ctrl), height_of_window, part_for_plot>,
      backend_ctrls, chopper_ctrl, housekeeping_ctrl, frontend_ctrl,
      backend_data, datasaver, backend_frames, hk_frames);

  // Setup of the tabs
  for (size_t i = 0; i < backends.N; i++) {
    config.tabs.push_back(backends.name(i));
  }
  config.tabs.push_back(" All ");
  config.tabs.push_back(" Retrieval ");
  config.tabspos = 0;

  // Our style
  GUI::LayoutAndStyleSettings();

  // Main loop
  BeginWhileLoopGUI;

  // Main menu bar
  GUI::MainMenu::fullscreen(config, window);
  GUI::MainMenu::quitscreen(config, window);
  GUI::Plotting::caha_mainmenu(backend_frames);
  const size_t current_tab = GUI::MainMenu::tabselect(config);

  // Drawer helper
  const auto startpos = ImGui::GetCursorPos();

  // Draw the individual tabs
  for (size_t i = 0; i < backends.N; i++)
    if (current_tab == i) backend_frames[i].plot(window, startpos);

  // Draw the combined backends
  if (current_tab == backends.N)
    GUI::Plotting::caha_plot_combined(window, startpos, backend_frames);

  // Control tool
  if (GUI::Windows::sub<5, height_of_window, 0, part_for_plot, 2,
                        height_of_window - part_for_plot>(window, startpos,
                                                          "CTRL Tool 1")) {
    Instrument::AllControl(config, save_path, directoryBrowser, datasaver, chop,
                           chopper_ctrl, wobbler_ctrl, hk,
                           housekeeping_ctrl, frontend, frontend_ctrl, backends,
                           backend_ctrls);
  }
  GUI::Windows::end();

  // Information tool
  if (GUI::Windows::sub<5, height_of_window, 2, part_for_plot, 3,
                        height_of_window - part_for_plot>(window, startpos,
                                                          "DATA Tool 1")) {
    Instrument::AllInformation(chop, chopper_ctrl, wobbler_ctrl, hk,
                               housekeeping_ctrl, frontend, frontend_ctrl,
                               backends, backend_ctrls, backend_data);
  }
  GUI::Windows::end();

  // Error handling
  if (not Instrument::AllErrors(config, chop, chopper_ctrl, wobbler_ctrl,
                                hk, housekeeping_ctrl, frontend, frontend_ctrl,
                                backends, backend_ctrls)) {
    glfwSetWindowShouldClose(window, 1);
  }

  // End of main loop
  EndWhileLoopGUI;

  Instrument::QuitAll(chopper_ctrl, wobbler_ctrl, housekeeping_ctrl,
                      frontend_ctrl, backend_ctrls);

  auto running_errors = runner.get();
  if (running_errors.size()) {
    std::cerr << "Errors while stopping runner:\n:";
    for (auto &e : running_errors) std::cerr << e << '\n';
    return 1;
  }

  CleanupGUI;

  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  std::ostringstream os;
  os << "Terminated with errors:\n" << e.what() << '\n';
  std::cerr << os.str();
  return EXIT_FAILURE;
}

int main(int argc, char **argv) {
  CommandLine::App rad("Run the WASPAM Radiometer");

  std::string xmlfilename;
  rad.NewRequiredOption("--xml", xmlfilename,
                        "Configuration file for the Radiometer");

  rad.Parse(argc, argv);

  run(File::ConfigParser(xmlfilename,
                         {"Chopper", "Wobbler", "Housekeeping", "Frontend",
                          "Backends", "Operations", "Savepath"}));
}
