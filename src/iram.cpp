#include "backend.h"
#include "chopper.h"
#include "file.h"
#include "frontend.h"
#include "gui.h"
#include "housekeeping.h"
#include "instrument.h"
#include "multithread.h"
#include "python_interface.h"
#include "wobbler.h"

int main () try {
  auto py = Python::createPython();  // Must be declared even if unused

  InitializeGUI("IRAM");
  
  // Our states
  GUI::Config config;
  
  // Chopper
  Instrument::Chopper::Controller<Instrument::Chopper::ChopperPos::Cold,
                                  Instrument::Chopper::ChopperPos::Antenna,
                                  Instrument::Chopper::ChopperPos::Hot,
                                  Instrument::Chopper::ChopperPos::Antenna> chopper_ctrl;
  Instrument::Chopper::Dummy chop{"/home/larsson/Work/radctrl/python/chopper/chopper.py"};
  
  // Wobbler
  Instrument::Wobbler::Controller<4> wobbler_ctrl;
  wobbler_ctrl.pos = {3000, 7000, 3000, 7000};
  Instrument::Wobbler::Dummy wob{"/home/larsson/Work/radctrl/python/wobbler/IRAM.py"};
  
  // Housekeeping
  Instrument::Housekeeping::Controller housekeeping_ctrl;
  Instrument::Housekeeping::Dummy hk{"/home/larsson/Work/radctrl/python/housekeeping/sensors.py"};
  
  // Frontend
  Instrument::Frontend::Controller frontend_ctrl;
  Instrument::Frontend::Dummy frontend{""};
  
  // Spectrometers
  Instrument::Spectrometer::Backends backends{
    Instrument::Spectrometer::Dummy(" Dummy1 "),
    Instrument::Spectrometer::Dummy(" Dummy2 ")
  };
  std::array<Instrument::Spectrometer::Controller, backends.N> backend_ctrls{
    Instrument::Spectrometer::Controller("Dummy3", 0, 0,
                                  (Eigen::MatrixXf(2, 2) << 0, 1e9, 0, 100e9).finished(),
                                  (Eigen::VectorXi(2) << 1000, 1000).finished(),
                                  100, 1, false),
    Instrument::Spectrometer::Controller("Dummy4", 0, 0,
                                  (Eigen::MatrixXf(2, 2) << 0, 1e9, 0, 100e9).finished(),
                                  (Eigen::VectorXi(2) << 1000, 1000).finished(),
                                  100, 1, false)
  };
  
  // Files
  auto directoryBrowser = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_CreateNewDir);
  directoryBrowser.SetTitle("Select Directory");
  std::filesystem::path save_path{"/home/larsson/data/"};
  directoryBrowser.SetPwd(save_path);
  directoryBrowser.SetTypeFilters({"[D]"});
  
  // Start the operation of the instrument on a different thread
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
  
  // Data and plotting frames
  std::array<Instrument::Data, backends.N> backend_data;
  std::vector<std::array<GUI::Plotting::Frame, 4>> backend_frames{Instrument::Spectrometer::PlotFrame(backend_ctrls)};
  
  // Start interchange between output data and operations
  auto saver = AsyncRef(&Instrument::ExchangeData<backends.N, decltype(chopper_ctrl),
                        decltype(housekeeping_ctrl), decltype(frontend_ctrl)>,
                        backend_ctrls, chopper_ctrl, housekeeping_ctrl, frontend_ctrl, backend_data);
  
  // Setup TESTS
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
  const size_t current_tab = GUI::MainMenu::tabselect(config);
  
  auto startpos = ImGui::GetCursorPos();
  if (current_tab == 0) {
    std::vector<double> y {1,2,3,4,5,6,7,6,5,4,3,2,1};
    if (GUI::Windows::sub<2, 7, 0, 0, 1, 3>(window, startpos, "Plot tool 1")) {
      if (ImPlot::BeginPlot("Test", "X", "Y", {-1, -1})) {
        ImPlot::PlotLine("Plot", backend_ctrls[0].d[0].data(), 1000);
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
  
  if (GUI::Windows::sub<5, 7, 0, 6, 2, 1>(window, startpos, "CTRL Tool 1")) {
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
  
  if (GUI::Windows::sub<5, 7, 2, 6, 3, 1>(window, startpos, "DATA Tool 1")) {
    Instrument::AllInformation(chop, chopper_ctrl, wob, wobbler_ctrl, hk, housekeeping_ctrl, frontend, frontend_ctrl, backends, backend_ctrls);
  } GUI::Windows::end();
  
  // Select the directory
  directoryBrowser.Display();
  if(directoryBrowser.HasSelected()) {
    save_path = directoryBrowser.GetSelected().string();
    directoryBrowser.ClearSelected();
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
