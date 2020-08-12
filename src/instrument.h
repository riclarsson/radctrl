#ifndef instrument_h
#define instrument_h

#include <algorithm>
#include <array>
#include <atomic>
#include <exception>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "backend.h"
#include "chopper.h"
#include "file.h"
#include "gui.h"
#include "timeclass.h"

namespace Instrument {
struct Data {
  // New variable (FIXME: should be respected to not overwrite any when true)
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
  
  // Running average of the raw data
  std::vector<std::vector<double>> avg_target;
  std::vector<std::vector<double>> avg_cold;
  std::vector<std::vector<double>> avg_hot;
  
  // Average calibrated data of the instrument
  std::vector<std::vector<double>> avg_calib;
  
  // Average calibrated noise temperature of the instrument
  std::vector<std::vector<double>> avg_noise;
  
  // Housekeeping data
  double tcold;
  double thot;
  
  // Counters
  size_t num_measurements;  // Total number of raw measurements
  size_t num_to_avg;  // Number to average
  size_t avg_count;  //  Count of current averages
  
  Data() noexcept = default;
  Data(const Data& other) noexcept :
    target(other.target), has_target(other.has_target), has_cold(other.has_cold),
    has_hot(other.has_hot), has_calib(other.has_calib), has_noise(other.has_noise),
    has_calib_avg(other.has_calib_avg), f(other.f), last_target(other.last_target),
    last_cold(other.last_cold), last_hot(other.last_hot), last_calib(other.last_calib),
    last_noise(other.last_noise), avg_target(other.avg_target), avg_cold(other.avg_cold),
    avg_hot(other.avg_hot), avg_calib(other.avg_calib), avg_noise(other.avg_noise),
    tcold(other.tcold), thot(other.thot), num_measurements(other.num_measurements),
    num_to_avg(other.num_to_avg), avg_count(other.avg_count) {}
    Data& operator=(const Data& other) noexcept {
    target = other.target; has_target= other.has_target; has_cold = other.has_cold;
    has_hot = other.has_hot; has_calib = other.has_calib; has_noise = other.has_noise;
    has_calib_avg = other.has_calib_avg; f = other.f; last_target = other.last_target;
    last_cold = other.last_cold; last_hot = other.last_hot; last_calib = other.last_calib;
    last_noise = other.last_noise; avg_target = other.avg_target; avg_cold = other.avg_cold;
    avg_hot = other.avg_hot; avg_calib = other.avg_calib; avg_noise = other.avg_noise;
    tcold = other.tcold; thot = other.thot; num_measurements = other.num_measurements;
    num_to_avg = other.num_to_avg; avg_count = other.avg_count;
    return *this;
  }
  
  template <typename T>
  Data(const std::vector<std::vector<T>>& freq_grid) noexcept : target(Chopper::ChopperPos::FINAL),
  has_target(false), has_cold(false), has_hot(false), has_calib(false), has_noise(false), has_calib_avg(false),
  f(freq_grid.size(), std::vector<double>(freq_grid[0].size())), last_target(f), last_cold(f), last_hot(f),
  last_calib(f), last_noise(f), avg_target(f), avg_cold(f), avg_hot(f), avg_calib(f), avg_noise(f),
  tcold(std::numeric_limits<double>::max()), thot(std::numeric_limits<double>::max()),
  num_measurements(0), num_to_avg(std::numeric_limits<size_t>::max()), avg_count(0) {
    for (size_t i=0; i<f.size(); i++) {
      for (size_t j=0; j<f[i].size();j ++) {
        f[i][j] = freq_grid[i][j];
        last_target[i][j] = 0;
        last_cold[i][j] = 0;
        last_hot[i][j] = 0;
        last_calib[i][j] = 0;
        last_noise[i][j] = 0;
        avg_target[i][j] = 0;
        avg_cold[i][j] = 0;
        avg_hot[i][j] = 0;
        avg_calib[i][j] = 0;
        avg_noise[i][j] = 0;
      }
    }
  }
  
