#include "instrument.h"

namespace Instrument {
void DataSaver::update_time(bool newname) noexcept {
  Time now{};
  std::stringstream ss;
  std::string newtimename;
  ss << now;
  ss >> newtimename;
  
  // The time has to be updated every day to not have too many files
  if (newtimename not_eq timename) {
    timename = newtimename;
    newname = true;
  }
  
  // We have a new file if we have a newname
  newfile = newname;
  
  if (newname) {
    updatepath.lock();  // We cannot change the path here
    daily_copies = 0;   // We first assume there are no copies
    std::string newfilename = filename_composer();  // and compose a filename
    while (
      std::filesystem::exists(newfilename)) {  // but if our new name exists
        daily_copies++;  // We test if the next filename exists
        newfilename = filename_composer();  // and compose a new file name
      }                                     // until there is a new filename
      updatepath.unlock();  // at which time we allow updating the path we are
      // writing towards
      filename = newfilename;  // and set a new file name
  }
}
  
void InitAll(Chopper::Controller &chopper_ctrl,
             Wobbler::Controller &wobbler_ctrl,
             Housekeeping::Controller &housekeeping_ctrl,
             Frontend::Controller &frontend_ctrl,
             Spectrometer::Controllers &spectrometer_ctrls) {
  std::cout << Time() << ' ' << "Binding housekeeping\n";
  housekeeping_ctrl.startup();
  std::cout << Time() << ' ' << "Initializing housekeeping\n";
  housekeeping_ctrl.initialize(false);
  if (housekeeping_ctrl.has_error()) {
    housekeeping_ctrl.error = true;
    std::cout << Time() << ' ' << "Error housekeeping\n";
  } else {
    std::cout << Time() << ' ' << "Done housekeeping\n";
    housekeeping_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding frontend\n";
  frontend_ctrl.startup();
  std::cout << Time() << ' ' << "Initializing frontend\n";
  frontend_ctrl.initialize(false);
  if (frontend_ctrl.has_error()) {
    std::cout << Time() << ' ' << "Error frontend\n";
    frontend_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done frontend\n";
    frontend_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding wobbler\n";
  wobbler_ctrl.startup();
  std::cout << Time() << ' ' << "Initializing wobbler\n";
  wobbler_ctrl.initialize(wobbler_ctrl.pos[0], false);
  if (wobbler_ctrl.has_error()) {
    std::cout << Time() << ' ' << "Error wobbler\n";
    wobbler_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done wobbler\n";
    wobbler_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding chopper\n";
  chopper_ctrl.startup();
  std::cout << Time() << ' ' << "Initializing chopper\n";
  chopper_ctrl.initialize(false);
  if (chopper_ctrl.has_error()) {
    std::cout << Time() << ' ' << "Error chopper\n";
    chopper_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done chopper\n";
    chopper_ctrl.init = true;
  }

  for (auto& spec: spectrometer_ctrls.backends) {
    std::cout << Time() << ' ' << "Binding backend " << spec.name << "\n";
    spec.startup();
    std::cout << Time() << ' ' << "Initializing backend " << spec.name << "\n";
    spec.initialize(false);
    if (spec.has_error()) {
      std::cout << Time() << ' ' << "Error backend " << spec.name << "\n";
      spec.error = true;
    } else {
      std::cout << Time() << ' ' << "Done backend " << spec.name << "\n";
      spec.init = true;
    }
  }
}

void CloseAll(Chopper::Controller &chopper_ctrl,
              Wobbler::Controller &wobbler_ctrl,
              Housekeeping::Controller &housekeeping_ctrl,
              Frontend::Controller &frontend_ctrl,
              Spectrometer::Controllers &spectrometer_ctrls) {
  chopper_ctrl.close();
  chopper_ctrl.init = false;

  wobbler_ctrl.close();
  wobbler_ctrl.init = false;

  housekeeping_ctrl.close();
  housekeeping_ctrl.init = false;

  frontend_ctrl.close();
  frontend_ctrl.init = false;

  for (auto& spec: spectrometer_ctrls.backends) {
    spec.close();
    spec.init = false;
  }
}

void ReadyRunAll(Chopper::Controller &chopper_ctrl,
                 Wobbler::Controller &wobbler_ctrl,
                 Housekeeping::Controller &housekeeping_ctrl,
                 Frontend::Controller &frontend_ctrl,
                 Spectrometer::Controllers &spectrometer_ctrls) {
  chopper_ctrl.run = true;
  wobbler_ctrl.run = true;
  housekeeping_ctrl.run = true;
  frontend_ctrl.run = true;
  for (auto &spec : spectrometer_ctrls.backends) spec.run = true;
}

void UnreadyRunAll(Chopper::Controller &chopper_ctrl,
                   Wobbler::Controller &wobbler_ctrl,
                   Housekeeping::Controller &housekeeping_ctrl,
                   Frontend::Controller &frontend_ctrl,
                   Spectrometer::Controllers &spectrometer_ctrls) {
  chopper_ctrl.run = false;
  wobbler_ctrl.run = false;
  housekeeping_ctrl.run = false;
  frontend_ctrl.run = false;
  for (auto &spec : spectrometer_ctrls.backends) spec.run = false;
}

void QuitAll(Chopper::Controller &chopper_ctrl, Wobbler::Controller &wobbler_ctrl,
             Housekeeping::Controller &housekeeping_ctrl,
             Frontend::Controller &frontend_ctrl,
             Spectrometer::Controllers &spectrometer_ctrls) {
  chopper_ctrl.quit = true;
  wobbler_ctrl.quit = true;
  housekeeping_ctrl.quit = true;
  frontend_ctrl.quit = true;
  for (auto &spec : spectrometer_ctrls.backends) spec.quit = true;
}

void AllControl(GUI::Config& config, std::filesystem::path& save_path,
                ImGui::FileBrowser& directoryBrowser, DataSaver& datasaver,
                Chopper::Controller &chopper_ctrl,
                Wobbler::Controller &wobbler_ctrl,
                Housekeeping::Controller &housekeeping_ctrl,
                Frontend::Controller &frontend_ctrl,
                Spectrometer::Controllers &spectrometer_ctrls) {
  static const std::vector<std::string> devices =
      File::Devices({"USB", "S", "chopper", "wobbler", "sensors", "ACM"}, 100);
  
  if (ImGui::BeginTabBar("GUI Control")) {
    if (ImGui::BeginTabItem(" Main ")) {
      bool none_init =
      not housekeeping_ctrl.init.load() and
      not chopper_ctrl.init.load() and not wobbler_ctrl.init.load() and
      not frontend_ctrl.init.load() and
      std::none_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                    [](auto &x) { return x.init.load(); });
      bool all_init =
      housekeeping_ctrl.init.load() and chopper_ctrl.init.load() and
      wobbler_ctrl.init.load() and frontend_ctrl.init.load() and
      std::all_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                  [](auto &x) { return x.init.load(); });
      
      if (ImGui::Button(" Initialize all ")) {
        if (none_init)
          Instrument::InitAll(chopper_ctrl, wobbler_ctrl,
                              housekeeping_ctrl, frontend_ctrl,
                              spectrometer_ctrls);
          else {
            config.gui_error = true;
            config.gui_errors.push_back(
              "Cannot initialize all because some machines are initialized");
          }
      }
      
      ImGui::SameLine();
      
      if (ImGui::Button(" Close all ")) {
        if (all_init) {
          Instrument::CloseAll(chopper_ctrl, wobbler_ctrl,
                               housekeeping_ctrl, frontend_ctrl,
                               spectrometer_ctrls);
        } else {
          config.gui_error = true;
          config.gui_errors.push_back(
            "Cannot close all because some machines are not initialized");
        }
      }
      
      auto tmp_save_path = save_path.string();
      if (ImGui::InputText("Path", &tmp_save_path,
        ImGuiInputTextFlags_EnterReturnsTrue)) {
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
            Instrument::ReadyRunAll(chopper_ctrl, wobbler_ctrl,
                                    housekeeping_ctrl, frontend_ctrl,
                                    spectrometer_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back(
              "Cannot run the experiment because not all machines are "
              "initialized");
          }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button(" Stop Running ")) {
          if (all_init) {
            Instrument::UnreadyRunAll(chopper_ctrl, wobbler_ctrl,
                                      housekeeping_ctrl, frontend_ctrl,
                                      spectrometer_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back(
              "Cannot stop the experiment because it is not running");
          }
        }
        
        ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Chopper ")) {
      Instrument::Chopper::GuiSetup(chopper_ctrl, devices);
      ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Wobbler ")) {
      Instrument::Wobbler::GuiSetup(wobbler_ctrl, devices);
      ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Spectrometers ")) {
      Instrument::Spectrometer::GuiSetup(spectrometer_ctrls);
      ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Housekeeping ")) {
      Instrument::Housekeeping::GuiSetup(housekeeping_ctrl, devices);
      ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Frontend ")) {
      Instrument::Frontend::GuiSetup(frontend_ctrl);
      ImGui::EndTabItem();
    }
    
    ImGui::EndTabBar();
  }
  
  // Select the directory  FIXME: Should be modal if running
  directoryBrowser.Display();
  if (directoryBrowser.HasSelected()) {
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
    ImGui::TextWrapped(
      " This will reset the current save (even if you are choosing the same "
      "directory)."
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
}

std::vector<std::string> RunExperiment(
    Chopper::Controller &chopper_ctrl,
    Wobbler::Controller &wobbler_ctrl,
    Housekeeping::Controller &housekeeping_ctrl,
    Frontend::Controller &frontend_ctrl,
    Spectrometer::Controllers &spectrometer_ctrls) {
  static_assert(Chopper::Controller::N == Wobbler::Controller::N,
                "Need the same number of positions");

  std::vector<std::string> errors(0);
  std::size_t pos = 0;
  bool run = false;
  bool init = false;
  bool quit = false;
  bool error = false;
  Time now;

  goto loop;

wait:
  Sleep(0.1);

loop:
  init = chopper_ctrl.init.load() and wobbler_ctrl.init.load() and
         housekeeping_ctrl.init.load() and frontend_ctrl.init.load() and
         std::all_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                     [](auto &x) { return x.init.load(); });
  run = chopper_ctrl.run.load() and wobbler_ctrl.run.load() and
        housekeeping_ctrl.run.load() and frontend_ctrl.run.load() and
        std::all_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                    [](auto &x) { return x.run.load(); });
  error = chopper_ctrl.has_error() or
          wobbler_ctrl.has_error() or
          housekeeping_ctrl.has_error() or
          frontend_ctrl.has_error() or spectrometer_ctrls.has_any_errors();
  quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or
         housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
         std::any_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                     [](auto &x) { return x.quit.load(); });

  // Quit if any instrument wants it
  if (quit) goto stop;

  // Wait while not running or finding errors
  if (not init or not run or error) goto wait;

  // Run Chopper
  std::cout << Time() << " Running Chopper\n";
  chopper_ctrl.operating = chopper_ctrl.waiting = true;
  chopper_ctrl.change_pos(chopper_ctrl.pos[pos]);
  chopper_ctrl.operating = chopper_ctrl.waiting = false;
  std::cout << Time() << " Done Chopper\n";

  // Run Wobbler
  std::cout << Time() << " Running Wobbler\n";
  wobbler_ctrl.operating = true;
  wobbler_ctrl.change_pos(wobbler_ctrl.pos[pos]);

  // Run Frontend
  std::cout << Time() << " Running Frontend\n";
  frontend_ctrl.operating = true;
  frontend_ctrl.run_machine();

  // Run Backends
  for (auto& spec : spectrometer_ctrls.backends) {
    std::cout << Time() << " Running Backend " << spec.name << "\n";
    spec.run_machine();
    spec.operating = true;
  }

  // Run Housekeeping
  std::cout << Time() << " Running Housekeeping\n";
  housekeeping_ctrl.operating = true;
  housekeeping_ctrl.run_machine();

  // Get Backends data
  for (auto& spec : spectrometer_ctrls.backends) {
    std::cout << Time() << " Get Data Backend " << spec.name << "\n";
    spec.waiting = true;
    spec.get_data(pos);
    spec.waiting = spec.operating = false;
    std::cout << Time() << " Done Backend\n";
  }

  // Get Housekeeping data
  std::cout << Time() << " Get Data Housekeeping\n";
  housekeeping_ctrl.waiting = true;
  housekeeping_ctrl.get_data();
  housekeeping_ctrl.waiting = housekeeping_ctrl.operating = false;
  std::cout << Time() << " Done Housekeeping\n";

  // Get Frontend data
  std::cout << Time() << " Get Data Frontend\n";
  frontend_ctrl.waiting = true;
  frontend_ctrl.get_data();
  frontend_ctrl.waiting = frontend_ctrl.operating = false;
  std::cout << Time() << " Done Frontend\n";

  // Wait with loading the data if the storing device is having problems keeping
  // up
  while (std::any_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                     [](auto &x) { return x.newdata.load(); }) or
         housekeeping_ctrl.newdata.load() or frontend_ctrl.newdata.load()) {
    Sleep(0.1);
    quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or
           housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
           std::any_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                       [](auto &x) { return x.quit.load(); });
    if (quit) goto stop;
  }

