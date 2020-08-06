#ifndef instrument_h
#define instrument_h

#include <algorithm>
#include <array>
#include <exception>
#include <functional>
#include <thread>
#include <vector>

#include "gui.h"
#include "timeclass.h"

namespace Instrument {
template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Backends, typename BackendControllers>
void InitAll (Chopper& chop, ChopperController& chopper_ctrl,
              Wobbler& wob, WobblerController& wobbler_ctrl,
              Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
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
}

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Backends, typename BackendControllers>
void CloseAll (Chopper& chop, ChopperController& chopper_ctrl,
               Wobbler& wob, WobblerController& wobbler_ctrl,
               Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
               Backends& backends, BackendControllers& backend_ctrls) {
  hk.close();
  housekeeping_ctrl.init = false;
  
  chop.close();
  chopper_ctrl.init = false;
  
  wob.close();
  wobbler_ctrl.init = false;
  
  for (size_t i=0; i<backends.N; i++) {
    backends.close(i);
    backend_ctrls[i].init = false;
  }
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename BackendControllers>
void ReadyRunAll (ChopperController& chopper_ctrl,
                  WobblerController& wobbler_ctrl,
                  HousekeepingController& housekeeping_ctrl,
                  BackendControllers& backend_ctrls) {
  housekeeping_ctrl.run = true;
  chopper_ctrl.run = true;
  wobbler_ctrl.run = true;
  for (auto& ctrl: backend_ctrls) ctrl.run = true;
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename BackendControllers>
void UnreadyRunAll (ChopperController& chopper_ctrl,
                    WobblerController& wobbler_ctrl,
                    HousekeepingController& housekeeping_ctrl,
                    BackendControllers& backend_ctrls) {
  housekeeping_ctrl.run = false;
  chopper_ctrl.run = false;
  wobbler_ctrl.run = false;
  for (auto& ctrl: backend_ctrls) ctrl.run = false;
}

template <typename ChopperController,
          typename WobblerController,
          typename HousekeepingController,
          typename BackendControllers>
void QuitAll (ChopperController& chopper_ctrl,
              WobblerController& wobbler_ctrl,
              HousekeepingController& housekeeping_ctrl,
              BackendControllers& backend_ctrls) {
  housekeeping_ctrl.quit = true;
  chopper_ctrl.quit = true;
  wobbler_ctrl.quit = true;
  for (auto& ctrl: backend_ctrls) ctrl.quit = true;
}

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Backends, typename BackendControllers>
void AllInformation (Chopper& chop, ChopperController& chopper_ctrl,
                     Wobbler& wob, WobblerController& wobbler_ctrl,
                     Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
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
      auto cold = housekeeping_ctrl.data.find(std::string{"Cold Load Temperature"});
      if (cold == housekeeping_ctrl.data.end())
        ImGui::Text("Cold Target [K]: UNKNOWN");
      else
        ImGui::Text("Cold Target [K]: %.3lf", std::stod(cold->second));
      
      ImGui::SameLine();
      ImGui::SetCursorPosX(x0 + dx * 41);
      auto hot = housekeeping_ctrl.data.find(std::string{"Hot Load Temperature"});
      if (hot == housekeeping_ctrl.data.end())
        ImGui::Text("Hot Target [K]: UNKNOWN");
      else
        ImGui::Text("Hot Target [K]: %.3lf", std::stod(hot->second));
      
      ImGui::Text("Initialized: ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.init);
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.init);
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.init);
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].init);
      }
      
      ImGui::Text("Running:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.run);
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.run);
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.run);
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].run);
      }
      
      ImGui::Text("Waiting:     ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.waiting);
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.waiting);
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.waiting);
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].waiting);
      }
      
      ImGui::Text("Operating:   ");
      ImGui::SameLine();
      ImGui::RadioButton("Chopper", chopper_ctrl.operating);
      ImGui::SameLine();
      ImGui::RadioButton("Wobbler", wobbler_ctrl.operating);
      ImGui::SameLine();
      ImGui::RadioButton("Housekeeping", housekeeping_ctrl.operating);
      for (size_t i=0; i<backends.N; i++) {
        ImGui::SameLine();
        ImGui::RadioButton(backends.name(i).c_str(), backend_ctrls[i].operating);
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
      ImGui::EndTabItem();
    }
    
    ImGui::EndTabBar();
  }
}

template <typename Chopper, typename ChopperController,
          typename Wobbler, typename WobblerController,
          typename Housekeeping, typename HousekeepingController,
          typename Backends, typename BackendControllers>
std::vector<std::string> RunExperiment (Chopper& chop, ChopperController& chopper_ctrl,
                                        Wobbler& wob, WobblerController& wobbler_ctrl,
                                        Housekeeping& hk, HousekeepingController& housekeeping_ctrl,
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
Sleep(0.5);

loop:
init = chopper_ctrl.init and wobbler_ctrl.init and housekeeping_ctrl.init and
  std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.init;});
run = chopper_ctrl.run and wobbler_ctrl.run and housekeeping_ctrl.run and
  std::all_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.run;});
error = chopper_ctrl.error or wobbler_ctrl.error or housekeeping_ctrl.error or
  std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.error;});
quit = chopper_ctrl.quit or wobbler_ctrl.quit or housekeeping_ctrl.quit or
  std::any_of(backend_ctrls.cbegin(), backend_ctrls.cend(), [](auto& x){return x.quit;});

std::cout << std::boolalpha << init << ' '  << run << ' ' << error << ' ' << quit << '\n';
  
// Quit if any instrument wants it
if (quit) goto stop;

// Wait while not running or finding errors
if (not init or not run or error) goto wait;

// When are we
now = Time();

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

// Store the measurements
for (size_t i=0; i<backends.N; i++)
  backend_ctrls[i].d = backends.datavec(i);
housekeeping_ctrl.data = hk.data();

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
}

#endif  // instrument_h
