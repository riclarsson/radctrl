#include "backend.h"
#include "chopper.h"
#include "cli_parsing.h"
#include "file.h"
#include "frontend.h"
#include "gui.h"
#include "housekeeping.h"
#include "instrument.h"
#include "multithread.h"
#include "python_interface.h"
#include "wobbler.h"

int run() try {
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  // Start the window and give it a name
  InitializeGUI("Dummy GUI for Testing");

  // Our global states are stored in a config
  GUI::Config config;

  // Chopper declaration
  Instrument::Chopper::Controller
      chopper_ctrl{
        Instrument::Chopper::Dummy("filename?"),
        "/dev/chopper", 1000, 0.0,
        Instrument::Chopper::ChopperPos::Cold,
        Instrument::Chopper::ChopperPos::Antenna,
        Instrument::Chopper::ChopperPos::Hot,
        Instrument::Chopper::ChopperPos::Antenna};

  // Wobbler declaration
  Instrument::Wobbler::Controller wobbler_ctrl{Instrument::Wobbler::Dummy("filename?"), "/dev/wobbler", 115200, '0'};
  wobbler_ctrl.pos = {3000, 7000, 3000, 7000};

  // Housekeeping declaration
  Instrument::Housekeeping::Controller housekeeping_ctrl{Instrument::Housekeeping::Dummy("filename?"), "some-device", 12345};

  // Frontend declaration
  Instrument::Frontend::Controller frontend_ctrl{Instrument::Frontend::Dummy("filename?"), "some-server", 1234};
  
  Instrument::Spectrometer::Controllers spectrometer_ctrls{
    std::vector<Instrument::Spectrometer::SingleController>{
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::Dummy(" Dummy1 "),
          "Dummy Data 1", "Dummy3", 0, 0,
          (Eigen::MatrixXd(2, 2) << 0, 1e9, 0, 100e9).finished(),
          (Eigen::VectorXi(2) << 1000, 1000).finished(), 100, 1, false),
      Instrument::Spectrometer::SingleController(
        Instrument::Spectrometer::Dummy(" Dummy2 "),
          "Dummy Data 2", "Dummy4", 0, 0,
          (Eigen::MatrixXd(2, 2) << 0, 1e9, 0, 100e9).finished(),
          (Eigen::VectorXi(2) << 1000, 1000).finished(), 100, 1, false)
    }
  };
  const std::size_t N = spectrometer_ctrls.size();
  std::vector<Instrument::Data> backend_data(spectrometer_ctrls.backends.size());
  std::vector<GUI::Plotting::CAHA76> backend_frames{
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[0].name, spectrometer_ctrls.backends[0].f},
    GUI::Plotting::CAHA76{spectrometer_ctrls.backends[1].name, spectrometer_ctrls.backends[1].f}};

  // Files chooser
  auto directoryBrowser = ImGui::FileBrowser(
      ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc |
      ImGuiFileBrowserFlags_CreateNewDir);
  directoryBrowser.SetTitle("Select Directory");
  std::filesystem::path save_path{"pathname?"};
  directoryBrowser.SetPwd(save_path);
  directoryBrowser.SetTypeFilters({"[D]"});
  
  // Housekeeping data for long-term view
  GUI::Plotting::ListOfLines76 hk_frames(100'000, "Housekeeping", "Time");

  // Start the operation of the instrument on a different thread
  Instrument::DataSaver datasaver(save_path, "IRAM");
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
  const std::size_t ret_tab = N + 1;
  config.tabs.push_back(" Housekeeping ");
  const std::size_t house_tab = N + 2;
  config.tabspos = 0;

  // Helpers
  const std::vector<std::string> devices =
      File::Devices({"USB", "S", "chopper", "wobbler", "sensors", "ACM"}, 100);

  // Our style
  GUI::LayoutAndStyleSettings();

  // Main loop
  BeginWhileLoopGUI;

  // Main menu bar
  GUI::MainMenu::fullscreen(config, window);
  GUI::MainMenu::quitscreen(config, window);
  GUI::Plotting::caha_mainmenu(backend_frames);
  hk_frames.mainmenu();
  const size_t current_tab = GUI::MainMenu::tabselect(config);

  // Drawer helper
  const auto startpos = ImGui::GetCursorPos();

  // Draw the individual tabs
  for (size_t i = 0; i < N; i++)
    if (current_tab == i) backend_frames[i].plot(window, startpos);

  // Draw the combined backends
  if (current_tab == N) {
    GUI::Plotting::caha_plot_combined(window, startpos, backend_frames);
  } else if (current_tab == ret_tab) {
    // plot retrieval
  } else if (current_tab == house_tab) {
    hk_frames.plot(window, startpos);
  }

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

int main(int argc, char **argv) {
  CommandLine::App rad("Run the Dummy GUI");
  
  rad.Parse(argc, argv);
  
  run();
}
