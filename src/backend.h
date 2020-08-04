#ifndef backend_h
#define backend_h

#include <atomic>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "gui.h"
#include "python_interface.h"

namespace Instrument {
namespace Spectrometer {
ENUMCLASS(Type, unsigned char,
          CTS210,
          CTS40,
          SWICTS,
          AFFTS,
          XFFTS,
          dFFTS
         )

struct GUI {
  bool init;
  bool error;
  bool mirror;
  Type spectrometer;
  
  std::string host;
  int tcp_port;
  int udp_port;
  std::vector<std::vector<double>> f;
  std::vector<std::vector<double>> d;
  
  int integration_time_microsecs;
  int blank_time_microsecs;
};

template <typename First, typename ... Others>
struct Backends {
  static constexpr size_t N = 1 + sizeof...(Others);
  
  First first;
  Backends<Others...> others;
  
  Backends(const First& f, const Others& ... o) noexcept : first(f), others(o...) {}
  
  void run() {first.run(); if constexpr (N > 2) others.run();}
  void init() {first.init(); if constexpr (N > 2) others.init();}
  void close() {first.close(); if constexpr (N > 2) others.close();}
  void startup(std::string& host,
               Eigen::Ref<Eigen::MatrixXd> freqs,
               int tcp,
               int udp,
               Eigen::Ref<Eigen::MatrixXi> channels,
               int micorsecs_integration,
               int micorsecs_blank,
               bool reverse_data) {
    first.startup(host, freqs, tcp, udp, channels, micorsecs_integration, micorsecs_blank, reverse_data);
    if constexpr (N > 2)
      others.startup(host, freqs, tcp, udp, channels, micorsecs_integration, micorsecs_blank, reverse_data);
  }
  bool newdata(int i) {if (not i) return first.newdata(); else if (i > 0) return others.newdata(i-1); else std::exit(1);}
  std::vector<std::vector<double>> data(int i) {if (not i) return first.datavec(); else if (i > 0) return others.data(i-1); else std::exit(1);}
};

template <typename ... Spectrometers>
void GuiSetup(Backends<Spectrometers...>& spectrometers, std::array<GUI, Backends<Spectrometers...>::N>& ctrls) {
}

class Dummy {
  bool manual;
  bool error_found;
  std::string error;
  std::string name;
  std::vector<std::vector<double>> data;
  std::atomic<bool> newdata;

public:
  template <typename ... Whatever> constexpr Dummy(Whatever...) : manual(false), error_found(false),
  error(""), name("Dummy"), data(2, std::vector<double>{1000, 0}), newdata(false) {
    for (int i=0; i<1000; i++) {
      data[0][i] = std::cos(3.14*2*i/1000.);
      data[1][i] = std::sin(3.14*2*i/1000.);
    }
  }
  
  void startup(std::string&, Eigen::Ref<Eigen::MatrixXd>, int, int, Eigen::Ref<Eigen::MatrixXi>, int, int, bool) {}
  void init(bool manual_init) {manual=manual_init; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void close() {}
  void run() {}
  void get_data(int) {
    for (int i=0; i<1000; i++) {
      data[0][i] = std::cos(std::acos(data[0][i])+0.0001);
      data[1][i] = std::sin(std::asin(data[1][i])+0.0001);
    }
    newdata.store(true);
  }
  std::vector<std::vector<double>> datavec() {std::vector<std::vector<double>> out{data}; newdata.store(false); return out;}
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // Dummy

}  // Spectrometer
}  // Instrument

#endif  // backend_h
