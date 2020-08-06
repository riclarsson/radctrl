#ifndef backend_h
#define backend_h

#include <atomic>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"

// FIXME:: Move to better position
std::vector<double> linspace(double s, double e, int count) {
  std::vector<double> ls(count);
  
  if (count == 0) {
    return ls;
  } else if (count == 1) {
    ls.front() = (e + s) / 2;
    return ls;
  } else {
    const double step = (e - s) / (count - 1);
    ls.front() = s;
    ls.back() = e;
    for(int i=1; i < count-1; ++i)
      ls[i] = s + step * i;
    return ls;
  }
}

namespace Instrument {
namespace Spectrometer {
struct GUI {
  bool init;
  bool error;
  bool quit;
  bool run;
  bool operating;
  bool waiting;
  
  std::string host;
  int tcp_port;
  int udp_port;
  Eigen::MatrixXd freq_limits;
  Eigen::VectorXi freq_counts;
  int integration_time_microsecs;
  int blank_time_microsecs;
  bool mirror;
  
  std::vector<std::vector<double>> f;
  std::vector<std::vector<double>> d;
  
  bool newcold, newhot, newatm;
  std::vector<std::vector<double>> cold;
  std::vector<std::vector<double>> hot;
  std::vector<std::vector<double>> atm;
  
  GUI(const std::string& h, int tcp, int udp, Eigen::MatrixXd fl, Eigen::VectorXi fc, int intus, int blaus, bool reverse) :
  init(false), error(false), quit(false), run(false), operating(false), waiting(false),
  host(h), tcp_port(tcp), udp_port(udp), freq_limits(fl), freq_counts(fc),
  integration_time_microsecs(intus), blank_time_microsecs(blaus), mirror(reverse) {
    const size_t N = freq_counts.size();
    f.resize(N);
    d.resize(N);
    for (size_t i=0; i<N; i++) {
      const size_t n = freq_counts[i];
      f[i] = linspace(freq_limits(i, 0), freq_limits(i, 1), n);
      d[i] = std::vector<double>(n, 0);
    }
  }
};

template <typename ... Spectrometers>
struct Backends {
  static constexpr size_t N = sizeof...(Spectrometers);
  
  std::tuple<Spectrometers...> spectrometers;
  
  Backends(Spectrometers ... s) noexcept : spectrometers(std::make_tuple(s...)) {static_assert(N > 0);}
  