  template <typename T>
  void update (Chopper::ChopperPos thistarget,
               double tc,
               double th,
               const std::vector<std::vector<T>>& data) noexcept {
    target = thistarget;
    num_measurements++;
    
    // Deal with cold load
    if (target == Chopper::ChopperPos::Cold) {
      tcold = tc;
      for (size_t i=0; i<data.size(); i++)
        for (size_t j=0; j<data[i].size(); j++)
          last_cold[i][j] = data[i][j]-1;  // FIXME
      has_cold = true;
    }
    
    // Deal with hot load
    if (target == Chopper::ChopperPos::Hot) {
      thot = th;
      for (size_t i=0; i<data.size(); i++)
        for (size_t j=0; j<data[i].size(); j++)
          last_hot[i][j] = data[i][j]+1;  // FIXME
      has_hot = true;
    }
    
    // Deal with target
    if (target == Chopper::ChopperPos::Antenna) {
      for (size_t i=0; i<data.size(); i++)
        for (size_t j=0; j<data[i].size(); j++)
          last_target[i][j] = data[i][j];
      has_target = true;
    }
    
    // Deal with noise
    if (has_cold and has_hot) {
      for (size_t i=0; i<data.size(); i++)
        for (size_t j=0; j<data[i].size(); j++)
          last_noise[i][j] = (thot*last_cold[i][j] - tcold*last_hot[i][j]) / (last_hot[i][j] - last_cold[i][j]);
      has_noise = true;
    }
    
    // Deal with calibration
    if (has_cold and has_hot and has_target) {
      for (size_t i=0; i<data.size(); i++)
        for (size_t j=0; j<data[i].size(); j++)
          last_calib[i][j] = tcold + (thot - tcold) * (last_target[i][j] - last_cold[i][j]) / (last_hot[i][j] - last_cold[i][j]);
      has_calib = true;
    }
    
    mtx.lock();
    // Deal with averaging 
    if (avg_count < num_to_avg)
      avg_count++;
    
    // cold load
    if (target == Chopper::ChopperPos::Cold) {
      if (avg_count not_eq 1) {
        for (size_t i=0; i<data.size(); i++)
          for (size_t j=0; j<data[i].size(); j++)
            avg_cold[i][j] = (avg_cold[i][j] * (avg_count-1) + last_cold[i][j]) / double(avg_count);
      } else {
        avg_cold = last_cold;
      }
    }
    
    // hot load
    if (target == Chopper::ChopperPos::Hot) {
      if (avg_count not_eq 1) {
        for (size_t i=0; i<data.size(); i++)
          for (size_t j=0; j<data[i].size(); j++)
            avg_hot[i][j] = (avg_hot[i][j] * (avg_count-1) + last_hot[i][j]) / double(avg_count);
      } else {
        avg_hot = last_hot;
      }
    }
    
    // target load
    if (target == Chopper::ChopperPos::Antenna) {
      if (avg_count not_eq 1) {
        for (size_t i=0; i<data.size(); i++)
          for (size_t j=0; j<data[i].size(); j++)
            avg_target[i][j] = (avg_target[i][j] * (avg_count-1) + last_target[i][j]) / double(avg_count);
      } else {
        avg_target = last_target;
      }
    }
    
    // noise
    if (has_hot and has_cold and (target==Chopper::ChopperPos::Cold or target==Chopper::ChopperPos::Hot)) {
      if (avg_count not_eq 1) {
        for (size_t i=0; i<data.size(); i++)
          for (size_t j=0; j<data[i].size(); j++)
            avg_noise[i][j] = (avg_noise[i][j] * (avg_count-1) + last_noise[i][j]) / double(avg_count);
      } else {
        avg_noise = last_noise;
      }
    }
    
    // calib
    if (has_hot and has_cold and has_target and (target==Chopper::ChopperPos::Antenna or not has_calib_avg)) {
      if (avg_count not_eq 1) {
        for (size_t i=0; i<data.size(); i++)
          for (size_t j=0; j<data[i].size(); j++)
            avg_calib[i][j] = (avg_calib[i][j] * (avg_count-1) + last_calib[i][j]) / double(avg_count);
      } else {
        avg_calib = last_calib;
      }
      has_calib_avg = true;
    }
    
    // Deal with bad avg
    if (avg_count > num_to_avg)
      avg_count--;
    mtx.unlock();
    
    newdata.store(true);
  }
};


class DataSaver {
  std::mutex updatepath;
  size_t daily_copies;
  bool newfile;
  std::string basename;
  std::string timename;
  std::string filename;
  std::filesystem::path savedir;

  std::string filename_composer() noexcept {
    return savedir.string() + basename + std::string{"."} + timename + std::string{"."} + std::to_string(daily_copies) + std::string{".xml"};
  }
  
