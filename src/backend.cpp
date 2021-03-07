#include "backend.h"

namespace Instrument::Spectrometer {
SingleController::SingleController(Interface && iface, const std::string &controller_name, const std::string &h, int tcp,
             int udp, Eigen::MatrixXd fl, Eigen::VectorXi fc, int intus,
             int blaus, bool reverse)
      : backend(iface),
        init(false),
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

SingleController::SingleController(Interface && iface, const std::string &controller_name,
             const std::filesystem::path &path, int intus, int blaus)
    : backend(iface),
      init(false),
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

SingleController::SingleController(SingleController && sc) noexcept 
: backend(std::move(sc.backend)),
  init(sc.init.load()),
  error(sc.error.load()),
  quit(sc.quit.load()),
  run(sc.run.load()),
  operating(sc.operating.load()),
  waiting(sc.waiting.load()),
  newdata(sc.newdata.load()),
  host(std::move(sc.host)),
  tcp_port(std::move(sc.tcp_port)),
  udp_port(std::move(sc.udp_port)),
  freq_limits(std::move(sc.freq_limits)),
  freq_counts(std::move(sc.freq_counts)),
  integration_time_microsecs(std::move(sc.integration_time_microsecs)),
  blank_time_microsecs(std::move(sc.blank_time_microsecs)),
  mirror(std::move(sc.mirror)),
  name(std::move(sc.name)) {}

SingleController::SingleController(const SingleController & sc) noexcept 
: backend(sc.backend),
  init(sc.init.load()),
  error(sc.error.load()),
  quit(sc.quit.load()),
  run(sc.run.load()),
  operating(sc.operating.load()),
  waiting(sc.waiting.load()),
  newdata(sc.newdata.load()),
  host(sc.host),
  tcp_port(sc.tcp_port),
  udp_port(sc.udp_port),
  freq_limits(sc.freq_limits),
  freq_counts(sc.freq_counts),
  integration_time_microsecs(sc.integration_time_microsecs),
  blank_time_microsecs(sc.blank_time_microsecs),
  mirror(sc.mirror),
  name(sc.name) {}

void SingleController::startup() {
  std::visit([this](auto&&spectrometer){
    spectrometer.startup(
      host, tcp_port, udp_port,
      freq_limits, freq_counts,
      integration_time_microsecs,
      blank_time_microsecs, mirror);
  }, backend);
}

void SingleController::initialize(bool manual) {
  std::visit([manual](auto&&spectrometer){spectrometer.init(manual);}, backend);
}

void SingleController::close() {
  std::visit([](auto&&spectrometer){spectrometer.close();}, backend);
}

bool SingleController::has_error() {
  return std::visit([](auto&&spectrometer){return spectrometer.has_error();}, backend);
}

void SingleController::run_machine() {
  std::visit([](auto&&spectrometer){spectrometer.run();}, backend);
}

void SingleController::get_data(int pos) {
  std::visit([pos](auto&&spectrometer){spectrometer.get_data(pos);}, backend);
}

void SingleController::store_data() {
  d = std::visit([](auto&&spectrometer){return spectrometer.datavec();}, backend);
}

const std::string& SingleController::error_string() const {
  return std::visit([](auto&&spectrometer) -> const std::string& {return spectrometer.error_string();}, backend);
}

void SingleController::delete_error() {
  std::visit([](auto&&spectrometer){spectrometer.delete_error();}, backend);
}

void GuiSetup(Controllers &ctrls) {
  // Do setup per spectrometer
  if (ImGui::BeginTabBar("Spectrometer List")) {
    for (auto& spec: ctrls.backends) {
      if (ImGui::BeginTabItem(spec.name.c_str())) {
        bool change = false;
        bool manual = false;
        if (not spec.init) {
          if (ImGui::Button(" Manual Init ")) {
            change = true;
            manual = true;
            spec.init = true;
          }
          ImGui::SameLine();
          if (ImGui::Button(" Automatic Init ")) {
            change = true;
            manual = false;
            spec.init = true;
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
            spec.init = false;
          }
        }

        if (change) {
          if (spec.init) {
            spec.startup();
            spec.initialize(manual);
          } else {
            spec.close();
          }
        }

        if (spec.init) {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &spec.host,
                           ImGuiInputTextFlags_CharsNoBlank |
                               ImGuiInputTextFlags_NoHorizontalScroll |
                               ImGuiInputTextFlags_AutoSelectAll |
                               ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &spec.tcp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &spec.udp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(150.0f);
          ImGui::InputText("Host", &spec.host,
                           ImGuiInputTextFlags_CharsNoBlank |
                               ImGuiInputTextFlags_NoHorizontalScroll |
                               ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
          ImGui::SameLine();
          ImGui::PushItemWidth(80.0f);
          ImGui::InputInt("TCP", &spec.tcp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("UDP", &spec.udp_port, 1, 100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::PopItemWidth();
        }

        if (ImGui::BeginCombo("Channels", "Channels and Frequency")) {
          ImGui::Text(" ");
          for (long int j = 0; j < spec.freq_counts.size(); j++) {
            ImGui::Text(
                "\tCount: %i [#]; Low: %.4lf [GHz]; Upper: %.4lf [GHz]\t",
                spec.freq_counts[j], spec.freq_limits(j, 0) / 1e9,
                spec.freq_limits(j, 1) / 1e9);
          }
          ImGui::Text(" ");
          ImGui::EndCombo();
        }

        if (spec.init) {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &spec.integration_time_microsecs, 1,
                          100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &spec.blank_time_microsecs, 1, 10,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll |
                              ImGuiInputTextFlags_ReadOnly);
          ImGui::SameLine();
          bool m = spec.mirror;
          ImGui::Checkbox("Mirror", &m);
          ImGui::PopItemWidth();
        } else {
          ImGui::PushItemWidth(90.0f);
          ImGui::InputInt("Int. [us]", &spec.integration_time_microsecs, 1,
                          100,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::InputInt("Blank. [us]", &spec.blank_time_microsecs, 1, 10,
                          ImGuiInputTextFlags_CharsDecimal |
                              ImGuiInputTextFlags_CharsNoBlank |
                              ImGuiInputTextFlags_NoHorizontalScroll |
                              ImGuiInputTextFlags_AutoSelectAll);
          ImGui::SameLine();
          ImGui::Checkbox("Mirror", &spec.mirror);
          ImGui::PopItemWidth();
        }

        if (spec.has_error()) {
          spec.init = false;
          spec.error = true;
        } else {
          spec.error = false;
        }
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

bool Controllers::has_any_errors() {
  bool error = false;
  for (auto& spec : backends) error = error or spec.has_error();
  return error;
}
}  // namespace Instrument::Spectrometer
