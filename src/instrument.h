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
#include "frontend.h"
#include "gui.h"
#include "housekeeping.h"
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
};  // Data

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

  void update_time(bool newname = false) noexcept;

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
};  // DataSaver

void InitAll(Chopper::Controller &chopper_ctrl,
             Wobbler::Controller &wobbler_ctrl,
             Housekeeping::Controller &housekeeping_ctrl,
             Frontend::Controller &frontend_ctrl,
             Spectrometer::Controllers &spectrometer_ctrls);

void CloseAll(Chopper::Controller &chopper_ctrl,
              Wobbler::Controller &wobbler_ctrl,
              Housekeeping::Controller &housekeeping_ctrl,
              Frontend::Controller &frontend_ctrl,
              Spectrometer::Controllers &spectrometer_ctrls);

void ReadyRunAll(Chopper::Controller &chopper_ctrl,
                 Wobbler::Controller &wobbler_ctrl,
                 Housekeeping::Controller &housekeeping_ctrl,
                 Frontend::Controller &frontend_ctrl,
                 Spectrometer::Controllers &spectrometer_ctrls);

void UnreadyRunAll(Chopper::Controller &chopper_ctrl,
                   Wobbler::Controller &wobbler_ctrl,
                   Housekeeping::Controller &housekeeping_ctrl,
                   Frontend::Controller &frontend_ctrl,
                   Spectrometer::Controllers &spectrometer_ctrls);

void QuitAll(Chopper::Controller &chopper_ctrl, Wobbler::Controller &wobbler_ctrl,
             Housekeeping::Controller &housekeeping_ctrl,
             Frontend::Controller &frontend_ctrl,
             Spectrometer::Controllers &spectrometer_ctrls);

void AllControl(GUI::Config& config, std::filesystem::path& save_path,
                ImGui::FileBrowser& directoryBrowser, DataSaver& datasaver,
                Chopper::Controller &chopper_ctrl,
                Wobbler::Controller &wobbler_ctrl,
                Housekeeping::Controller &housekeeping_ctrl,
                Frontend::Controller &frontend_ctrl,
                Spectrometer::Controllers &spectrometer_ctrls);

