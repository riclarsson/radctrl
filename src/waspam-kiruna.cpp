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
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  // Start the window and give it a name
  InitializeGUI("WASPAM");

  // Our global states are stored in config
  GUI::Config config;

  // Chopper declaration
  Instrument::Chopper::Controller chopper_ctrl = Instrument::Chopper::parse(parser);

  // Wobbler declaration
  Instrument::Wobbler::Controller wobbler_ctrl = Instrument::Wobbler::parse(parser);

  // Housekeeping declaration
  Instrument::Housekeeping::Controller housekeeping_ctrl = Instrument::Housekeeping::parse(parser);

  // Frontend declaration
  Instrument::Frontend::Controller frontend_ctrl = Instrument::Frontend::parse(parser);
  
  // Backend declarations
  Instrument::Spectrometer::Controllers spectrometer_ctrls = Instrument::Spectrometer::parse(parser);
  std::vector<Instrument::Data> backend_data = Instrument::init_spectrometer_data(spectrometer_ctrls);
  std::vector<GUI::Plotting::CAHA76> backend_frames = Instrument::init_plotting_data(spectrometer_ctrls);
  const std::size_t N = spectrometer_ctrls.size();
  
  // Save system
  std::filesystem::path save_path{parser("Savepath", "path")};
  Instrument::DataSaver datasaver(save_path, "WASPAM");
  
  // Imgui saver
  auto directoryBrowser = ImGui::FileBrowser(
    ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc |
    ImGuiFileBrowserFlags_CreateNewDir);
  directoryBrowser.SetTitle("Select Directory");
  directoryBrowser.SetPwd(save_path);
  directoryBrowser.SetTypeFilters({"[D]"});
  
  // Housekeeping data for long-term view
  GUI::Plotting::ListOfLines76 hk_frames(100'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  auto runner = AsyncRef(
      &Instrument::RunExperiment,
      chopper_ctrl, wobbler_ctrl, housekeeping_ctrl,
      frontend_ctrl, spectrometer_ctrls);

  // Start interchange between output data and operations on yet another thread
  auto saver = AsyncRef(
      &Instrument::ExchangeData,
      spectrometer_ctrls, chopper_ctrl, housekeeping_ctrl, frontend_ctrl,
      backend_data, datasaver, backend_frames, hk_frames);

  // Setup of the tabs
  for (auto& spec: spectrometer_ctrls.backends) {
    config.tabs.push_back(spec.name);
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
  for (size_t i = 0; i < N; i++)
    if (current_tab == i) backend_frames[i].plot(window, startpos);

  // Draw the combined backends
  if (current_tab == N)
    GUI::Plotting::caha_plot_combined(window, startpos, backend_frames);

  // Control tool
  if (GUI::Windows::sub<5, 7, 0, 6, 2, 1>(window, startpos, "CTRL Tool 1")) {
    Instrument::AllControl(config, save_path, directoryBrowser, datasaver,
                           chopper_ctrl, wobbler_ctrl,
                           housekeeping_ctrl, frontend_ctrl, spectrometer_ctrls);
  }
  GUI::Windows::end();

  // Information tool
  if (GUI::Windows::sub<5, 7, 2, 6, 3, 1>(window, startpos, "DATA Tool 1")) {
    Instrument::AllInformation(chopper_ctrl, wobbler_ctrl,
                               housekeeping_ctrl, frontend_ctrl,
                               spectrometer_ctrls, backend_data);
  }
  GUI::Windows::end();

  // Error handling
  if (not Instrument::AllErrors(config, chopper_ctrl, wobbler_ctrl,
                                housekeeping_ctrl, frontend_ctrl,
                                spectrometer_ctrls)) {
    glfwSetWindowShouldClose(window, 1);
  }

  // End of main loop
  EndWhileLoopGUI;

  Instrument::QuitAll(chopper_ctrl, wobbler_ctrl, housekeeping_ctrl,
                      frontend_ctrl, spectrometer_ctrls);

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

int run_no_gui(File::ConfigParser parser) try {
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  // Chopper declaration
  Instrument::Chopper::Controller chopper_ctrl = Instrument::Chopper::parse(parser);

  // Wobbler declaration
  Instrument::Wobbler::Controller wobbler_ctrl = Instrument::Wobbler::parse(parser);

  // Housekeeping declaration
  Instrument::Housekeeping::Controller housekeeping_ctrl = Instrument::Housekeeping::parse(parser);

  // Frontend declaration
  Instrument::Frontend::Controller frontend_ctrl = Instrument::Frontend::parse(parser);
  
  // Backend declarations
  Instrument::Spectrometer::Controllers spectrometer_ctrls = Instrument::Spectrometer::parse(parser);
  std::vector<Instrument::Data> backend_data = Instrument::init_spectrometer_data(spectrometer_ctrls);
  std::vector<GUI::Plotting::CAHA76> backend_frames = Instrument::init_plotting_data(spectrometer_ctrls);
  
  // Save system
  std::filesystem::path save_path{parser("Savepath", "path")};
  Instrument::DataSaver datasaver(save_path, "WASPAM");
  
  // Housekeeping data for long-term view
  GUI::Plotting::ListOfLines76 hk_frames(1'000'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  auto runner = AsyncRef(
      &Instrument::RunExperiment,
      chopper_ctrl, wobbler_ctrl, housekeeping_ctrl,
      frontend_ctrl, spectrometer_ctrls);

  // Start interchange between output data and operations on yet another thread
  auto saver = AsyncRef(
      &Instrument::ExchangeData,
      spectrometer_ctrls, chopper_ctrl, housekeeping_ctrl, frontend_ctrl,
      backend_data, datasaver, backend_frames, hk_frames);
  
  Instrument::InitAll(chopper_ctrl, wobbler_ctrl,
                      housekeeping_ctrl, frontend_ctrl,
                      spectrometer_ctrls);
  
  Instrument::ReadyRunAll(chopper_ctrl, wobbler_ctrl,
                          housekeeping_ctrl, frontend_ctrl,
                          spectrometer_ctrls);
  
  Sleep(30);
  
  auto running_errors = runner.get();
  if (running_errors.size()) {
    std::cerr << "Errors while stopping runner:\n:";
    for (auto &e : running_errors) std::cerr << e << '\n';
    return 1;
  }
  

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
  
  bool no_gui = false;
  rad.NewFlag("--nogui", no_gui, "Turn off the GUI and automatically initialize and run the measurements from XML");

  rad.Parse(argc, argv);

  if (not no_gui) {
    run(File::ConfigParser(xmlfilename, 
                          {"Chopper", "Wobbler", "Housekeeping", "Frontend",
                            "Backends", "Operations", "Savepath"}));
  } else {
    run_no_gui(File::ConfigParser(xmlfilename, 
                           {"Chopper", "Wobbler", "Housekeeping", "Frontend",
                             "Backends", "Operations", "Savepath"}));
  }
}