  void update_time(bool newname=false) noexcept {
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
      daily_copies = 0;  // We first assume there are no copies
      std::string newfilename = filename_composer(); // and compose a filename
      while (std::filesystem::exists(newfilename)) {  // but if our new name exists
        daily_copies++;  // We test if the next filename exists
        newfilename = filename_composer();  // and compose a new file name
      }  // until there is a new filename
      updatepath.unlock();  // at which time we allow updating the path we are writing towards
      filename = newfilename;  // and set a new file name
    }
  }
  
public:
  DataSaver(const std::string& dir, const std::string& basefilename) noexcept : daily_copies(0), newfile(true),
  basename(basefilename), timename("not-a-time-starting-value"), savedir(dir) {}
  
  void updatePath(const std::filesystem::path& newdir) {
    updatepath.lock();
    savedir = newdir;
    updatepath.unlock();
    update_time(true);
  }
  
  template <size_t N>
  void save(const Chopper::ChopperPos& last,
            const std::map<std::string, double>& hk_data,
            const std::map<std::string, double>& frontend_data,
            const std::array<std::vector<std::vector<float>>, N>& backends_data,
            const std::array<std::string, N>& backend_names) noexcept {
    update_time(not std::filesystem::exists(filename));
    
    if (newfile) {
      File::File<File::Operation::Write, File::Type::Xml> metadatafile(filename);
      
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
      size_t counter = 0;
      for (auto& hk: hk_data) {
        metadatafile.add_attribute((std::string{"Data"} + std::to_string(counter)).c_str(), hk.first.c_str());
        counter += 1;
      }
      metadatafile.leave_child();
      
      metadatafile.new_child("Frontend");
      metadatafile.add_attribute("Type", "double");
      metadatafile.add_attribute("size", frontend_data.size());
      counter = 0;
      for (auto& fe: frontend_data) {
        metadatafile.add_attribute((std::string{"Data"} + std::to_string(counter)).c_str(), fe.first.c_str());
        counter += 1;
      }
      metadatafile.leave_child();
      
      metadatafile.new_child("Backends");
      metadatafile.add_attribute("NumberOfBackends", N);
      metadatafile.add_attribute("Type", "VectorOfVector");
      for (size_t i=0; i<N; i++) {
        metadatafile.new_child((std::string{"Data"} + std::to_string(i)).c_str());
        metadatafile.add_attribute("Type", "float");
        metadatafile.add_attribute("Name", backend_names[i].c_str());
        metadatafile.add_attribute("NumberOfBoards", backends_data[i].size());
        metadatafile.add_attribute("ChannelsPerBoard", backends_data[i][0].size());
        metadatafile.leave_child();
      }
      metadatafile.leave_child();
      
      metadatafile.close();
      
      newfile = false;
    }
    
    File::File<File::Operation::AppendBinary, File::Type::Xml> datafile(filename);
    size_t n = 0;
    n += datafile.write(Time());
    n += datafile.write(int(last));
    for (auto& hk: hk_data) n += datafile.write(hk.second);
    for (auto& fe: frontend_data) n += datafile.write(fe.second);
    for (auto& specdata: backends_data) n += datafile.write(specdata);
  }
};

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Frontend, typename FrontendController,
          typename Backends, typename BackendControllers>
