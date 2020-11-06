#ifndef instrument_h
#define instrument_h

#include <algorithm>
#include <array>
#include <atomic>
#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "backend.h"
#include "chopper.h"
#include "file.h"
#include "gui.h"
#include "timeclass.h"
#include "wobbler.h"

namespace Instrument {
  
/*! Main data class for saving and plotting */
struct Data {
  std::atomic<bool> newdata;

  // Multithread guard
  std::mutex mtx;

  // Chopper Pointing
  Chopper::ChopperPos target;
  bool has_target;
  bool has_cold;
  bool has_hot;
  bool has_calib;
  bool has_noise;
  bool has_calib_avg;

  // Frequency grids of the instrument
  std::vector<std::vector<double>> f;

  // Last raw data of the instrument
  std::vector<std::vector<double>> last_target;
  std::vector<std::vector<double>> last_cold;
  std::vector<std::vector<double>> last_hot;

  // Last calibrated data of the instrument
  std::vector<std::vector<double>> last_calib;

  // Last calibrated noise temperature of the instrument
  std::vector<std::vector<double>> last_noise;

  // Average calibrated data of the instrument
  std::vector<std::vector<double>> avg_calib;

  // Housekeeping data
  double tcold;
  double thot;

  // Counters
  std::size_t num_measurements;  // Total number of raw measurements
  std::size_t num_to_avg;        // Number to average
  std::size_t avg_count;         //  Count of current averages
  
  /*! reset the average count */
  void reset_average() noexcept {
    mtx.lock();
    avg_count = 0;
    mtx.unlock();
  }
  
  /*! set the average count maximum */
  void set_average_max_count(std::size_t num) noexcept {
    mtx.lock();
    constexpr std::size_t one = 1;
    num_to_avg = std::max(one, num);
    if (avg_count > num_to_avg) {
      avg_count = num_to_avg;
    }
    mtx.unlock();
  }

  Data() noexcept = default;
  Data(const Data &other) noexcept
      : target(other.target),
        has_target(other.has_target),
        has_cold(other.has_cold),
        has_hot(other.has_hot),
        has_calib(other.has_calib),
        has_noise(other.has_noise),
        has_calib_avg(other.has_calib_avg),
        f(other.f),
        last_target(other.last_target),
        last_cold(other.last_cold),
        last_hot(other.last_hot),
        last_calib(other.last_calib),
        last_noise(other.last_noise),
        avg_calib(other.avg_calib),
        tcold(other.tcold),
        thot(other.thot),
        num_measurements(other.num_measurements),
        num_to_avg(other.num_to_avg),
        avg_count(other.avg_count) {}
  Data &operator=(const Data &other) noexcept {
    target = other.target;
    has_target = other.has_target;
    has_cold = other.has_cold;
    has_hot = other.has_hot;
    has_calib = other.has_calib;
    has_noise = other.has_noise;
    has_calib_avg = other.has_calib_avg;
    f = other.f;
    last_target = other.last_target;
    last_cold = other.last_cold;
    last_hot = other.last_hot;
    last_calib = other.last_calib;
    last_noise = other.last_noise;
    avg_calib = other.avg_calib;
    tcold = other.tcold;
    thot = other.thot;
    num_measurements = other.num_measurements;
    num_to_avg = other.num_to_avg;
    avg_count = other.avg_count;
    return *this;
  }

  template <typename T>
  Data(const std::vector<std::vector<T>> &freq_grid) noexcept
      : target(Chopper::ChopperPos::FINAL),
        has_target(false),
        has_cold(false),
        has_hot(false),
        has_calib(false),
        has_noise(false),
        has_calib_avg(false),
        f(freq_grid.size(), std::vector<double>(freq_grid[0].size())),
        last_target(f),
        last_cold(f),
        last_hot(f),
        last_calib(f),
        last_noise(f),
        avg_calib(f),
        tcold(std::numeric_limits<double>::max()),
        thot(std::numeric_limits<double>::max()),
        num_measurements(0),
        num_to_avg(std::numeric_limits<std::size_t>::max()),
        avg_count(0) {
    for (std::size_t i = 0; i < f.size(); i++) {
      for (std::size_t j = 0; j < f[i].size(); j++) {
        f[i][j] = freq_grid[i][j];
        last_target[i][j] = 0;
        last_cold[i][j] = 0;
        last_hot[i][j] = 0;
        last_calib[i][j] = 0;
        last_noise[i][j] = 0;
        avg_calib[i][j] = 0;
      }
    }
  }

