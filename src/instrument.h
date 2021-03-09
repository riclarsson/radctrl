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
    constexpr std::size_t one = 1;
    mtx.lock();
    num_to_avg = std::max(one, num);
    if (avg_count > num_to_avg) {
      avg_count = num_to_avg;
    }
    mtx.unlock();
  }

  Data() noexcept = default;
  
  Data(const Data &other) noexcept;
  
  Data &operator=(const Data &other) noexcept;

  Data(const std::vector<std::vector<double>> &freq_grid) noexcept;

  void update(Chopper::ChopperPos thistarget, double tc, double th,
              const std::vector<std::vector<float>> &data) noexcept;
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

  void save(const Chopper::ChopperPos &last,
            const std::map<std::string, double> &hk_data,
            const std::map<std::string, double> &frontend_data,
            const std::vector<std::vector<std::vector<float>>> &backends_data,
            const std::vector<std::string> &backend_names) noexcept;
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

void AllInformation(Chopper::Controller &chopper_ctrl,
                    Wobbler::Controller &wobbler_ctrl,
                    Housekeeping::Controller &housekeeping_ctrl,
                    Frontend::Controller &frontend_ctrl,
                    Spectrometer::Controllers &spectrometer_ctrls, std::vector<Instrument::Data>& backend_data) noexcept;

std::vector<std::string> RunExperiment(
    Chopper::Controller &chopper_ctrl,
    Wobbler::Controller &wobbler_ctrl,
    Housekeeping::Controller &housekeeping_ctrl,
    Frontend::Controller &frontend_ctrl,
    Spectrometer::Controllers &spectrometer_ctrls);

void ExchangeData(
    Spectrometer::Controllers &spectrometer_ctrls,
    Chopper::Controller &chopper_ctrl, Housekeeping::Controller &housekeeping_ctrl,
    Frontend::Controller &frontend_ctrl, std::vector<Data> &data,
    DataSaver &saver,
    std::vector<GUI::Plotting::CAHA76> &rawplots,
    GUI::Plotting::ListOfLines76 &hkplots) noexcept;

/*! Handle errors.  If it returns false, errors cannot be handled */
bool AllErrors(GUI::Config& config,
               Chopper::Controller &chopper_ctrl,
               Wobbler::Controller &wobbler_ctrl,
               Housekeeping::Controller &housekeeping_ctrl,
               Frontend::Controller &frontend_ctrl,
               Spectrometer::Controllers &spectrometer_ctrls);

std::vector<Instrument::Data> init_spectrometer_data(Spectrometer::Controllers &spectrometer_ctrls);

std::vector<GUI::Plotting::CAHA76> init_plotting_data(Spectrometer::Controllers &spectrometer_ctrls);
}  // namespace Instrument

#endif  // instrument_h