  template <size_t i=0>
  void run(int j) {
    if (i == j)
      std::get<i>(spectrometers).run();
    else if constexpr (i < N - 1)
      run<i+1>(j);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  void init(int j, bool manual) {
    if (i == j)
      std::get<i>(spectrometers).init(manual);
    else if constexpr (i < N - 1)
      init<i+1>(j, manual);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  void startup(int j, std::string& h, int t, int u, Eigen::Ref<Eigen::MatrixXd> fl, Eigen::Ref<Eigen::VectorXi> fc, int ius, int bus, bool m) {
    if (i == j)
      std::get<i>(spectrometers).startup(h, t, u, fl, fc, ius, bus, m);
    else if constexpr (i < N - 1)
      startup<i+1>(j, h, t, u, fl, fc, ius, bus, m);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  void close(int j) {
    if (i == j)
      std::get<i>(spectrometers).close();
    else if constexpr (i < N - 1)
      close<i+1>(j);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  void delete_error(int j) {
    if (i == j)
      std::get<i>(spectrometers).delete_error();
    else if constexpr (i < N - 1)
      delete_error<i+1>(j);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  void get_data(int j, int k) {
    if (i == j)
      std::get<i>(spectrometers).get_data(k);
    else if constexpr (i < N - 1)
      get_data<i+1>(j, k);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  std::string name(int j) {
    if (i == j)
      return std::get<i>(spectrometers).name();
    else if constexpr (i < N - 1)
      return name<i+1>(j);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  std::string error_string(int j) {
    if (i == j)
      return std::get<i>(spectrometers).error_string();
    else if constexpr (i < N - 1)
      return error_string<i+1>(j);
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  bool has_error(int j) {
    if (i == j)
      return std::get<i>(spectrometers).has_error();
    else if constexpr (i < N - 1) {
      return has_error<i+1>(j);
    }
    else
      std::exit(1);
  }
  
  template <size_t i=0>
  std::vector<std::vector<double>> datavec(int j) {
    if (i == j)
      return std::get<i>(spectrometers).datavec();
    else if constexpr (i < N - 1)
      return datavec<i+1>(j);
    else
      std::exit(1);
  }
};

template <typename ... Spectrometers>
void GuiSetup(Backends<Spectrometers ...>& spectrometers, std::array<GUI, sizeof...(Spectrometers)>& ctrls) {
  constexpr size_t N = sizeof...(Spectrometers);

  // Do setup per spectrometer
  if (ImGui::BeginTabBar("Spectrometer List")) {
    for (size_t i=0; i<N; i++) {
      if (ImGui::BeginTabItem(spectrometers.name(i).c_str())) {
        
        bool change=false;
        bool manual=false;
        if (not ctrls[i].init) {
          if (ImGui::Button(" Manual Init ")) {
            change = true;
            manual = true;
            ctrls[i].init = true;
          } ImGui::SameLine();
          if (ImGui::Button(" Automatic Init ")) {
            change = true;
            manual = false;
            ctrls[i].init = true;
          } ImGui::SameLine();
          ImGui::Text(" Close ");
        } else  {
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
            spectrometers.startup(i, ctrls[i].host, ctrls[i].tcp_port, ctrls[i].udp_port,
                                  ctrls[i].freq_limits, ctrls[i].freq_counts,
                                  ctrls[i].integration_time_microsecs, ctrls[i].blank_time_microsecs,
                                  ctrls[i].mirror);
            spectrometers.init(i, manual);
          } else {
            spectrometers.close(i);
          }
        }
        
        if (ctrls[i].init) {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &ctrls[i].host, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &ctrls[i].tcp_port, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &ctrls[i].udp_port, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &ctrls[i].host, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &ctrls[i].tcp_port, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &ctrls[i].udp_port, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
        }
        
        if (ImGui::BeginCombo("Channels", "Channels and Frequency")) {
          ImGui::Text(" ");
          for (long int j=0; j<ctrls[i].freq_counts.size(); j++) {
            ImGui::Text("\tCount: %i [#]; Low: %.4lf [GHz]; Upper: %.4lf [GHz]\t", ctrls[i].freq_counts[j], ctrls[i].freq_limits(j, 0)/1e9, ctrls[i].freq_limits(j, 1)/1e9);
          }
          ImGui::Text(" ");
          ImGui::EndCombo();
        }
        
        if (ctrls[i].init) {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &ctrls[i].integration_time_microsecs, 1, 100,  ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &ctrls[i].blank_time_microsecs, 1, 10, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          bool m = ctrls[i].mirror;
          ImGui::Checkbox("Mirror", &m);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &ctrls[i].integration_time_microsecs, 1, 100, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &ctrls[i].blank_time_microsecs, 1, 10, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AutoSelectAll);
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
  std::vector<std::vector<double>> data;
  std::vector<double> dummy_x;

public:
  template <typename ... Whatever> constexpr Dummy(const std::string& n, Whatever...) : mname(n), 
  manual(false), error_found(false),
  error(""), data(2, std::vector<double>(1000)), dummy_x(1000) {
    for (int i=0; i<1000; i++) {
      dummy_x[i] = 4*3.14*i/1000.;
      data[0][i] = 5 + std::cos(dummy_x[i]);
      data[1][i] = 5 + std::sin(dummy_x[i]);
    }
  }
  
  void startup(std::string&, int, int, Eigen::Ref<Eigen::MatrixXd>, Eigen::Ref<Eigen::VectorXi>, int, int, bool) {}
  void init(bool manual_init) {manual=manual_init; if (not manual) {error = "Must be manual, is dummy"; error_found=true;}}
  void close() {}
  void run() {}
  std::vector<std::vector<double>> datavec() {std::vector<std::vector<double>> out{data}; return out;}
  std::string name() const {return mname;}
  
  void get_data(int) {
    for (int i=0; i<1000; i++) {
      dummy_x[i] += 0.05;
      data[0][i] = 5 + std::cos(dummy_x[i]);
      data[1][i] = 5 + std::sin(dummy_x[i]);
    }
    Sleep(0.1);
  }
  bool manual_run() {return manual;}
  const std::string& error_string() const {return error;}
  bool has_error() {return error_found;}
  void delete_error() {error_found=false; error = "";}
};  // Dummy

}  // Spectrometer
}  // Instrument

#endif  // backend_h