  template <typename T>
  void update(Chopper::ChopperPos thistarget, double tc, double th,
              const std::vector<std::vector<T>> &data) noexcept {
    target = thistarget;
    num_measurements++;

    // Deal with cold load
    if (target == Chopper::ChopperPos::Cold) {
      tcold = tc;
      for (std::size_t i = 0; i < data.size(); i++)
        for (std::size_t j = 0; j < data[i].size(); j++)
          last_cold[i][j] = data[i][j] - 1;  // FIXME
      has_cold = true;
    }

    // Deal with hot load
    if (target == Chopper::ChopperPos::Hot) {
      thot = th;
      for (std::size_t i = 0; i < data.size(); i++)
        for (std::size_t j = 0; j < data[i].size(); j++)
          last_hot[i][j] = data[i][j] + 1;  // FIXME
      has_hot = true;
    }

    // Deal with target
    if (target == Chopper::ChopperPos::Antenna) {
      for (std::size_t i = 0; i < data.size(); i++)
        for (std::size_t j = 0; j < data[i].size(); j++)
          last_target[i][j] = data[i][j];
      has_target = true;
    }

    // Deal with noise
    if (has_cold and has_hot) {
      for (std::size_t i = 0; i < data.size(); i++)
        for (std::size_t j = 0; j < data[i].size(); j++)
          last_noise[i][j] = (thot * last_cold[i][j] - tcold * last_hot[i][j]) /
                             (last_hot[i][j] - last_cold[i][j]);
      has_noise = true;
    }

    // Deal with calibration
    if (has_cold and has_hot and has_target and (target == Chopper::ChopperPos::Cold or target == Chopper::ChopperPos::Hot)) {
      for (std::size_t i = 0; i < data.size(); i++)
        for (std::size_t j = 0; j < data[i].size(); j++)
          last_calib[i][j] = tcold + (thot - tcold) *
                                         (last_target[i][j] - last_cold[i][j]) /
                                         (last_hot[i][j] - last_cold[i][j]);
      has_calib = true;
    }

    mtx.lock();
    
    // calib
    if (has_hot and has_cold and has_target and (target == Chopper::ChopperPos::Cold or target == Chopper::ChopperPos::Hot)) {
      if (avg_count == 0) {
        for (std::size_t i = 0; i < data.size(); i++)
          for (std::size_t j = 0; j < data[i].size(); j++)
            avg_calib[i][j] = last_calib[i][j];
      } else {
        const double oldw = double(avg_count) / double(avg_count + 1);
        const double neww = 1.0 / double(avg_count + 1);
        for (std::size_t i = 0; i < data.size(); i++)
          for (std::size_t j = 0; j < data[i].size(); j++)
            avg_calib[i][j] =  oldw * avg_calib[i][j] + neww * last_calib[i][j];
      }
      
      if (num_to_avg not_eq avg_count) 
        avg_count++;
      has_calib_avg = true;
    }
    
    mtx.unlock();

    newdata.store(true);
  }
};

class DataSaver {
  std::mutex updatepath;
  std::size_t daily_copies;
  bool newfile;
  std::string basename;
  std::string timename;
  std::string filename;
  std::filesystem::path savedir;

  std::string filename_composer() noexcept {
    const std::string dot = ".";
    const std::string xml = ".xml";
    return savedir.string() + basename + dot + timename +
           dot + std::to_string(daily_copies) +
           xml;
  }

