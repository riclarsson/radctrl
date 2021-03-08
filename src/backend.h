#ifndef backend_h
#define backend_h

#include <Eigen/Core>
#include <algorithm>
#include <atomic>
#include <string>
#include <variant>
#include <vector>

#include "file.h"
#include "gui.h"
#include "mathhelpers.h"
#include "python_interface.h"
#include "timeclass.h"

// #if HAS_ACS2020
//   #include "ACS2020-FM/acs_2020.h"
// #endif

#include "backend/backend_affts.h"
#include "backend/backend_xffts.h"
#include "backend/backend_dffts.h"
#include "backend/backend_rcts104.h"
#include "backend/backend_swicts.h"
#include "backend/backend_pc104.h"

namespace Instrument::Spectrometer {
class Dummy {
  std::string mname;
  bool manual;
  bool error_found;
  std::string error;
  std::vector<std::vector<float>> data;
  std::vector<float> dummy_x;

 public:
  Dummy(const std::string &n="Dummy")  // default name so default-constructible
      : mname(n),
        manual(false),
        error_found(false),
        error(""),
        data(2, std::vector<float>(1000)),
        dummy_x(1000) {
    for (int i = 0; i < 1000; i++) {
      dummy_x[i] = 4 * 3.14 * i / 1000.;
      data[0][i] = 5 + std::cos(dummy_x[i]);
      data[1][i] = 5 + std::sin(dummy_x[i]);
    }
  }

  void startup(const std::string &, int, int, Eigen::Ref<Eigen::MatrixXd>,
               Eigen::Ref<Eigen::VectorXi>, int, int, bool) {}
  void init(bool manual_init) {
    manual = manual_init;
    if (not manual) {
      error = "Must be manual, is dummy";
      error_found = true;
    }
  }
  
  void close() {}
  
  void run() {}
  
  std::vector<std::vector<float>> datavec() { return data; }
  
  std::string name() const { return mname; }

  void get_data(int) {
    for (int i = 0; i < 1000; i++) {
      dummy_x[i] += 0.05f;
      data[0][i] = 5 + std::cos(dummy_x[i]);
      data[1][i] = 5 + std::sin(dummy_x[i]);
    }
    Sleep(0.1);
  }
  
  bool manual_run() { return manual; }
  
  const std::string &error_string() const { return error; }
  
  bool has_error() { return error_found; }
  
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // Dummy

typedef std::variant<Dummy, AFFTS, XFFTS, dFFTS, RCTS104, PC104, SWICTS> Interface;

struct SingleController {
  Interface backend;
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;
  
  std::string host;
  int tcp_port;
  int udp_port;
  Eigen::MatrixXd freq_limits;
  Eigen::VectorXi freq_counts;
  int integration_time_microsecs;
  int blank_time_microsecs;
  bool mirror;
  
  std::string name;
  std::vector<std::vector<double>> f;
  std::vector<std::vector<float>> d;
  
  void startup();
  
  void initialize(bool manual);
  
  void close();
  
  bool has_error();
  
  void run_machine();
  
  void get_data(int pos);
  
  void store_data();
  
  const std::string& error_string() const;
  
  void delete_error();

  SingleController(Interface && iface, const std::string &controller_name, const std::string &h, int tcp,
                   int udp, Eigen::MatrixXd fl, Eigen::VectorXi fc, int intus,
                   int blaus, bool reverse);

  SingleController(Interface && iface, const std::string &controller_name,
                   const std::filesystem::path &path, int intus, int blaus);
  
  SingleController(SingleController && sc) noexcept;
  
  SingleController(const SingleController & sc) noexcept;
};

struct Controllers {
  std::vector<SingleController> backends;
  Time now;
  
  Controllers(std::vector<SingleController> && sctrl) : backends(sctrl), now() {}
  
  bool has_any_errors();
  
  std::size_t size() noexcept {return backends.size();}
};

void GuiSetup(Controllers &ctrls);
}  // namespace Instrument::Spectrometer

#endif  // backend_h