  // Store the measurements in the controller
  std::cout << Time() << " Store Chopper\n";
  chopper_ctrl.lasttarget = chopper_ctrl.pos[pos];
  for (auto& spec : spectrometer_ctrls.backends) {
    std::cout << Time() << " Store Backend " << spec.name << "\n";
    spec.store_data();
  }
  std::cout << Time() << " Store Housekeeping\n";
  housekeeping_ctrl.data = housekeeping_ctrl.data_load();
  std::cout << Time() << " Store Frontend\n";
  frontend_ctrl.data = frontend_ctrl.data_load();

  // If the front end contains the hot load or cold load, load thosehere
  frontend_ctrl.set_cold(housekeeping_ctrl.data["Cold Load Temperature"]);
  frontend_ctrl.set_hot(housekeeping_ctrl.data["Hot Load Temperature"]);

  // Tell the storing device that there is new data
  std::cout << Time() << " Set all to done\n";
  for (auto &spec : spectrometer_ctrls.backends) spec.newdata.store(true);
  chopper_ctrl.newdata.store(true);
  housekeeping_ctrl.newdata.store(true);
  frontend_ctrl.newdata.store(true);

  // Finally just wait for the wobbler to be happy
  std::cout << Time() << " Wait for Wobbler\n";
  wobbler_ctrl.waiting = true;
  wobbler_ctrl.wait();
  wobbler_ctrl.waiting = wobbler_ctrl.operating = false;
  std::cout << Time() << " Done Wobbler\n";