  void update_time(bool newname = false) noexcept {
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

 public:
  DataSaver(const std::string &dir, const std::string &basefilename) noexcept
      : daily_copies(0),
        newfile(true),
        basename(basefilename),
        timename("not-a-time-starting-value"),
        savedir(dir) {}

  void updatePath(const std::filesystem::path &newdir) {
    updatepath.lock();
    savedir = newdir;
    updatepath.unlock();
    update_time(true);
  }

  template <std::size_t N>
  void save(const Chopper::ChopperPos &last,
            const std::map<std::string, double> &hk_data,
            const std::map<std::string, double> &frontend_data,
            const std::array<std::vector<std::vector<float>>, N> &backends_data,
            const std::array<std::string, N> &backend_names) noexcept {
    update_time(not std::filesystem::exists(filename));

    if (newfile) {
      const std::string data = "Data";
      
      File::File<File::Operation::Write, File::Type::Xml> metadatafile(
          filename);

      metadatafile.new_child("Time");
      metadatafile.add_attribute("Type", "Time");
      metadatafile.add_attribute("Version", Time().Version());
      metadatafile.leave_child();

      metadatafile.new_child("Chopper");
      metadatafile.add_attribute("Type", "int");
      metadatafile.leave_child();

      metadatafile.new_child("Housekeeping");
      metadatafile.add_attribute("Type", "double");
      metadatafile.add_attribute("size", hk_data.size());
      std::size_t counter = 0;
      for (auto &hk : hk_data) {
        metadatafile.add_attribute(
          (data + std::to_string(counter)).c_str(),
            hk.first.c_str());
        counter += 1;
      }
      metadatafile.leave_child();

      metadatafile.new_child("Frontend");
      metadatafile.add_attribute("Type", "double");
      metadatafile.add_attribute("size", frontend_data.size());
      counter = 0;
      for (auto &fe : frontend_data) {
        metadatafile.add_attribute(
          (data + std::to_string(counter)).c_str(),
            fe.first.c_str());
        counter += 1;
      }
      metadatafile.leave_child();

      metadatafile.new_child("Backends");
      metadatafile.add_attribute("NumberOfBackends", N);
      metadatafile.add_attribute("Type", "VectorOfVector");
      for (std::size_t i = 0; i < N; i++) {
        metadatafile.new_child(
          (data + std::to_string(i)).c_str());
        metadatafile.add_attribute("Type", "float");
        metadatafile.add_attribute("Name", backend_names[i].c_str());
        metadatafile.add_attribute("NumberOfBoards", backends_data[i].size());
        metadatafile.add_attribute("ChannelsPerBoard",
                                   backends_data[i][0].size());
        metadatafile.leave_child();
      }
      metadatafile.leave_child();

      metadatafile.close();

      newfile = false;
    }

    File::File<File::Operation::AppendBinary, File::Type::Xml> datafile(
        filename);
    std::size_t n = 0;
    n += datafile.write(Time());
    n += datafile.write(int(last));
    for (auto &hk : hk_data) n += datafile.write(hk.second);
    for (auto &fe : frontend_data) n += datafile.write(fe.second);
    for (auto &specdata : backends_data) n += datafile.write(specdata);
  }
};

template <typename Chopper, typename ChopperController, typename Wobbler,
          typename WobblerController, typename Housekeeping,
          typename HousekeepingController, typename Frontend,
          typename FrontendController, typename Backends,
          typename BackendControllers>
void InitAll(Chopper &chop, ChopperController &chopper_ctrl, Wobbler &wob,
             WobblerController &wobbler_ctrl, Housekeeping &hk,
             HousekeepingController &housekeeping_ctrl, Frontend &frontend,
             FrontendController &frontend_ctrl, Backends &backends,
             BackendControllers &backend_ctrls) {
  std::cout << Time() << ' ' << "Binding housekeeping\n";
  hk.startup(housekeeping_ctrl.dev, housekeeping_ctrl.baudrate);
  std::cout << Time() << ' ' << "Initializing housekeeping\n";
  hk.init(false);
  if (hk.has_error()) {
    housekeeping_ctrl.error = true;
    std::cout << Time() << ' ' << "Error housekeeping\n";
  } else {
    std::cout << Time() << ' ' << "Done housekeeping\n";
    housekeeping_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding frontend\n";
  frontend.startup(frontend_ctrl.server, frontend_ctrl.port);
  std::cout << Time() << ' ' << "Initializing frontend\n";
  frontend.init(false);
  if (frontend.has_error()) {
    std::cout << Time() << ' ' << "Error frontend\n";
    frontend_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done frontend\n";
    frontend_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding wobbler\n";
  wob.startup(wobbler_ctrl.dev, wobbler_ctrl.baudrate, wobbler_ctrl.address);
  std::cout << Time() << ' ' << "Initializing wobbler\n";
  wob.init(wobbler_ctrl.pos[0], false);
  if (wob.has_error()) {
    std::cout << Time() << ' ' << "Error wobbler\n";
    wobbler_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done wobbler\n";
    wobbler_ctrl.init = true;
  }

  std::cout << Time() << ' ' << "Binding chopper\n";
  chop.startup(chopper_ctrl.dev, chopper_ctrl.offset, chopper_ctrl.sleeptime);
  std::cout << Time() << ' ' << "Initializing chopper\n";
  chop.init(false);
  if (chop.has_error()) {
    std::cout << Time() << ' ' << "Error chopper\n";
    chopper_ctrl.error = true;
  } else {
    std::cout << Time() << ' ' << "Done chopper\n";
    chopper_ctrl.init = true;
  }

  for (std::size_t i = 0; i < backends.N; i++) {
    std::cout << Time() << ' ' << "Binding backend " << i + 1 << "\n";
    backends.startup(i, backend_ctrls[i].host, backend_ctrls[i].tcp_port,
                     backend_ctrls[i].udp_port, backend_ctrls[i].freq_limits,
                     backend_ctrls[i].freq_counts,
                     backend_ctrls[i].integration_time_microsecs,
                     backend_ctrls[i].blank_time_microsecs,
                     backend_ctrls[i].mirror);
    std::cout << Time() << ' ' << "Initializing backend " << i + 1 << "\n";
    backends.init(i, false);
    if (backends.has_error(i)) {
      std::cout << Time() << ' ' << "Error backend " << i + 1 << "\n";
      backend_ctrls[i].error = true;
    } else {
      std::cout << Time() << ' ' << "Done backend " << i + 1 << "\n";
      backend_ctrls[i].init = true;
    }
  }
}

template <typename Chopper, typename ChopperController, typename Wobbler,
          typename WobblerController, typename Housekeeping,
          typename HousekeepingController, typename Frontend,
          typename FrontendController, typename Backends,
          typename BackendControllers>
void CloseAll(Chopper &chop, ChopperController &chopper_ctrl, Wobbler &wob,
              WobblerController &wobbler_ctrl, Housekeeping &hk,
              HousekeepingController &housekeeping_ctrl, Frontend &frontend,
              FrontendController &frontend_ctrl, Backends &backends,
              BackendControllers &backend_ctrls) {
  chop.close();
  chopper_ctrl.init = false;

  wob.close();
  wobbler_ctrl.init = false;

  hk.close();
  housekeeping_ctrl.init = false;

  frontend.close();
  frontend_ctrl.init = false;

  for (std::size_t i = 0; i < backends.N; i++) {
    backends.close(i);
    backend_ctrls[i].init = false;
  }
}

template <typename ChopperController, typename WobblerController,
          typename HousekeepingController, typename FrontendController,
          typename BackendControllers>
void ReadyRunAll(ChopperController &chopper_ctrl,
                 WobblerController &wobbler_ctrl,
                 HousekeepingController &housekeeping_ctrl,
                 FrontendController &frontend_ctrl,
                 BackendControllers &backend_ctrls) noexcept {
  chopper_ctrl.run = true;
  wobbler_ctrl.run = true;
  housekeeping_ctrl.run = true;
  frontend_ctrl.run = true;
  for (auto &ctrl : backend_ctrls) ctrl.run = true;
}

template <typename ChopperController, typename WobblerController,
          typename HousekeepingController, typename FrontendController,
          typename BackendControllers>
void UnreadyRunAll(ChopperController &chopper_ctrl,
                   WobblerController &wobbler_ctrl,
                   HousekeepingController &housekeeping_ctrl,
                   FrontendController &frontend_ctrl,
                   BackendControllers &backend_ctrls) noexcept {
  chopper_ctrl.run = false;
  wobbler_ctrl.run = false;
  housekeeping_ctrl.run = false;
  frontend_ctrl.run = false;
  for (auto &ctrl : backend_ctrls) ctrl.run = false;
}

template <typename ChopperController, typename WobblerController,
          typename HousekeepingController, typename FrontendController,
          typename BackendControllers>
void QuitAll(ChopperController &chopper_ctrl, WobblerController &wobbler_ctrl,
             HousekeepingController &housekeeping_ctrl,
             FrontendController &frontend_ctrl,
             BackendControllers &backend_ctrls) noexcept {
  chopper_ctrl.quit = true;
  wobbler_ctrl.quit = true;
  housekeeping_ctrl.quit = true;
  frontend_ctrl.quit = true;
  for (auto &ctrl : backend_ctrls) ctrl.quit = true;
}

template <typename Chopper, typename ChopperController, typename Wobbler,
          typename WobblerController, typename Housekeeping,
          typename HousekeepingController, typename Frontend,
          typename FrontendController, typename Backends,
          typename BackendControllers>
void AllControl(GUI::Config& config, std::filesystem::path& save_path,
                ImGui::FileBrowser& directoryBrowser, DataSaver& datasaver,
                Chopper &chop, ChopperController &chopper_ctrl,
                Wobbler &wob, WobblerController &wobbler_ctrl,
                Housekeeping &hk,
                HousekeepingController &housekeeping_ctrl,
                Frontend &frontend, FrontendController &frontend_ctrl,
                Backends &backends, BackendControllers &backend_ctrls) noexcept {
  static const std::vector<std::string> devices =
      File::Devices({"USB", "S", "chopper", "wobbler", "sensors", "ACM"}, 100);
  
  if (ImGui::BeginTabBar("GUI Control")) {
    if (ImGui::BeginTabItem(" Main ")) {
      bool none_init =
      not housekeeping_ctrl.init.load() and
      not chopper_ctrl.init.load() and not wobbler_ctrl.init.load() and
      not frontend_ctrl.init.load() and
      std::none_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                    [](auto &x) { return x.init.load(); });
      bool all_init =
      housekeeping_ctrl.init.load() and chopper_ctrl.init.load() and
      wobbler_ctrl.init.load() and frontend_ctrl.init.load() and
      std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                  [](auto &x) { return x.init.load(); });
      
      if (ImGui::Button(" Initialize all ")) {
        if (none_init)
          Instrument::InitAll(chop, chopper_ctrl, wob, wobbler_ctrl, hk,
                              housekeeping_ctrl, frontend, frontend_ctrl,
                              backends, backend_ctrls);
          else {
            config.gui_error = true;
            config.gui_errors.push_back(
              "Cannot initialize all because some machines are initialized");
          }
      }
      
      ImGui::SameLine();
      
      if (ImGui::Button(" Close all ")) {
        if (all_init) {
          Instrument::CloseAll(chop, chopper_ctrl, wob, wobbler_ctrl, hk,
                                housekeeping_ctrl, frontend, frontend_ctrl,
                                backends, backend_ctrls);
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
                                    backend_ctrls);
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
                                      backend_ctrls);
          } else {
            config.gui_error = true;
            config.gui_errors.push_back(
              "Cannot stop the experiment because it is not running");
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

template <typename Chopper, typename ChopperController, typename Wobbler,
          typename WobblerController, typename Housekeeping,
          typename HousekeepingController, typename Frontend,
          typename FrontendController, typename Backends,
          typename BackendControllers, typename BackendData>
void AllInformation(Chopper &chop, ChopperController &chopper_ctrl,
                    Wobbler &wob, WobblerController &wobbler_ctrl,
                    Housekeeping & /*hk*/,
                    HousekeepingController &housekeeping_ctrl,
                    Frontend &frontend, FrontendController &frontend_ctrl,
                    Backends &backends,
                    BackendControllers &backend_ctrls, BackendData& backend_data) noexcept {
  float x0 = ImGui::GetCursorPosX();
  float dx = ImGui::GetFontSize();

  if (ImGui::BeginTabBar("All Information")) {
    if (ImGui::BeginTabItem(" Main ")) {
      ImGui::Text("Chopper Target: %s", toString(chop.get_data()).c_str());

      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 14);
      ImGui::Text("Wobbler Target: %i", wob.get_data());

      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 27);
      auto cold = housekeeping_ctrl.data.find("Cold Load Temperature");
      if (cold == housekeeping_ctrl.data.end())
        ImGui::Text("Cold Target [K]: UNKNOWN");
      else
        ImGui::Text("Cold Target [K]: %.3lf", cold->second);

      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 41);
      auto hot = housekeeping_ctrl.data.find("Hot Load Temperature");
      if (hot == housekeeping_ctrl.data.end())
        ImGui::Text("Hot Target [K]: UNKNOWN");
      else
        ImGui::Text("Hot Target [K]: %.3lf", hot->second);

      ImGui::Text("Initialized: ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.init.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.init.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.init.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend.name().c_str(), frontend_ctrl.init.load());
      for (std::size_t i = 0; i < backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(),
                           backend_ctrls[i].init.load());
      }

      ImGui::Text("Running:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend.name().c_str(), frontend_ctrl.run.load());
      for (std::size_t i = 0; i < backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(),
                           backend_ctrls[i].run.load());
      }

      ImGui::Text("Waiting:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend.name().c_str(), frontend_ctrl.waiting.load());
      for (std::size_t i = 0; i < backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(),
                           backend_ctrls[i].waiting.load());
      }

      ImGui::Text("Operating:   ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend.name().c_str(),
                         frontend_ctrl.operating.load());
      for (std::size_t i = 0; i < backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(),
                           backend_ctrls[i].operating.load());
      }

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(" Chopper ")) {
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(" Wobbler ")) {
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(" Spectrometers ")) {
      if (ImGui::BeginTabBar(" Spectrometer Information ")) {
        
        for (std::size_t i = 0; i < backends.N; i++) {
          if (ImGui::BeginTabItem(backends.name(i).c_str())) {
            std::stringstream ss;
            ss << backends.now;
            std::string ymd, hms;
            ss >> ymd >> hms;
            ImGui::Text("Last measurement: %s %s", ymd.c_str(), hms.c_str());
            ImGui::EndTabItem();
            
            ImGui::Text("Average count: %" PRIu64, backend_data[i].avg_count);
            ImGui::Text("Number of measurements: %" PRIu64, backend_data[i].num_measurements);
            if (std::size_t num = backend_data[i].num_to_avg; ImGui::InputScalar(" Max Averaging ", ImGuiDataType_U64, &num, 0, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
              backend_data[i].set_average_max_count(num);
            }
            if (ImGui::Button(" Reset Averaging ")) {
              backend_data[i].reset_average();
            }
            ImGui::EndTabItem();
          }
        }
        
        ImGui::EndTabBar();
      }
    }

    if (ImGui::BeginTabItem(" Housekeeping ")) {
      constexpr int sameline_number = 3;
      int sameline = sameline_number;
      float local_dx = 0.0f;
      for (auto &data : housekeeping_ctrl.data) {
        sameline--;
        ImGui::Text("%s: %.3lf", data.first.c_str(), data.second);
        local_dx += dx * 30 * 0.7f;
        if (sameline) {
          ImGui::SameLine();
          ImGui::SetCursorPosX(x0 + local_dx);
        } else {
          sameline = sameline_number;
          local_dx = 0.0f;
        }
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(" Frontend ")) {
      constexpr int sameline_number = 3;
      int sameline = sameline_number;
      float local_dx = 0.0f;
      for (auto &data : frontend_ctrl.data) {
        sameline--;
        ImGui::Text("%s: %.3lf", data.first.c_str(), data.second);
        local_dx += dx * 30 * 0.7f;
        if (sameline) {
          ImGui::SameLine();
          ImGui::SetCursorPosX(x0 + local_dx);
        } else {
          sameline = sameline_number;
          local_dx = 0.0f;
        }
      }
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

template <typename Chopper, typename ChopperController, typename Wobbler,
          typename WobblerController, typename Housekeeping,
          typename HousekeepingController, typename Frontend,
          typename FrontendController, typename Backends,
          typename BackendControllers>
std::vector<std::string> RunExperiment(
    Chopper &chop, ChopperController &chopper_ctrl, Wobbler &wob,
    WobblerController &wobbler_ctrl, Housekeeping &hk,
    HousekeepingController &housekeeping_ctrl, Frontend &frontend,
    FrontendController &frontend_ctrl, Backends &backends,
    BackendControllers &backend_ctrls) noexcept {
  static_assert(ChopperController::N == WobblerController::N,
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
         std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                     [](auto &x) { return x.init.load(); });
  run = chopper_ctrl.run.load() and wobbler_ctrl.run.load() and
        housekeeping_ctrl.run.load() and frontend_ctrl.run.load() and
        std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                    [](auto &x) { return x.run.load(); });
  error = chop.has_error() or wob.has_error() or hk.has_error() or
          frontend.has_error() or backends.has_any_errors();
  quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or
         housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
         std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                     [](auto &x) { return x.quit.load(); });

  // Quit if any instrument wants it
  if (quit) goto stop;

  // Wait while not running or finding errors
  if (not init or not run or error) goto wait;

  // Run Chopper
  std::cout << Time() << " Running Chopper\n";
  chopper_ctrl.operating = chopper_ctrl.waiting = true;
  chop.run(chopper_ctrl.pos[pos]);
  chopper_ctrl.operating = chopper_ctrl.waiting = false;
  std::cout << Time() << " Done Chopper\n";

  // Run Wobbler
  std::cout << Time() << " Running Wobbler\n";
  wobbler_ctrl.operating = true;
  wob.move(wobbler_ctrl.pos[pos]);

  // Run Frontend
  std::cout << Time() << " Running Frontend\n";
  frontend_ctrl.operating = true;
  frontend.run();

  // Run Backends
  for (std::size_t i = 0; i < backends.N; i++) {
    std::cout << Time() << " Running Backend " << i + 1 << "\n";
    backends.run(i);
    backend_ctrls[i].operating = true;
  }

  // Run Housekeeping
  std::cout << Time() << " Running Housekeeping\n";
  housekeeping_ctrl.operating = true;
  hk.run();

  // Get Backends data
  for (std::size_t i = 0; i < backends.N; i++) {
    std::cout << Time() << " Get Data Backend " << i + 1 << "\n";
    backend_ctrls[i].waiting = true;
    backends.get_data(i, pos);
    backend_ctrls[i].waiting = backend_ctrls[i].operating = false;
    std::cout << Time() << " Done Backend\n";
  }

  // Get Housekeeping data
  std::cout << Time() << " Get Data Housekeeping\n";
  housekeeping_ctrl.waiting = true;
  hk.get_data();
  housekeeping_ctrl.waiting = housekeeping_ctrl.operating = false;
  std::cout << Time() << " Done Housekeeping\n";

  // Get Frontend data
  std::cout << Time() << " Get Data Frontend\n";
  frontend_ctrl.waiting = true;
  frontend.get_data();
  frontend_ctrl.waiting = frontend_ctrl.operating = false;
  std::cout << Time() << " Done Frontend\n";

  // Wait with loading the data if the storing device is having problems keeping
  // up
  while (std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                     [](auto &x) { return x.newdata.load(); }) or
         housekeeping_ctrl.newdata.load() or frontend_ctrl.newdata.load()) {
    Sleep(0.1);
    quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or
           housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
           std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                       [](auto &x) { return x.quit.load(); });
    if (quit) goto stop;
  }

  // Store the measurements in the controller
  std::cout << Time() << " Store Chopper\n";
  chopper_ctrl.lasttarget = chopper_ctrl.pos[pos];
  for (std::size_t i = 0; i < backends.N; i++) {
    std::cout << Time() << " Store Backend " << i + 1 << "\n";
    backend_ctrls[i].d = backends.datavec(i);
  }
  std::cout << Time() << " Store Housekeeping\n";
  housekeeping_ctrl.data = hk.data();
  std::cout << Time() << " Store Frontend\n";
  frontend_ctrl.data = frontend.data();

  // If the front end contains the hot load or cold load, load those over to
  // Housekeeping
  if constexpr (frontend.has_cold_load) {
    std::cout << Time() << " Copy Frontend Cold Load\n";
    housekeeping_ctrl.data["Cold Load Temperature"] = frontend.cold_load();
  }
  if constexpr (frontend.has_hot_load) {
    std::cout << Time() << " Copy Frontend Hot Load\n";
    housekeeping_ctrl.data["Hot Load Temperature"] = frontend.hot_load();
  }

  // Tell the storing device that there is new data
  std::cout << Time() << " Set all to done\n";
  for (auto &x : backend_ctrls) x.newdata.store(true);
  chopper_ctrl.newdata.store(true);
  housekeeping_ctrl.newdata.store(true);
  frontend_ctrl.newdata.store(true);

  // Finally just wait for the wobbler to be happy
  std::cout << Time() << " Wait for Wobbler\n";
  wobbler_ctrl.waiting = true;
  wob.wait();
  wobbler_ctrl.waiting = wobbler_ctrl.operating = false;
  std::cout << Time() << " Done Wobbler\n";

  // Ready for next loop
  pos = (pos + 1) % chopper_ctrl.N;
  goto loop;

// Stop must kill all machines if necessary
stop:
  try {
    if (chopper_ctrl.init) chop.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (wobbler_ctrl.init) wob.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (housekeeping_ctrl.init) hk.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  try {
    if (frontend_ctrl.init) frontend.close();
  } catch (const std::exception &e) {
    errors.push_back(e.what());
  }

  for (std::size_t i = 0; i < backends.N; i++) {
    try {
      if (backend_ctrls[i].init) backends.close(i);
    } catch (const std::exception &e) {
      errors.push_back(e.what());
    }
  }

  return errors;
}

template <std::size_t N, typename ChopperController, typename HousekeepingController,
          typename FrontendController, std::size_t PLOT_H, std::size_t PLOT_P>
void ExchangeData(
    std::array<Spectrometer::Controller, N> &backend_ctrls,
    ChopperController &chopper_ctrl, HousekeepingController &housekeeping_ctrl,
    FrontendController &frontend_ctrl, std::array<Data, N> &data,
    DataSaver &saver,
    std::array<GUI::Plotting::CAHA<PLOT_H, PLOT_P>, N> &rawplots,
    GUI::Plotting::ListOfLines<PLOT_H, PLOT_P> &hkplots) noexcept {
  bool allnew = false;
  bool quit = false;
  Chopper::ChopperPos last;
  const std::string med = " Median ";
  std::map<std::string, double> hk_data_longterm;
  std::vector<std::vector<double>> last_calib;
  std::map<std::string, double> hk_data;
  std::map<std::string, double> frontend_data;
  std::array<std::vector<std::vector<float>>, N> backends_data;
  std::array<std::string, N> backend_names;

  for (std::size_t i = 0; i < N; i++) {
    backend_names[i] = backend_ctrls[i].name;
    data[i] = Data(backend_ctrls[i].f);
    data[i].newdata.store(false);
  }

  if (rawplots.size() not_eq N) std::terminate();

wait:
  Sleep(0.1);

loop:
  allnew = housekeeping_ctrl.newdata.load() and frontend_ctrl.newdata.load() and
           std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                       [](auto &x) { return x.newdata.load(); });
  quit = housekeeping_ctrl.quit.load() and frontend_ctrl.quit.load() and
         std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(),
                     [](auto &x) { return x.quit.load(); });

