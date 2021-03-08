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
  Instrument::Wobbler::Controller wobbler_ctrl{
      Instrument::Wobbler::PythonOriginalWASPAM(parser("Wobbler", "path")),
      parser("Wobbler", "dev"), std::stoi(parser("Wobbler", "baudrate")),
      parser("Wobbler", "address")[0]};
  wobbler_ctrl.pos = {std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end")),
                      std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end"))};

  // Housekeeping declaration
  Instrument::Housekeeping::Controller housekeeping_ctrl{
      Instrument::Housekeeping::Agilent34970A(parser("Housekeeping", "path")),
      parser("Housekeeping", "dev"),
      std::stoi(parser("Housekeeping", "baudrate"))};

  // Frontend declaration
  Instrument::Frontend::Controller frontend_ctrl{
      Instrument::Frontend::Waspam(parser("Frontend", "path")),
      parser("Frontend", "server"), std::stoi(parser("Frontend", "port"))};

  // Spectrometers declarations
  int integration_time_microseconds =
      std::stoi(parser("Operations", "integration_time"));
  int blank_time_microseconds = std::stoi(parser("Operations", "blank_time"));

  constexpr std::size_t N = 2;
  
  Instrument::Spectrometer::Controllers spectrometer_ctrls{
    std::vector<Instrument::Spectrometer::SingleController>{
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::RCTS104(parser("Backends", "spectormeter1"), parser("Backends", "path1")),
        parser("Backends", "spectormeter1"), parser("Backends", "config1"),
        integration_time_microseconds, blank_time_microseconds),
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::RCTS104(parser("Backends", "spectormeter2"), parser("Backends", "path2")),
        parser("Backends", "spectormeter2"), parser("Backends", "config2"),
        integration_time_microseconds, blank_time_microseconds)
    }
  };
  std::vector<Instrument::Data> backend_data(spectrometer_ctrls.backends.size());
  std::vector<GUI::Plotting::CAHA76> backend_frames{
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[0].name, spectrometer_ctrls.backends[0].f},
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[1].name, spectrometer_ctrls.backends[1].f}};
  if (std::stoi(parser("Backends", "size")) not_eq N)
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
  GUI::Plotting::ListOfLines76 hk_frames(100'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  Instrument::DataSaver datasaver(save_path, "WASPAM");
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
  Instrument::Wobbler::Controller wobbler_ctrl{
      Instrument::Wobbler::PythonOriginalWASPAM(parser("Wobbler", "path")),
      parser("Wobbler", "dev"), std::stoi(parser("Wobbler", "baudrate")),
      parser("Wobbler", "address")[0]};
  wobbler_ctrl.pos = {std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end")),
                      std::stoi(parser("Wobbler", "start")),
                      std::stoi(parser("Wobbler", "end"))};

  // Housekeeping declaration
  Instrument::Housekeeping::Controller housekeeping_ctrl{
      Instrument::Housekeeping::Agilent34970A(parser("Housekeeping", "path")),
      parser("Housekeeping", "dev"),
      std::stoi(parser("Housekeeping", "baudrate"))};

  // Frontend declaration
  Instrument::Frontend::Controller frontend_ctrl{
      Instrument::Frontend::Waspam(parser("Frontend", "path")),
      parser("Frontend", "server"), std::stoi(parser("Frontend", "port"))};

  // Spectrometers declarations
  int integration_time_microseconds =
      std::stoi(parser("Operations", "integration_time"));
  int blank_time_microseconds = std::stoi(parser("Operations", "blank_time"));
  
  Instrument::Spectrometer::Controllers spectrometer_ctrls{
    std::vector<Instrument::Spectrometer::SingleController>{
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::RCTS104(parser("Backends", "spectormeter1"), parser("Backends", "path1")),
        parser("Backends", "spectormeter1"), parser("Backends", "config1"),
        integration_time_microseconds, blank_time_microseconds),
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::RCTS104(parser("Backends", "spectormeter2"), parser("Backends", "path2")),
        parser("Backends", "spectormeter2"), parser("Backends", "config2"),
        integration_time_microseconds, blank_time_microseconds)
    }
  };
  const std::size_t N = spectrometer_ctrls.size();
  std::vector<Instrument::Data> backend_data(spectrometer_ctrls.backends.size());
  std::vector<GUI::Plotting::CAHA76> backend_frames{
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[0].name, spectrometer_ctrls.backends[0].f},
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[1].name, spectrometer_ctrls.backends[1].f}};
  if (size_t(std::stoi(parser("Backends", "size"))) not_eq N)
    throw std::runtime_error("Bad backend count");

  // Files chooser
  std::filesystem::path save_path{parser("Savepath", "path")};
  
  // Housekeeping data for long-term view
  GUI::Plotting::ListOfLines76 hk_frames(1'000'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  Instrument::DataSaver datasaver(save_path, "WASPAM");
  
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
