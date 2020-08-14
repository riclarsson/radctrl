#ifndef housekeeping_h
#define housekeeping_h

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "asio_interface.h"
#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Housekeeping {
struct Controller {
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;
  
  std::string dev;
  int baudrate;
  
  std::map<std::string, double> data;
  
  Controller(const std::string& d, int b) noexcept : init(false), error(false), quit(false), run(false), operating(false), waiting(false), newdata(false),
  dev(d), baudrate(b), data() {}
};

template <typename Housekeeping>
void GuiSetup(Housekeeping& hk, Controller& ctrl, const std::vector<std::string>& devs) {
  bool change=false;
  bool manual=false;
  if (not ctrl.init) {
    if (ImGui::Button(" Manual Init ")) {
      change = true;
      manual = true;
      ctrl.init = true;
    } ImGui::SameLine();
    if (ImGui::Button(" Automatic Init ")) {
      change = true;
      manual = false;
      ctrl.init = true;
    } ImGui::SameLine();
    ImGui::Text(" Close ");
  } else  {
    ImGui::Text(" Manual Init ");
    ImGui::SameLine();
    ImGui::Text(" Automatic Init ");
    ImGui::SameLine();
    if (ImGui::Button(" Close ")) {
      change = true;
      ctrl.init = false;
    }
  }
  
  if (change) {
    if (ctrl.init) {
      hk.startup(ctrl.dev, ctrl.baudrate);
      hk.init(manual);
    } else {
      hk.close();
    }
  }
  
  ImGui::PushItemWidth(120.0f);
  if (auto dev = ctrl.dev.c_str(); ImGui::BeginCombo("Device", dev)) {
    bool newselection=false;
    for (auto& d: devs) {
      auto dev2 = d.c_str();
      bool is_selected = (dev == dev2); // You can store your selection however you want, outside or inside your objects
      if (ImGui::Selectable(dev2, is_selected)) {
        newselection = true;
        dev = dev2;
      }
    }
    ImGui::EndCombo();
    
    if (not ctrl.init and newselection)
      ctrl.dev = dev;
  }
  
  ImGui::PushItemWidth(100.0f);
  ImGui::SameLine();
  if (ctrl.init)
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_ReadOnly);
  else
    ImGui::InputInt("Baud Rate", &ctrl.baudrate, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll);
  ImGui::PopItemWidth();
  
  if (hk.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}

class Dummy {
  bool manual;
  bool error_found;
  bool new_data;
  std::map<std::string, double> database;
  std::string error;
  
public:
  using DataType = std::map<std::string, double>;
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), error_found(false), new_data(false),
  database({
    {"Cold Load Temperature", 18},
    {"Hot Load Temperature", 297}
  }), error("") {}
  void startup(const std::string&, int) {}
  void init(bool manual_press=false) {manual=manual_press; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void run() {}
  void close() {}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  void get_data() const {}
  DataType data() const {return database;}
};  // Dummy

class AgilentPython {
  bool manual;
  bool error_found;
  bool new_data;
  std::string error;
  
  std::map<std::string, double> database;
  
  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function runfun;
  Python::Function download;
  
  Python::Object<Python::Type::Dict> status;
  
public:
  using DataType = std::map<std::string, double>;
  AgilentPython(const std::filesystem::path& path) : manual(false), error_found(false), new_data(false), error("")  {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find Agilent python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"Agilent34970A"};
  };
  
  void startup(const std::string& dev, int baud) {
    PyInst = Python::ClassInstance{PyClass(dev, baud)};
    initfun = Python::Function{PyInst("init")};
    shutdown = Python::Function{PyInst("close")};
    runfun = Python::Function{PyInst("run")};
    download = Python::Function{PyInst("get_status_as_dict")};
  }
  void init(bool manual_press=false) {manual=manual_press; initfun();}
  void run() {runfun();}
  void close() {}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  void get_data() {
    status=download();
    auto keys = status.keysDict();
    for (auto& key: keys)
      database[key] = status.fromDict<Python::Type::Double>(key).toDouble();
  }
  DataType data() const {return database;}
};  // AgilentPython

template <size_t N>
constexpr double linear_interpolation (const std::array<double, N>& x, const std::array<double, N>& y, double t) {
  int i=1;
  while (i<N-1 and t<x[i]) i++;
  return y[i-1] + y[i] * (t - x[i-1]) / (x[i] - x[i-1]);
}

class Agilent34970A {
  bool manual;
  bool error_found;
  bool new_data;
  std::string error;
  
  std::map<std::string, double> database;
  
  Network::Serial port;
  
  std::string results;
  
public:
  using DataType = std::map<std::string, double>;
  Agilent34970A() : manual(false), error_found(false), new_data(false), error("")  {
  };
  
  void startup(const std::string& dev, int baud) {
    port.set_baudrate(baud);
    port.open(dev);
  }
  void init(bool manual_press=false) {
    manual=manual_press;
    port.write("\x03\n");
    port.write("*IDN?\n");
    std::string name = port.readline();
    
    port.write("SYSTEM:CPON 100\n");
    port.write("ROUTE:CHAN:DELAY:AUTO ON, (@101:120)\n");
    port.write("CALC:SCALE:STATE OFF, (@101:120)\n");
    
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@101)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@102)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@103)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@104)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@105)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@106)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@107)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@108)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@109)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@111)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@112)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@113)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@114)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@115)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@116)\n");
    port.write("SENSE:FUNCTION \"RESISTANCE\", (@117)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@118)\n");
    port.write("SENSE:FUNCTION \"VOLTAGE:DC\", (@119)\n");
    
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@101)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@102)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@103)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@104)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@105)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@106)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@107)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@108)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@109)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@111)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@112)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@113)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@114)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@115)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@116)\n");
    port.write("SENSE:RESISTANCE:NPLC 10, (@117)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@118)\n");
    port.write("SENSE:VOLTAGE:DC:NPLC 10, (@119)\n");
    
    port.write("ROUTE:SCAN (@101,102,103,104,105,106,107,108,109,114,115,116,117,118,119)\n");
  }
  void run() {
    port.write("READ?\n");
    results = port.readline();
  }
  void close() {port.close();}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
  void get_data() {
    size_t pos = 0;
    std::vector<double> vals;
    while (pos < results.size()) {
      auto end = results.find(',', pos);
      
      if (end >= pos or end == std::string::npos) break;
      
      vals.push_back(std::stod(results.substr(pos, end-pos)));
      pos = end + 1;
    }
    
    database ={{"Cold Load Temperature", vals[1]},
               {"Hot Load Temperature", vals[2]},
               {"HEMT Temperature", vals[0]},
               {"Room Temperature 1", vals[6]},
               {"Room Temperature 2", vals[12]},
               {"CTS 1 Temperature 1", vals[3]},
               {"CTS 1 Temperature 2", vals[4]},
               {"CTS 2 Temperature 1", vals[9]},
               {"CTS 2 Temperature 2", vals[10]}};
  }
  DataType data() const {return database;}
};  // AgilentPython

}  // Housekeeping
}  // Instrument

#endif  // housekeeping_h