void InitAll (Chopper& chop, ChopperController& chopper_ctrl,
              Wobbler& wob, WobblerController& wobbler_ctrl,
              Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
              Frontend& frontend, FrontendController& frontend_ctrl,
              Backends& backends, BackendControllers& backend_ctrls) {
  std::thread hkinit([](Housekeeping& inst, HousekeepingController& ctrl){
    inst.startup(ctrl.dev, ctrl.baudrate);
    inst.init(false);
    if (inst.has_error()) {
      ctrl.error = true;
    } else {
      ctrl.init = true;
    }
  }, std::ref(hk), std::ref(housekeeping_ctrl));
  
  std::thread frontendinit([](Frontend& inst, FrontendController& ctrl){
    inst.startup(ctrl.server, ctrl.port);
    inst.init(false);
    if (inst.has_error()) {
      ctrl.error = true;
    } else {
      ctrl.init = true;
    }
  }, std::ref(frontend), std::ref(frontend_ctrl));
  
  std::thread wobinit([](Wobbler& inst, WobblerController& ctrl){
    inst.startup(ctrl.dev, ctrl.baudrate, ctrl.address);
    inst.init(false);
    if (inst.has_error()) {
      ctrl.error = true;
    } else {
      ctrl.init = true;
    }
  }, std::ref(wob), std::ref(wobbler_ctrl));
  
  std::thread chopinit([](Chopper& inst, ChopperController& ctrl){
    inst.startup(ctrl.dev, ctrl.offset, ctrl.sleeptime);
    inst.init(false);
    if (inst.has_error()) {
      ctrl.error = true;
    } else {
      ctrl.init = true;
    }
  }, std::ref(chop), std::ref(chopper_ctrl));
  
  for (size_t ii=0; ii<backends.N; ii++) {
    std::thread backinit([](int i, Backends& inst, BackendControllers& ctrls) {
    inst.startup(i, ctrls[i].host, ctrls[i].tcp_port, ctrls[i].udp_port,
                     ctrls[i].freq_limits, ctrls[i].freq_counts,
                     ctrls[i].integration_time_microsecs, ctrls[i].blank_time_microsecs,
                     ctrls[i].mirror);
    inst.init(i, false);
    if (inst.has_error(i))
      ctrls[i].error = true;
    else
      ctrls[i].init = true;
    }, ii, std::ref(backends), std::ref(backend_ctrls));
    
    backinit.detach();
  }
  
  hkinit.detach();
  wobinit.detach();
  chopinit.detach();
  frontendinit.detach();
}

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Frontend, typename FrontendController,
          typename Backends, typename BackendControllers>
void CloseAll (Chopper& chop, ChopperController& chopper_ctrl,
               Wobbler& wob, WobblerController& wobbler_ctrl,
               Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
               Frontend& frontend, FrontendController& frontend_ctrl,
               Backends& backends, BackendControllers& backend_ctrls) {
  chop.close();
  chopper_ctrl.init = false;
  
  wob.close();
  wobbler_ctrl.init = false;
  
  hk.close();
  housekeeping_ctrl.init = false;
  
  frontend.close();
  frontend_ctrl.init = false;
  
  for (size_t i=0; i<backends.N; i++) {
    backends.close(i);
    backend_ctrls[i].init = false;
  }
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename FrontendController,
          typename BackendControllers>
void ReadyRunAll (ChopperController& chopper_ctrl,
                  WobblerController& wobbler_ctrl,
                  HousekeepingController& housekeeping_ctrl,
                  FrontendController& frontend_ctrl,
                  BackendControllers& backend_ctrls) noexcept {
  chopper_ctrl.run = true;
  wobbler_ctrl.run = true;
  housekeeping_ctrl.run = true;
  frontend_ctrl.run = true;
  for (auto& ctrl: backend_ctrls) ctrl.run = true;
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename FrontendController,
          typename BackendControllers>
void UnreadyRunAll (ChopperController& chopper_ctrl,
                    WobblerController& wobbler_ctrl,
                    HousekeepingController& housekeeping_ctrl,
                    FrontendController& frontend_ctrl,
                    BackendControllers& backend_ctrls) noexcept {
  chopper_ctrl.run = false;
  wobbler_ctrl.run = false;
  housekeeping_ctrl.run = false;
  frontend_ctrl.run = false;
  for (auto& ctrl: backend_ctrls) ctrl.run = false;
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename FrontendController,
          typename BackendControllers>
void QuitAll (ChopperController& chopper_ctrl,
              WobblerController& wobbler_ctrl,
              HousekeepingController& housekeeping_ctrl,
              FrontendController& frontend_ctrl,
              BackendControllers& backend_ctrls) noexcept {
  chopper_ctrl.quit = true;
  wobbler_ctrl.quit = true;
  housekeeping_ctrl.quit = true;
  frontend_ctrl.quit = true;
  for (auto& ctrl: backend_ctrls) ctrl.quit = true;
}

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Frontend, typename FrontendController,
          typename Backends, typename BackendControllers>
void AllInformation (Chopper& chop, ChopperController& chopper_ctrl,
                     Wobbler& wob, WobblerController& wobbler_ctrl,
                     Housekeeping& /*hk*/, HousekeepingController& housekeeping_ctrl,
                     Frontend& frontend, FrontendController& frontend_ctrl,
                     Backends& backends, BackendControllers& backend_ctrls) noexcept {
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
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].init.load());
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
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].run.load());
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
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].waiting.load());
      }
      
      ImGui::Text("Operating:   ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend.name().c_str(), frontend_ctrl.operating.load());
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].operating.load());
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
      ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem(" Housekeeping ")) {
      constexpr int sameline_number = 3;
      int sameline = sameline_number;
      float local_dx = 0.0f;
      for (auto& data: housekeeping_ctrl.data) {
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
      for (auto& data: frontend_ctrl.data) {
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

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Frontend, typename FrontendController,
          typename Backends, typename BackendControllers>
std::vector<std::string> RunExperiment (Chopper& chop, ChopperController& chopper_ctrl,
                                        Wobbler& wob, WobblerController& wobbler_ctrl,
                                        Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
                                        Frontend& frontend, FrontendController& frontend_ctrl,
                                        Backends& backends, BackendControllers& backend_ctrls) noexcept {
static_assert(ChopperController::N == WobblerController::N, "Need the same number of positions");

std::vector<std::string> errors(0);
size_t pos = 0;
bool run = false;
bool init = false;
bool quit = false;
bool error = false;
Time now;

goto loop;

wait:
Sleep(0.1);

loop:
init = chopper_ctrl.init.load() and wobbler_ctrl.init.load() and housekeeping_ctrl.init.load() and frontend_ctrl.init.load() and
  std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.init.load();});
run = chopper_ctrl.run.load() and wobbler_ctrl.run.load() and housekeeping_ctrl.run.load() and frontend_ctrl.run.load() and
  std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.run.load();});