  if (quit) goto stop;
  if (not allnew) goto wait;

  last = chopper_ctrl.lasttarget;
  hk_data = housekeeping_ctrl.data;
  frontend_data = frontend_ctrl.data;
  for (std::size_t i = 0; i < N; i++) backends_data[i] = backend_ctrls[i].d;

  chopper_ctrl.newdata.store(false);
  housekeeping_ctrl.newdata.store(false);
  frontend_ctrl.newdata.store(false);
  for (auto &ctrl : backend_ctrls) ctrl.newdata.store(false);

  // Save the raw data to file
  saver.save(last, hk_data, frontend_data, backends_data, backend_names);

  // Update plotting tools data
  for (std::size_t i = 0; i < N; i++) {
    data[i].update(last, hk_data["Cold Load Temperature"],
                   hk_data["Hot Load Temperature"], backends_data[i]);

    // Fill rawplots
    for (std::size_t j = 0; j < data[i].f.size(); j++) {
      if (last == Chopper::ChopperPos::Cold)
        rawplots[i].Raw()[3 * j + 0].setY(data[i].last_cold[j]);
      if (last == Chopper::ChopperPos::Antenna)
        rawplots[i].Raw()[3 * j + 1].setY(data[i].last_target[j]);
      if (last == Chopper::ChopperPos::Hot)
        rawplots[i].Raw()[3 * j + 2].setY(data[i].last_hot[j]);
      if (data[i].has_noise) rawplots[i].Noise()[j].setY(data[i].last_noise[j]);
      if (data[i].has_calib)
        rawplots[i].Integration()[j].setY(data[i].last_calib[j]);
      if (data[i].has_calib_avg)
        rawplots[i].Averaging()[j].setY(data[i].avg_calib[j]);
    }
  }
  
