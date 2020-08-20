#ifndef backend_h
#define backend_h

#include <Eigen/Core>
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

#include "file.h"
#include "gui.h"
#include "mathhelpers.h"
#include "python_interface.h"
#include "timeclass.h"

namespace Instrument {
namespace Spectrometer {
struct Controller {
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

  Controller(const std::string& controller_name, const std::string& h, int tcp,
             int udp, Eigen::MatrixXd fl, Eigen::VectorXi fc, int intus,
             int blaus, bool reverse)
      : init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        host(h),
        tcp_port(tcp),
        udp_port(udp),
        freq_limits(fl),
        freq_counts(fc),
        integration_time_microsecs(intus),
        blank_time_microsecs(blaus),
        mirror(reverse),
        name(controller_name) {
    const size_t N = freq_counts.size();
    f.resize(N);
    d.resize(N);
    for (size_t i = 0; i < N; i++) {
      const size_t n = freq_counts[i];
      f[i] = linspace(freq_limits(i, 0), freq_limits(i, 1), n);
      d[i] = std::vector<float>(n, 0);
    }
  }

  Controller(const std::string& controller_name,
             const std::filesystem::path& path, int intus, int blaus)
      : init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        integration_time_microsecs(intus),
        blank_time_microsecs(blaus),
        name(controller_name) {
    File::File<File::Operation::Read, File::Type::Xml> file{path};
    std::string name = controller_name;
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
    auto child = file.get_child(name);
    host = file.get_attribute("host").as_string();
    tcp_port = file.get_attribute("tcp").as_int();
    udp_port = file.get_attribute("udp").as_int();

    std::istringstream is(child.text().as_string());
    const int N = file.get_attribute("Nboards").as_int();
    freq_limits = Eigen::MatrixXd(N, 2);
    freq_counts = Eigen::VectorXi(N);
    for (int i = 0; i < N; i++)
      is >> freq_counts[i] >> freq_limits(i, 0) >> freq_limits(i, 1);
    mirror = file.get_attribute("mirror").as_bool();

    f.resize(N);
    d.resize(N);
    for (int i = 0; i < N; i++) {
      const int n = freq_counts[i];
      f[i] = linspace(freq_limits(i, 0), freq_limits(i, 1), n);
      d[i] = std::vector<float>(n, 0);
    }
  }
};

template <size_t N>
std::vector<std::array<GUI::Plotting::Frame, 4>> PlotFrame(
    const std::array<Controller, N>& ctrls) {
  std::vector<std::array<GUI::Plotting::Frame, 4>> tmp;
  for (size_t i = 0; i < N; i++) {
    std::vector<GUI::Plotting::Line> raw;
    std::vector<GUI::Plotting::Line> noi;
    std::vector<GUI::Plotting::Line> avg;
    std::vector<GUI::Plotting::Line> itg;
    for (size_t j = 0; j < ctrls[i].f.size(); j++) {
      raw.push_back(GUI::Plotting::Line(
          std::string{"Cold "} + std::to_string(j), ctrls[i].f[j],
          GUI::Plotting::Data{ctrls[i].f[j].size()}));
      raw.push_back(GUI::Plotting::Line(
          std::string{"Target "} + std::to_string(j), ctrls[i].f[j],
          GUI::Plotting::Data{ctrls[i].f[j].size()}));
      raw.push_back(GUI::Plotting::Line(
          std::string{"Hot "} + std::to_string(j), ctrls[i].f[j],
          GUI::Plotting::Data{ctrls[i].f[j].size()}));
      noi.push_back(GUI::Plotting::Line(
          std::string{"System Noise "} + std::to_string(j), ctrls[i].f[j],
          GUI::Plotting::Data{ctrls[i].f[j].size()}));
      avg.push_back(GUI::Plotting::Line(
          std::string{"Average Measurement "} + std::to_string(j),
          ctrls[i].f[j], GUI::Plotting::Data{ctrls[i].f[j].size()}));
      itg.push_back(GUI::Plotting::Line(
          std::string{"Last Measurement "} + std::to_string(j), ctrls[i].f[j],
          GUI::Plotting::Data{ctrls[i].f[j].size()}));
    }
    tmp.push_back(std::array<GUI::Plotting::Frame, 4>{
        GUI::Plotting::Frame{"Raw", "Frequency", "Counts", raw},
        GUI::Plotting::Frame{"Noise", "Frequency", "Temperature [K]", noi},
        GUI::Plotting::Frame{"Integration", "Frequency", "Temperature [K]",
                             itg},
        GUI::Plotting::Frame{"Average", "Frequency", "Temperature [K]", avg}});
  }

  return tmp;
}

template <typename... Spectrometers>
struct Backends {
  static constexpr size_t N = sizeof...(Spectrometers);

  std::tuple<Spectrometers...> spectrometers;
  Time now;

  Backends(Spectrometers... s) noexcept : spectrometers(std::make_tuple(s...)) {
    static_assert(N > 0);
  }

