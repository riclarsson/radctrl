#include "backend.h"
#include "chopper.h"
#include "frontend.h"
#include "gui.h"
#include "housekeeping.h"
#include "instrument.h"
#include "multithread.h"
#include "python_interface.h"
#include "wobbler.h"
#include "xml_config.h"

int main (int argc, char * argv[]) try {
  constexpr size_t height_of_window=7;   // Any size larger than part_for_plot
  constexpr size_t part_for_plot=6; // multiple of 2 and 3
  static_assert(part_for_plot % 6 == 0, "part_of_plot must be a multiple of 6");
  
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  // Start the window and give it a name
  InitializeGUI("IRAM");
  
  // Our global states are stored in configs
  if (argc not_eq 2) throw std::runtime_error("Bad parguments, please include the XML file");
  File::ConfigParser parser(argv[1], {"Chopper", "Wobbler", "Housekeeping", "Frontend", "Backends", "Savepath"});
  std::cout<<parser<<"\n";
  GUI::Config config;
  
  // Chopper declaration
  Instrument::Chopper::PythonOriginal chop{parser("Chopper", "path")};
  Instrument::Chopper::Controller<Instrument::Chopper::ChopperPos::Cold,
                                  Instrument::Chopper::ChopperPos::Antenna,
                                  Instrument::Chopper::ChopperPos::Hot,
                                  Instrument::Chopper::ChopperPos::Antenna> chopper_ctrl{parser("Chopper", "dev"),
                                      std::stoi(parser("Chopper", "offset")), std::stod(parser("Chopper", "sleeptime"))};
  
  // Wobbler declaration
  Instrument::Wobbler::PythonOriginalIRAM wob{parser("Wobbler", "path")};
  Instrument::Wobbler::Controller<4> wobbler_ctrl{parser("Wobbler", "dev"), std::stoi(parser("Wobbler", "baudrate")), parser("Wobbler", "address")[0]};
  wobbler_ctrl.pos = {std::stoi(parser("Wobbler", "start")), std::stoi(parser("Wobbler", "end")),
                      std::stoi(parser("Wobbler", "start")), std::stoi(parser("Wobbler", "end"))};
  
  // Housekeeping declaration
  Instrument::Housekeeping::Dummy hk{parser("Housekeeping", "path")};
  Instrument::Housekeeping::Controller housekeeping_ctrl;
  
  // Frontend declaration
  Instrument::Frontend::Dummy frontend{parser("Frontend", "path")};
  Instrument::Frontend::Controller frontend_ctrl{parser("Frontend", "server"), std::stoi(parser("Frontend", "port"))};
  
  // Spectrometers declarations
  Instrument::Spectrometer::Backends backends{
    Instrument::Spectrometer::Dummy(" Dummy1 "),
    Instrument::Spectrometer::Dummy(" Dummy2 ")
  };
  std::array<Instrument::Spectrometer::Controller, backends.N> backend_ctrls{
    Instrument::Spectrometer::Controller("Dummy Data 1", "Dummy3", 0, 0,
                                  (Eigen::MatrixXd(2, 2) << 0, 1e9, 0, 100e9).finished(),
                                  (Eigen::VectorXi(2) << 1000, 1000).finished(),
                                  100, 1, false),
    Instrument::Spectrometer::Controller("Dummy Data 2", "Dummy4", 0, 0,
                                  (Eigen::MatrixXd(2, 2) << 0, 1e9, 0, 100e9).finished(),
                                  (Eigen::VectorXi(2) << 1000, 1000).finished(),
                                  100, 1, false)
  };
  std::array<Instrument::Data, backends.N> backend_data;
  std::array<GUI::Plotting::CAHA<height_of_window, part_for_plot>, backends.N> backend_frames{
    GUI::Plotting::CAHA<height_of_window, part_for_plot>{backend_ctrls[0].name, backend_ctrls[0].f},
    GUI::Plotting::CAHA<height_of_window, part_for_plot>{backend_ctrls[1].name, backend_ctrls[1].f}
  };
  
  // Files chooser
  auto directoryBrowser = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_CreateNewDir);
  directoryBrowser.SetTitle("Select Directory");
  std::filesystem::path save_path{"/home/larsson/data/"};
  directoryBrowser.SetPwd(save_path);
  directoryBrowser.SetTypeFilters({"[D]"});
  
  // Start the operation of the instrument on a different thread
  Instrument::DataSaver datasaver(save_path, "IRAM");
  auto runner = AsyncRef(
              &Instrument::RunExperiment<decltype(chop), decltype(chopper_ctrl),
                                         decltype(wob), decltype(wobbler_ctrl),
                                         decltype(hk), decltype(housekeeping_ctrl),
                                         decltype(frontend), decltype(frontend_ctrl),
                                         decltype(backends), decltype(backend_ctrls)>,
              chop, chopper_ctrl,
              wob, wobbler_ctrl,
              hk, housekeeping_ctrl,
              frontend, frontend_ctrl,
              backends, backend_ctrls);
  
  // Start interchange between output data and operations on yet another thread
  auto saver = AsyncRef(&Instrument::ExchangeData<backends.N, decltype(chopper_ctrl),
                        decltype(housekeeping_ctrl), decltype(frontend_ctrl),
                        height_of_window, part_for_plot>,
                        backend_ctrls, chopper_ctrl, housekeeping_ctrl, frontend_ctrl, backend_data,
                        datasaver, backend_frames);
  
  // Setup of the tabs
  for (size_t i=0; i<backends.N; i++) {
    config.tabs.push_back(backends.name(i));
  }
  config.tabs.push_back(" All ");
  config.tabs.push_back(" Retrieval ");
  config.tabspos = 0;
  
  // Helpers
  const std::vector<std::string> devices = File::Devices({"USB", "S", "chopper", "wobbler", "sensors", "ACM"}, 100);
  
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
  for (size_t i=0; i<backends.N; i++)
    if (current_tab == i)
      backend_frames[i].plot(window, startpos);
  
  // Draw the combined backends
  if (current_tab == backends.N)
    GUI::Plotting::plot_combined(window, startpos, backend_frames);
    
  // Control tool
  if (GUI::Windows::sub<5, height_of_window, 0, part_for_plot, 2, height_of_window-part_for_plot>(window, startpos, "CTRL Tool 1")) {
    if (ImGui::BeginTabBar("GUI Control")) {
      
      if (ImGui::BeginTabItem(" Main ")) {
        bool none_init = not housekeeping_ctrl.init.load() and not chopper_ctrl.init.load() and
          not wobbler_ctrl.init.load() and not frontend_ctrl.init.load() and
          std::none_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.init.load();});
        bool all_init = housekeeping_ctrl.init.load() and chopper_ctrl.init.load() and
          wobbler_ctrl.init.load() and frontend_ctrl.init.load() and
          std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.init.load();});
          
        if (ImGui::Button(" Initialize all ")) {
          if (none_init)
            Instrument::InitAll(chop, chopper_ctrl, wob, wobbler_ctrl, hk, housekeeping_ctrl, frontend, frontend_ctrl, backends, backend_ctrls);
          else {
            config.gui_error = true;
            config.gui_errors.push_back("Cannot initialize all because some machines are initialized");
          }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button(" Close all ")) {
          if (all_init) {
            Instrument::CloseAll(chop, chopper_ctrl, wob, wobbler_ctrl, hk, housekeeping_ctrl, frontend, frontend_ctrl, backends, backend_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back("Cannot close all because some machines are not initialized");
          }
        }
        
        auto tmp_save_path = save_path.string();
        if (ImGui::InputText("Path", &tmp_save_path, ImGuiInputTextFlags_EnterReturnsTrue)) {
          if (std::filesystem::is_directory(tmp_save_path)) {
            save_path = tmp_save_path;
            directoryBrowser.SetPwd(save_path);
            config.new_save_path = true;
          } else {
            config.gui_error = true;
            config.gui_errors.push_back("Invalid directory");
          }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button(" Select Path ")) {
          directoryBrowser.Open();
        }
        
        if (ImGui::Button(" Ready to Run ")) {
          if (all_init) {
            Instrument::ReadyRunAll(chopper_ctrl, wobbler_ctrl, housekeeping_ctrl, frontend_ctrl, backend_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back("Cannot run the experiment because not all machines are initialized");
          }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button(" Stop Running ")) {
          if (all_init) {
            Instrument::UnreadyRunAll(chopper_ctrl, wobbler_ctrl, housekeeping_ctrl, frontend_ctrl, backend_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back("Cannot stop the experiment because it is not running");
          }
        }
        
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Chopper ")) {
        Instrument::Chopper::GuiSetup(chop, chopper_ctrl, devices);
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Wobbler ")) {
        Instrument::Wobbler::GuiSetup(wob, wobbler_ctrl, devices);
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Spectrometers ")) {
        Instrument::Spectrometer::GuiSetup(backends, backend_ctrls);
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Housekeeping ")) {
        Instrument::Housekeeping::GuiSetup(hk, housekeeping_ctrl, devices);
        ImGui::EndTabItem();
      }
      
      if (ImGui::BeginTabItem(" Frontend ")) {
        Instrument::Frontend::GuiSetup(frontend, frontend_ctrl);
        ImGui::EndTabItem();
      }
      
      ImGui::EndTabBar();
    }
  } GUI::Windows::end();
  
  // Information tool
  if (GUI::Windows::sub<5, height_of_window, 2, part_for_plot, 3, height_of_window-part_for_plot>(window, startpos, "DATA Tool 1")) {
    Instrument::AllInformation(chop, chopper_ctrl, wob, wobbler_ctrl, hk, housekeeping_ctrl, frontend, frontend_ctrl, backends, backend_ctrls);
  } GUI::Windows::end();
  
  // Select the directory  FIXME: Should be modal if running
  directoryBrowser.Display();
  if(directoryBrowser.HasSelected()) {
    config.new_save_path = true;
    save_path = directoryBrowser.GetSelected().string();
    directoryBrowser.ClearSelected();
  }
  
  if (config.new_save_path) {
    ImGui::OpenPopup("DirectoryChooser");
    config.new_save_path = false;
  }
  
  // Directory popup question
  if (ImGui::BeginPopupModal("DirectoryChooser")) {
    ImGui::Text("\n You are changing directory to %s\t", save_path.c_str());
    ImGui::TextWrapped(" This will reset the current save (even if you are choosing the same directory)."
    " Is this OK?");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      datasaver.updatePath(save_path);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(" Cancel ", {80.0f, 30.0f})) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  
  // Error handling (only if there are no known errors)
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
    for (auto& backend_ctrl: backend_ctrls) {
      if (backend_ctrl.error) {
        ImGui::OpenPopup("Error");
        backend_ctrl.error = false;
        config.active_errors++;
      }
    }
    if (housekeeping_ctrl.error) {
      ImGui::OpenPopup("Error");
      housekeeping_ctrl.error = false;
      config.active_errors++;
    }
    if (frontend_ctrl.error) {
      ImGui::OpenPopup("Error");
      frontend_ctrl.error = false;
      config.active_errors++;
    }
  }
  
  // Error popup
  if (ImGui::BeginPopupModal("Error")) {
    ImGui::Text(" Found %i error(s). These are cleaned up by pressing OK, but they must be fixed\t", config.active_errors);
    
    ImGui::TextWrapped(" Chopper: %s", chop.error_string().c_str());
    ImGui::TextWrapped(" Wobbler: %s", wob.error_string().c_str());
    for (size_t i=0; i<backends.N; i++) {
      ImGui::TextWrapped("%s: %s", backends.name(i).c_str(), backends.error_string(i).c_str());
    }
    ImGui::TextWrapped(" Housekeeping: %s", hk.error_string().c_str());
    ImGui::TextWrapped(" Frontend: %s", frontend.error_string().c_str());
    
    ImGui::Text(" ");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      chop.delete_error();
      wob.delete_error();
      for (size_t i=0; i<backends.N; i++) {
        backends.delete_error(i);
      }
      hk.delete_error();
      frontend.delete_error();
      ImGui::CloseCurrentPopup();
      config.active_errors=0;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Quit ", {80.0f, 30.0f})) {
      glfwSetWindowShouldClose(window, 1);
    }
    
    ImGui::EndPopup();
  }
  
  if (config.gui_error) {
    ImGui::OpenPopup("GUI Error");
    config.gui_error = false;
  }
  
  if (ImGui::BeginPopupModal("GUI Error")) {
    ImGui::Text("\tError! You performed invalid action(s)\t");
    for (auto& t: config.gui_errors) {
      ImGui::Text("%s\t", t.c_str());
    }
    
    ImGui::Text(" ");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      ImGui::CloseCurrentPopup();
      config.gui_errors.resize(0);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Quit ", {80.0f, 30.0f})) {
      glfwSetWindowShouldClose(window, 1);
    }
    
    ImGui::EndPopup();
  }
  
  // End of main loop
  EndWhileLoopGUI;
  
  Instrument::QuitAll(chopper_ctrl, wobbler_ctrl, housekeeping_ctrl, frontend_ctrl, backend_ctrls);
  auto running_errors = runner.get();
  if (running_errors.size()) {
    std::cerr << "Errors while stopping runner:\n:";
    for (auto& e: running_errors)
    std::cerr << e << '\n';
    return 1;
  }
  
  CleanupGUI;
  
  return EXIT_SUCCESS;
} catch (const std::exception& e) {
  std::ostringstream os;
  os << "Terminated with errors:\n" << e.what() << '\n';
  std::cerr << os.str();
  return EXIT_FAILURE;
}