  // Update the housekeeping data
  for (auto& d: hk_data) hk_data_longterm[d.first] = d.second;
  for (auto& d: frontend_data) hk_data_longterm[d.first] = d.second;
  for (std::size_t i=0; i<N; i++) {
    if (data[i].has_calib) {
      last_calib = data[i].last_calib;
      for (std::size_t j=0; j<last_calib.size(); j++) {
        if (last_calib[j].size()) {
          std::sort(last_calib[j].begin(), last_calib[j].end());
          if (last_calib.size() > 1) {
            hk_data_longterm[backend_names[i] + med + std::to_string(j)] = last_calib[j][last_calib[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + med] = last_calib[j][last_calib[j].size()/2];
          }
        }
      }
    }
  }
  hkplots.update_data(Time().Seconds(), hk_data_longterm);
  hkplots.update_lines();

  goto loop;
stop : {}
}


/*! Handle errors.  If it returns false, errors cannot be handled */
template <typename Chopper, typename ChopperController, typename Wobbler,
typename WobblerController, typename Housekeeping,
typename HousekeepingController, typename Frontend,
typename FrontendController, typename Backends,
typename BackendControllers>
bool AllErrors(GUI::Config& config,
               Chopper &chop, ChopperController &chopper_ctrl, Wobbler &wob,
               WobblerController &wobbler_ctrl, Housekeeping &hk,
               HousekeepingController &housekeeping_ctrl, Frontend &frontend,
               FrontendController &frontend_ctrl, Backends &backends,
               BackendControllers &backend_ctrls) {
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
    for (auto &backend_ctrl : backend_ctrls) {
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
    ImGui::Text(
      " Found %i error(s). These are cleaned up by pressing OK, but they "
      "must be fixed\t",
      config.active_errors);
    
    ImGui::TextWrapped(" Chopper: %s", chop.error_string().c_str());
    ImGui::TextWrapped(" Wobbler: %s", wob.error_string().c_str());
    for (size_t i = 0; i < backends.N; i++) {
      ImGui::TextWrapped("%s: %s", backends.name(i).c_str(),
                         backends.error_string(i).c_str());
    }
    ImGui::TextWrapped(" Housekeeping: %s", hk.error_string().c_str());
    ImGui::TextWrapped(" Frontend: %s", frontend.error_string().c_str());
    
    ImGui::Text(" ");
    if (ImGui::Button(" OK ", {80.0f, 30.0f})) {
      chop.delete_error();
      wob.delete_error();
      for (size_t i = 0; i < backends.N; i++) {
        backends.delete_error(i);
      }
      hk.delete_error();
      frontend.delete_error();
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
}  // namespace Instrument

#endif  // instrument_h
