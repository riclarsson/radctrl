#include "backend_affts.h"

namespace Instrument::Spectrometer {
AFFTS::AFFTS(const std::string &n, const std::filesystem::path &path)
    : mname(n), manual(false), error_found(false), error("") {
  if (not std::filesystem::exists(path)) {
    std::ostringstream os;
    os << "Cannot find AFFTS python file at:\n\t" << path << '\n';
    throw std::runtime_error(os.str());
  }
  py::eval_file(path.c_str());
  PyClass = Python::ClassInterface("FW");
}
void AFFTS::startup(const std::string &host, int tcp, int udp,
              Eigen::Ref<Eigen::MatrixXd> freq_limits,
              Eigen::Ref<Eigen::VectorXi> freq_counts,
              int integration_time_microsecs, int blank_time_microsecs,
              bool mirror) {
  PyInst = Python::ClassInstance{
    PyClass(host, tcp, udp, freq_limits / 1e6, freq_counts,
            integration_time_microsecs, blank_time_microsecs, mirror)};
            initfun = Python::Function{PyInst("init")};
            shutdown = Python::Function{PyInst("close")};
            runfun = Python::Function{PyInst("run")};
            download = Python::Function{PyInst("get_data")};
            get_data_copy = Python::Function{PyInst("copy")};
            
            // Fill our data with zeroes
            data.resize(freq_counts.size());
            for (long i = 0; i < freq_counts.size(); i++)
              data[i] = std::vector<float>(freq_counts[i], 0);
}
  
void AFFTS::get_data(int i) {
  download(i);

  // Copy to C++
  internal_data = get_data_copy(i);
  std::vector<float> copy = internal_data.toVector<float>();

  // Assign to correct position
  size_t ind = 0;
  for (std::vector<float> &board : data) {
    for (float &val : board) {
      val = copy[ind];
      ind++;
    }
  }
}
}