  template <size_t i = 0>
  void run(int j) {
    if (i == j)
      std::get<i>(spectrometers).run();
    else if constexpr (i < N - 1)
      run<i + 1>(j);
    else
      std::exit(1);
    now = Time();
  }

  template <size_t i = 0>
  void init(int j, bool manual) {
    if (i == j)
      std::get<i>(spectrometers).init(manual);
    else if constexpr (i < N - 1)
      init<i + 1>(j, manual);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  void startup(int j, const std::string& h, int t, int u,
               Eigen::Ref<Eigen::MatrixXd> fl, Eigen::Ref<Eigen::VectorXi> fc,
               int ius, int bus, bool m) {
    if (i == j)
      std::get<i>(spectrometers).startup(h, t, u, fl, fc, ius, bus, m);
    else if constexpr (i < N - 1)
      startup<i + 1>(j, h, t, u, fl, fc, ius, bus, m);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  void close(int j) {
    if (i == j)
      std::get<i>(spectrometers).close();
    else if constexpr (i < N - 1)
      close<i + 1>(j);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  void delete_error(int j) {
    if (i == j)
      std::get<i>(spectrometers).delete_error();
    else if constexpr (i < N - 1)
      delete_error<i + 1>(j);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  void get_data(int j, int k) {
    if (i == j)
      std::get<i>(spectrometers).get_data(k);
    else if constexpr (i < N - 1)
      get_data<i + 1>(j, k);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  std::string name(int j) {
    if (i == j)
      return std::get<i>(spectrometers).name();
    else if constexpr (i < N - 1)
      return name<i + 1>(j);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  std::string error_string(int j) {
    if (i == j)
      return std::get<i>(spectrometers).error_string();
    else if constexpr (i < N - 1)
      return error_string<i + 1>(j);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  bool has_error(int j) {
    if (i == j)
      return std::get<i>(spectrometers).has_error();
    else if constexpr (i < N - 1)
      return has_error<i + 1>(j);
    else
      std::exit(1);
  }

  template <size_t i = 0>
  bool has_any_errors() {
    if constexpr (i >= N)
      return false;
    else if (has_error<i>(i))
      return true;
    else
      return has_any_errors<i + 1>();
  }

  template <size_t i = 0>
  std::vector<std::vector<float>> datavec(int j) {
    if (i == j)
      return std::get<i>(spectrometers).datavec();
    else if constexpr (i < N - 1)
      return datavec<i + 1>(j);
    else
      std::exit(1);
  }
};

template <typename... Spectrometers>
void GuiSetup(Backends<Spectrometers...>& spectrometers,
              std::array<Controller, sizeof...(Spectrometers)>& ctrls) {
  constexpr size_t N = sizeof...(Spectrometers);

  // Do setup per spectrometer
  if (ImGui::BeginTabBar("Spectrometer List")) {
    for (size_t i = 0; i < N; i++) {
      if (ImGui::BeginTabItem(spectrometers.name(i).c_str())) {
        bool change = false;
        bool manual = false;
        if (not ctrls[i].init) {
          if (ImGui::Button(" Manual Init ")) {
            change = true;
            manual = true;
            ctrls[i].init = true;
          }
          ImGui::SameLine();
          if (ImGui::Button(" Automatic Init ")) {
            change = true;
            manual = false;
            ctrls[i].init = true;
          }
          ImGui::SameLine();
          ImGui::Text(" Close ");
        } else {
          ImGui::Text(" Manual Init ");
          ImGui::SameLine();
          ImGui::Text(" Automatic Init ");
          ImGui::SameLine();
          if (ImGui::Button(" Close ")) {
            change = true;
            ctrls[i].init = false;
          }
        }

        if (change) {
          if (ctrls[i].init) {
            spectrometers.startup(
                i, ctrls[i].host, ctrls[i].tcp_port, ctrls[i].udp_port,
                ctrls[i].freq_limits, ctrls[i].freq_counts,
                ctrls[i].integration_time_microsecs,
                ctrls[i].blank_time_microsecs, ctrls[i].mirror);
            spectrometers.init(i, manual);
          } else {
            spectrometers.close(i);
          }
        }

        if (ctrls[i].init) {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &ctrls[i].host,
                           ImGuiInputTextFlags_CharsNoBlank |
                               ImGuiInputTextFlags_NoHorizontalScroll |
                               ImGuiInputTextFlags_AutoSelectAll |
                               ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &ctrls[i].tcp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &ctrls[i].udp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &ctrls[i].host,
                           ImGuiInputTextFlags_CharsNoBlank |
                               ImGuiInputTextFlags_NoHorizontalScroll |
                               ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &ctrls[i].tcp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &ctrls[i].udp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
        }

        if (ImGui::BeginCombo("Channels", "Channels and Frequency")) {
          ImGui::Text(" ");
          for (long int j = 0; j < ctrls[i].freq_counts.size(); j++) {
            ImGui::Text(
                "\tCount: %i [#]; Low: %.4lf [GHz]; Upper: %.4lf [GHz]\t",
                ctrls[i].freq_counts[j], ctrls[i].freq_limits(j, 0) / 1e9,
                ctrls[i].freq_limits(j, 1) / 1e9);
          }
          ImGui::Text(" ");
          ImGui::EndCombo();
        }

        if (ctrls[i].init) {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &ctrls[i].integration_time_microsecs, 1,
                          100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &ctrls[i].blank_time_microsecs, 1, 10,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          bool m = ctrls[i].mirror;
          ImGui::Checkbox("Mirror", &m);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &ctrls[i].integration_time_microsecs, 1,
                          100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &ctrls[i].blank_time_microsecs, 1, 10,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::Checkbox("Mirror", &ctrls[i].mirror);
          ImGui::PopItemWidth();
        }

        if (spectrometers.has_error(i)) {
          ctrls[i].init = false;
          ctrls[i].error = true;
        } else
          ctrls[i].error = false;
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

class Dummy {
  std::string mname;
  bool manual;
  bool error_found;
  std::string error;
  std::vector<std::vector<float>> data;
  std::vector<float> dummy_x;

 public:
  template <typename... Whatever>
  constexpr Dummy(const std::string& n, Whatever...)
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

  void startup(const std::string&, int, int, Eigen::Ref<Eigen::MatrixXd>,
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
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // Dummy

class AFFTS {
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
  AFFTS(const std::string& n, const std::filesystem::path& path)
      : mname(n), manual(false), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find AFFTS python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"FW"};
  }

  void startup(const std::string& host, int tcp, int udp,
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

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i) {
    download(i);

    // Copy to C++
    internal_data = get_data_copy(i);
    std::vector<float> copy = internal_data.toVector<float>();

    // Assign to correct position
    size_t ind = 0;
    for (std::vector<float>& board : data) {
      for (float& val : board) {
        val = copy[ind];
        ind++;
      }
    }
  }

  bool manual_run() { return manual; }
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // AFFTS

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
  XFFTS(const std::string& n, const std::filesystem::path& path)
      : mname(n), manual(false), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find XFFTS python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"XFW"};
  }

  void startup(const std::string& host, int tcp, int udp,
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

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i) {
    download(i);

    // Copy to C++
    internal_data = get_data_copy(i);
    std::vector<float> copy = internal_data.toVector<float>();

    // Assign to correct position
    size_t ind = 0;
    for (std::vector<float>& board : data) {
      for (float& val : board) {
        val = copy[ind];
        ind++;
      }
    }
  }

  bool manual_run() { return manual; }
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // XFFTS

class RCTS104 {
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
  RCTS104(const std::string& n, const std::filesystem::path& path)
      : mname(n), manual(false), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find XFFTS python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"rcts104"};
  }

  void startup(const std::string& host, int tcp, int udp,
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

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i) {
    download(i);

    // Copy to C++
    internal_data = get_data_copy(i);
    std::vector<float> copy = internal_data.toVector<float>();

    // Assign to correct position
    size_t ind = 0;
    for (std::vector<float>& board : data) {
      for (float& val : board) {
        val = copy[ind];
        ind++;
      }
    }
  }

  bool manual_run() { return manual; }
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // RCTS104

class PC104 {
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
  PC104(const std::string& n, const std::filesystem::path& path)
      : mname(n), manual(false), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find XFFTS python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"pc104"};
  }

  void startup(const std::string& host, int tcp, int udp,
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

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i) {
    download(i);

    // Copy to C++
    internal_data = get_data_copy(i);
    std::vector<float> copy = internal_data.toVector<float>();

    // Assign to correct position
    size_t ind = 0;
    for (std::vector<float>& board : data) {
      for (float& val : board) {
        val = copy[ind];
        ind++;
      }
    }
  }

  bool manual_run() { return manual; }
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // PC104

class SWICTS {
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
  SWICTS(const std::string& n, const std::filesystem::path& path)
      : mname(n), manual(false), error_found(false), error("") {
    if (not std::filesystem::exists(path)) {
      std::ostringstream os;
      os << "Cannot find XFFTS python file at:\n\t" << path << '\n';
      throw std::runtime_error(os.str());
    }
    py::eval_file(path.c_str());
    PyClass = Python::ClassInterface{"swicts"};
  }

  void startup(const std::string& host, int tcp, int udp,
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

  void init(bool manual_init) {
    manual = manual_init;
    initfun();
  }
  void close() { shutdown(); }

  void run() { runfun(); }

  std::vector<std::vector<float>> datavec() { return data; }

  std::string name() const { return mname; }

  void get_data(int i) {
    download(i);

    // Copy to C++
    internal_data = get_data_copy(i);
    std::vector<float> copy = internal_data.toVector<float>();

    // Assign to correct position
    size_t ind = 0;
    for (std::vector<float>& board : data) {
      for (float& val : board) {
        val = copy[ind];
        ind++;
      }
    }
  }

  bool manual_run() { return manual; }
  const std::string& error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // SWICTS

}  // namespace Spectrometer
}  // namespace Instrument

#endif  // backend_h
