#ifndef backend_xffts_h
#define backend_xffts_h

#include <filesystem>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "../python_interface.h"

namespace Instrument::Spectrometer {
class XFFTS {
  std::string mname;
  bool manual;
  bool error_found;
  std::string error;
  std::vector<std::vector<float>> data;

  Python::ClassInterface PyClass;
  Python::ClassInstance PyInst;
  Python::Function initfun;
  Python::Function shutdown;
  Python::Function runfun;
  Python::Function download;
  Python::Function get_data_copy;

  Python::Object<Python::Type::NumpyVector> internal_data;

 public:
  XFFTS(const std::string &n, const std::filesystem::path &path);

  void startup(const std::string &host, int tcp, int udp,
               Eigen::Ref<Eigen::MatrixXd> freq_limits,
               Eigen::Ref<Eigen::VectorXi> freq_counts,
               int integration_time_microsecs, int blank_time_microsecs,
               bool mirror);

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i);

  bool manual_run() { return manual; }
  
  const std::string &error_string() const { return error; }
  
  bool has_error() { return error_found; }
  
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // XFFTS

}

#endif  // backend_xffts_h