  // Ready for next loop
  pos = (pos + 1) % chopper_ctrl.N;
  goto loop;

// Stop must kill all machines if necessary
stop:
  try {
    if (chopper_ctrl.init) chopper_ctrl.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (wobbler_ctrl.init) wobbler_ctrl.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (housekeeping_ctrl.init) housekeeping_ctrl.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (frontend_ctrl.init) frontend_ctrl.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  for (auto &spec : spectrometer_ctrls.backends) {
    try {
      if (spec.init) spec.close();
    } catch (const std::exception &e) {
      errors.push_back(e.what());
    }
  }

  return errors;
}

bool AllErrors(GUI::Config& config,
               Chopper::Controller &chopper_ctrl,
               Wobbler::Controller &wobbler_ctrl,
               Housekeeping::Controller &housekeeping_ctrl,
               Frontend::Controller &frontend_ctrl,
               Spectrometer::Controllers &spectrometer_ctrls) {
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
    for (auto &spec : spectrometer_ctrls.backends) {
      if (spec.error) {
        ImGui::OpenPopup("Error");
        spec.error = false;
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
    ImGui::Text(
      " Found %i error(s). These are cleaned up by pressing OK, but they "
      "must be fixed\t",
      config.active_errors);
    
    ImGui::TextWrapped(" Chopper: %s", chopper_ctrl.error_string().c_str());
    ImGui::TextWrapped(" Wobbler: %s", wobbler_ctrl.error_string().c_str());
    for (auto &spec : spectrometer_ctrls.backends) {
      ImGui::TextWrapped("%s: %s", spec.name.c_str(), spec.error_string().c_str());
    }
    ImGui::TextWrapped(" Housekeeping: %s", housekeeping_ctrl.error_string().c_str());
    ImGui::TextWrapped(" Frontend: %s", frontend_ctrl.error_string().c_str());
    
    ImGui::Text(" ");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      chopper_ctrl.delete_error();
      wobbler_ctrl.delete_error();
      for (auto &spec : spectrometer_ctrls.backends) {
        spec.delete_error();
      }
      housekeeping_ctrl.delete_error();
      frontend_ctrl.delete_error();
      ImGui::CloseCurrentPopup();
      config.active_errors = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Quit ", {80.0f, 30.0f})) {
      return false;
    }
    
    ImGui::EndPopup();
  }
  
  if (config.gui_error) {
    ImGui::OpenPopup("GUI Error");
    config.gui_error = false;
  }
  
  if (ImGui::BeginPopupModal("GUI Error")) {
    ImGui::Text("\tError! You performed invalid action(s)\t");
    for (auto &t : config.gui_errors) {
      ImGui::Text("%s\t", t.c_str());
    }
    
    ImGui::Text(" ");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      ImGui::CloseCurrentPopup();
      config.gui_errors.resize(0);
    }
    ImGui::SameLine();
    if (ImGui::Button(" Quit ", {80.0f, 30.0f})) {
      return false;
    }
    
    ImGui::EndPopup();
  }
  
  return true;
}
}