error = chop.has_error() or wob.has_error() or hk.has_error() or frontend.has_error() or backends.has_any_errors();
quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
  std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.quit.load();});
  
// Quit if any instrument wants it
if (quit) goto stop;

// Wait while not running or finding errors
if (not init or not run or error) goto wait;

// Run Chopper
chopper_ctrl.operating = chopper_ctrl.waiting = true;
chop.run(chopper_ctrl.pos[pos]);
chopper_ctrl.operating = chopper_ctrl.waiting = false;

// Run Wobbler
wobbler_ctrl.operating = true;
wob.move(wobbler_ctrl.pos[pos]);

// Run Housekeeping
housekeeping_ctrl.operating = true;
hk.run();

// Run Frontend
frontend_ctrl.operating = true;
frontend.run();

// Run Backends
for (size_t i=0; i<backends.N; i++) {
  backends.run(i);
  backend_ctrls[i].operating = true;
}

// Get Backends data
for (size_t i=0; i<backends.N; i++) {
  backend_ctrls[i].waiting = true;
  backends.get_data(i, pos);
  backend_ctrls[i].waiting = backend_ctrls[i].operating = false;
}

// Get Housekeeping data
housekeeping_ctrl.waiting = true;
hk.get_data();
housekeeping_ctrl.waiting = housekeeping_ctrl.operating = false;

// Get Frontend data
frontend_ctrl.waiting = true;
frontend.get_data();
frontend_ctrl.waiting = frontend_ctrl.operating = false;

// Wait with loading the data if the storing device is having problems keeping up
while (std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.newdata.load();}) or housekeeping_ctrl.newdata.load() or frontend_ctrl.newdata.load()) {
  Sleep(0.1);
  quit = chopper_ctrl.quit.load() or wobbler_ctrl.quit.load() or housekeeping_ctrl.quit.load() or frontend_ctrl.quit.load() or
    std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.quit.load();});
  if (quit) goto stop;
}

// Store the measurements in the controller
chopper_ctrl.lasttarget = chopper_ctrl.pos[pos];
for (size_t i=0; i<backends.N; i++)
  backend_ctrls[i].d = backends.datavec(i);
housekeeping_ctrl.data = hk.data();
frontend_ctrl.data = frontend.data();

// If the front end contains the hot load or cold load, load those over to Housekeeping
if constexpr(frontend.has_cold_load)
  housekeeping_ctrl.data["Cold Load Temperature"] = frontend.cold_load();
if constexpr(frontend.has_hot_load)
  housekeeping_ctrl.data["Hot Load Temperature"] = frontend.hot_load();

// Tell the storing device that there is new data
for (auto& x: backend_ctrls)
  x.newdata.store(true);
chopper_ctrl.newdata.store(true);
housekeeping_ctrl.newdata.store(true);
frontend_ctrl.newdata.store(true);

