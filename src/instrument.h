#ifndef instrument_h
#define instrument_h

#include <algorithm>
#include <array>
#include <atomic>
#include <exception>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "chopper.h"
#include "gui.h"
#include "backend.h"

namespace Instrument {
struct Data {
  // Chopper Pointing
  Chopper::ChopperPos target;
  
  // Frequency grids of the instrument
  std::vector<std::vector<float>> f;
  
  // Last raw data of the instrument
  std::vector<std::vector<float>> last_target;
  std::vector<std::vector<float>> last_cold;
  std::vector<std::vector<float>> last_hot;
  
  // Last calibrated data of the instrument
  std::vector<std::vector<float>> last_calib;
  
  // Last calibrated noise temperature of the instrument
  std::vector<std::vector<float>> last_noise;
  
  // Running average of the raw data
  std::vector<std::vector<float>> avg_target;
  std::vector<std::vector<float>> avg_cold;
  std::vector<std::vector<float>> avg_hot;
  
  // Average calibrated data of the instrument
  std::vector<std::vector<float>> avg_calib;
  
  // Average calibrated noise temperature of the instrument
  std::vector<std::vector<float>> avg_noise;
  
  // Housekeeping data
  Time now;
  float tcold;
  float thot;
  float integration_time_in_seconds;
  float blank_time_in_seconds;
  
  // User selections
  size_t num_avg;  // Number to average
  size_t avg_count;  //  Count of current average
  size_t spectral_average;  // Count of nearby channels to average
  float freq_scaling;  // Scale of frequency (defaults to Hz)
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
    inst.startup(/*FIXME????FIXME*/);
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
  
  for (size_t i=0; i<backends.N; i++) {
    backends.startup(i, backend_ctrls[i].host, backend_ctrls[i].tcp_port, backend_ctrls[i].udp_port,
                     backend_ctrls[i].freq_limits, backend_ctrls[i].freq_counts,
                     backend_ctrls[i].integration_time_microsecs, backend_ctrls[i].blank_time_microsecs,
                     backend_ctrls[i].mirror);
    backends.init(i, false);
    if (backends.has_error(i))
      backend_ctrls[i].error = true;
    else
      backend_ctrls[i].init = true;
  }
  
  hkinit.join();
  wobinit.join();
  chopinit.join();
  frontendinit.join();
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
                  BackendControllers& backend_ctrls) {
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
                    BackendControllers& backend_ctrls) {
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
              BackendControllers& backend_ctrls) {
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
                     Backends& backends, BackendControllers& backend_ctrls) {
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
        ImGui::Text("Cold Target [K]: %.3lf", std::stod(cold->second));
      
      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 41);
      auto hot = housekeeping_ctrl.data.find("Hot Load Temperature");
      if (hot == housekeeping_ctrl.data.end())
        ImGui::Text("Hot Target [K]: UNKNOWN");
      else
        ImGui::Text("Hot Target [K]: %.3lf", std::stod(hot->second));
      
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
        ImGui::Text("%s: %s", data.first.c_str(), data.second.c_str());
        local_dx += dx * (data.first.size() + data.second.size()) * 0.7f;
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
        ImGui::Text("%s: %s", data.first.c_str(), data.second.c_str());
        local_dx += dx * (data.first.size() + data.second.size()) * 0.7f;
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
                                        Backends& backends, BackendControllers& backend_ctrls) {
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

std::cout << std::boolalpha << init << ' '  << run << ' ' << error << ' ' << quit << '\n';
  
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
typename HousekeepingController, typename FrontendController>
void ExchangeData(std::array<Spectrometer::Controller, N>& backend_ctrls,
                  ChopperController& chopper_ctrl,
                  HousekeepingController& housekeeping_ctrl,
                  FrontendController& frontend_ctrl,
                  std::array<Data, N>& data) {
bool allnew = false;
bool quit = false;
Chopper::ChopperPos last;
std::map<std::string, std::string> hk_data;
std::map<std::string, std::string> frontend_data;
std::array<std::vector<std::vector<float>>, N> backend_data;

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
for (size_t i=0; i<N; i++) backend_data[i] = backend_ctrls[i].d;

chopper_ctrl.newdata.store(false);
housekeeping_ctrl.newdata.store(false);
frontend_ctrl.newdata.store(false);
for (size_t i=0; i<N; i++) backend_ctrls[i].newdata.store(false);

// FIXME: save data to file and update plotting tools here

goto loop;
stop: {}
}
}  // Instrument

#endif  // instrument_h