template <typename BackendData>
void AllInformation(Chopper::Controller &chopper_ctrl,
                    Wobbler::Controller &wobbler_ctrl,
                    Housekeeping::Controller &housekeeping_ctrl,
                    Frontend::Controller &frontend_ctrl,
                    Spectrometer::Controllers &spectrometer_ctrls, BackendData& backend_data) noexcept {
  float x0 = ImGui::GetCursorPosX();
  float dx = ImGui::GetFontSize();

  if (ImGui::BeginTabBar("All Information")) {
    if (ImGui::BeginTabItem(" Main ")) {
      ImGui::Text("Chopper Target: %s", toString(chopper_ctrl.get_pos()).c_str());

      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 14);
      ImGui::Text("Wobbler Target: %i", wobbler_ctrl.get_pos());

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
      ImGui::RadioButton(frontend_ctrl.name().c_str(), frontend_ctrl.init.load());
      for (auto& spec: spectrometer_ctrls.backends) {
        ImGui::SameLine();
        ImGui::RadioButton(spec.name.c_str(), spec.init.load());
      }

      ImGui::Text("Running:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.run.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend_ctrl.name().c_str(), frontend_ctrl.run.load());
      for (auto& spec: spectrometer_ctrls.backends) {
        ImGui::SameLine();
        ImGui::RadioButton(spec.name.c_str(), spec.run.load());
      }

      ImGui::Text("Waiting:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.waiting.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend_ctrl.name().c_str(), frontend_ctrl.waiting.load());
      for (auto& spec: spectrometer_ctrls.backends) {
        ImGui::SameLine();
        ImGui::RadioButton(spec.name.c_str(), spec.waiting.load());
      }

      ImGui::Text("Operating:   ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.operating.load());
      ImGui::SameLine();
      ImGui::RadioButton(frontend_ctrl.name().c_str(), frontend_ctrl.operating.load());
      for (auto& spec: spectrometer_ctrls.backends) {
        ImGui::SameLine();
        ImGui::RadioButton(spec.name.c_str(), spec.operating.load());
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
        
        for (std::size_t i = 0; i < spectrometer_ctrls.backends.size(); i++) {
          auto& spec = spectrometer_ctrls.backends[i];
          if (ImGui::BeginTabItem(spec.name.c_str())) {
            std::stringstream ss;
            ss << spectrometer_ctrls.now;
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

std::vector<std::string> RunExperiment(
    Chopper::Controller &chopper_ctrl,
    Wobbler::Controller &wobbler_ctrl,
    Housekeeping::Controller &housekeeping_ctrl,
    Frontend::Controller &frontend_ctrl,
    Spectrometer::Controllers &spectrometer_ctrls);

template <std::size_t N, std::size_t PLOT_H, std::size_t PLOT_P>
void ExchangeData(
    Spectrometer::Controllers &spectrometer_ctrls,
    Chopper::Controller &chopper_ctrl, Housekeeping::Controller &housekeeping_ctrl,
    Frontend::Controller &frontend_ctrl, std::array<Data, N> &data,
    DataSaver &saver,
    std::array<GUI::Plotting::CAHA<PLOT_H, PLOT_P>, N> &rawplots,
    GUI::Plotting::ListOfLines<PLOT_H, PLOT_P> &hkplots) noexcept {
  bool allnew = false;
  bool quit = false;
  Chopper::ChopperPos last;
  const std::string med_str = " Median ";
  const std::string med_noise_str = " Median Noise ";
  const std::string raw_cold_str = " Raw Cold ";
  const std::string raw_hot_str = " Raw Hot ";
  const std::string raw_target_str = " Raw Target ";
  std::map<std::string, double> hk_data_longterm;
  std::vector<std::vector<double>> last_data;
  std::map<std::string, double> hk_data;
  std::map<std::string, double> frontend_data;
  std::array<std::vector<std::vector<float>>, N> backends_data;
  std::array<std::string, N> backend_names;

  for (std::size_t i = 0; i < N; i++) {
    backend_names[i] = spectrometer_ctrls.backends[i].name;
    data[i] = Data(spectrometer_ctrls.backends[i].f);
    data[i].newdata.store(false);
  }

  if (rawplots.size() not_eq N) std::terminate();

wait:
  Sleep(0.1);

loop:
  allnew = housekeeping_ctrl.newdata.load() and frontend_ctrl.newdata.load() and
           std::all_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                       [](auto &x) { return x.newdata.load(); });
  quit = housekeeping_ctrl.quit.load() and frontend_ctrl.quit.load() and
         std::all_of(spectrometer_ctrls.backends.cbegin(), spectrometer_ctrls.backends.cend(),
                     [](auto &x) { return x.quit.load(); });

  if (quit) goto stop;
  if (not allnew) goto wait;

  last = chopper_ctrl.lasttarget;
  hk_data = housekeeping_ctrl.data;
  frontend_data = frontend_ctrl.data;
  for (std::size_t i = 0; i < N; i++) backends_data[i] = spectrometer_ctrls.backends[i].d;

  chopper_ctrl.newdata.store(false);
  housekeeping_ctrl.newdata.store(false);
  frontend_ctrl.newdata.store(false);
  for (auto &spec : spectrometer_ctrls.backends) spec.newdata.store(false);

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
      last_data = data[i].last_calib;
      for (std::size_t j=0; j<last_data.size(); j++) {
        if (last_data[j].size()) {
          std::sort(last_data[j].begin(), last_data[j].end());
          if (last_data.size() > 1) {
            hk_data_longterm[backend_names[i] + med_str + std::to_string(j)] = last_data[j][last_data[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + med_str] = last_data[j][last_data[j].size()/2];
          }
        }
      }
    }
    if (data[i].has_noise) {
      last_data = data[i].last_noise;
      for (std::size_t j=0; j<last_data.size(); j++) {
        if (last_data[j].size()) {
          std::sort(last_data[j].begin(), last_data[j].end());
          if (last_data.size() > 1) {
            hk_data_longterm[backend_names[i] + med_noise_str + std::to_string(j)] = last_data[j][last_data[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + med_noise_str] = last_data[j][last_data[j].size()/2];
          }
        }
      }
    }
    if (data[i].has_cold) {
      last_data = data[i].last_cold;
      for (std::size_t j=0; j<last_data.size(); j++) {
        if (last_data[j].size()) {
          std::sort(last_data[j].begin(), last_data[j].end());
          if (last_data.size() > 1) {
            hk_data_longterm[backend_names[i] + raw_cold_str + std::to_string(j)] = last_data[j][last_data[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + raw_cold_str] = last_data[j][last_data[j].size()/2];
          }
        }
      }
    }
    if (data[i].has_hot) {
      last_data = data[i].last_hot;
      for (std::size_t j=0; j<last_data.size(); j++) {
        if (last_data[j].size()) {
          std::sort(last_data[j].begin(), last_data[j].end());
          if (last_data.size() > 1) {
            hk_data_longterm[backend_names[i] + raw_hot_str + std::to_string(j)] = last_data[j][last_data[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + raw_hot_str] = last_data[j][last_data[j].size()/2];
          }
        }
      }
    }
    if (data[i].has_target) {
      last_data = data[i].last_target;
      for (std::size_t j=0; j<last_data.size(); j++) {
        if (last_data[j].size()) {
          std::sort(last_data[j].begin(), last_data[j].end());
          if (last_data.size() > 1) {
            hk_data_longterm[backend_names[i] + raw_target_str + std::to_string(j)] = last_data[j][last_data[j].size()/2];
          } else {
            hk_data_longterm[backend_names[i] + raw_target_str] = last_data[j][last_data[j].size()/2];
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
bool AllErrors(GUI::Config& config,
               Chopper::Controller &chopper_ctrl,
               Wobbler::Controller &wobbler_ctrl,
               Housekeeping::Controller &housekeeping_ctrl,
               Frontend::Controller &frontend_ctrl,
               Spectrometer::Controllers &spectrometer_ctrls);
}  // namespace Instrument

#endif  // instrument_h