// Finally just wait for the wobbler to be happy
wobbler_ctrl.waiting = true;
wob.wait();
wobbler_ctrl.waiting = wobbler_ctrl.operating = false;

// Ready for next loop
pos = (pos+1) % chopper_ctrl.N;
goto loop;

// Stop must kill all machines if necessary
stop:
  try {
    if (chopper_ctrl.init)
      chop.close();
  } catch (const std::exception& e) {
    errors.push_back(e.what());
  }
  
  try {
    if (wobbler_ctrl.init)
      wob.close();
  } catch (const std::exception& e) {
    errors.push_back(e.what());
  }
  
  try {
    if (housekeeping_ctrl.init)
      hk.close();
  } catch (const std::exception& e) {
    errors.push_back(e.what());
  }
  
  try {
    if (frontend_ctrl.init)
      frontend.close();
  } catch (const std::exception& e) {
    errors.push_back(e.what());
  }
  
  for (size_t i=0; i<backends.N; i++) {
    try {
      if (backend_ctrls[i].init)
        backends.close(i);
    } catch (const std::exception& e) {
      errors.push_back(e.what());
    }
  }
  
  return errors;
}

template <size_t N, typename ChopperController,
typename HousekeepingController, typename FrontendController,
size_t CAHA_N, size_t CAHA_M>
void ExchangeData(std::array<Spectrometer::Controller, N>& backend_ctrls,
                  ChopperController& chopper_ctrl,
                  HousekeepingController& housekeeping_ctrl,
                  FrontendController& frontend_ctrl,
                  std::array<Data, N>& data,
                  DataSaver& saver,
                  std::array<GUI::Plotting::CAHA<CAHA_N, CAHA_M>, N>& rawplots) noexcept {
bool allnew = false;
bool quit = false;
Chopper::ChopperPos last;
std::map<std::string, double> hk_data;
std::map<std::string, double> frontend_data;
std::array<std::vector<std::vector<float>>, N> backends_data;
std::array<std::string, N> backend_names;

for (size_t i=0; i<N; i++) {
  backend_names[i] = backend_ctrls[i].name;
  data[i] = Data(backend_ctrls[i].f);
  data[i].newdata.store(false);
}

if (rawplots.size() not_eq N) std::exit(1);

wait:
Sleep(0.1);

loop:
allnew = housekeeping_ctrl.newdata.load() and frontend_ctrl.newdata.load() and
std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.newdata.load();});
quit = housekeeping_ctrl.quit.load() and frontend_ctrl.quit.load() and
std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.quit.load();});

if (quit) goto stop;
if (not allnew) goto wait;

last = chopper_ctrl.lasttarget;
hk_data = housekeeping_ctrl.data;
frontend_data = frontend_ctrl.data;
for (size_t i=0; i<N; i++) backends_data[i] = backend_ctrls[i].d;

chopper_ctrl.newdata.store(false);
housekeeping_ctrl.newdata.store(false);
frontend_ctrl.newdata.store(false);
for (auto& ctrl: backend_ctrls) ctrl.newdata.store(false);

// Save the raw data to file
saver.save(last, hk_data, frontend_data, backends_data, backend_names);

// Update plotting tools data
for (size_t i=0; i<N; i++) {
  data[i].update(last, hk_data["Cold Load Temperature"], hk_data["Hot Load Temperature"], backends_data[i]);
  
  // Fill rawplots
  for (size_t j=0; j<data[i].f.size(); j++) {
    if (last == Chopper::ChopperPos::Cold)
      rawplots[i].Raw()[3*j+0].setY(data[i].last_cold[j]);
    if (last == Chopper::ChopperPos::Antenna)
      rawplots[i].Raw()[3*j+1].setY(data[i].last_target[j]);
    if (last == Chopper::ChopperPos::Hot)
      rawplots[i].Raw()[3*j+2].setY(data[i].last_hot[j]);
    if (data[i].has_noise)
      rawplots[i].Noise()[j].setY(data[i].last_noise[j]);
    if (data[i].has_calib)
      rawplots[i].Integration()[j].setY(data[i].last_calib[j]);
    if (data[i].has_calib_avg)
      rawplots[i].Averaging()[j].setY(data[i].avg_calib[j]);
  }
}

goto loop;
stop: {}
}
}  // Instrument

#endif  // instrument_h
