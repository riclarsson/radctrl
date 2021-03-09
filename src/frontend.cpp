#include "frontend.h"

#include "timeclass.h"

namespace Instrument::Frontend {
bool Controller::has_error() {
  return std::visit([](auto && frontend){return frontend.has_error();}, frontend);
}

const std::string& Controller::error_string() const {
  return std::visit([](auto && frontend) -> const std::string& {return frontend.error_string();}, frontend);
}

const std::string& Controller::name() const {
  return std::visit([](auto && frontend) -> const std::string& {return frontend.name();}, frontend);
}

void Controller::delete_error() {
  std::visit([](auto && frontend){frontend.delete_error();}, frontend);
}

void Controller::startup() {
  std::visit([this](auto && frontend){frontend.startup(server, port);}, frontend);
}

void Controller::initialize(bool manual) {
  std::visit([manual](auto && frontend){frontend.init(manual);}, frontend);
  init = true;
}

void Controller::close() {
  std::visit([](auto && frontend){frontend.close();}, frontend);
  init = false;
}

void Controller::run_machine() {
  std::visit([](auto && frontend){frontend.run();}, frontend);
}

void Controller::set_cold(double& cold) {
  cold = std::visit([cold](auto && frontend){
    if constexpr (frontend.has_cold_load) {
      std::cout << Time() << " Copy Frontend Cold Load\n";
      return frontend.cold_load();
    } else {
      return cold;
    }
  }, frontend);
}

void Controller::set_hot(double& hot) {
  hot = std::visit([hot](auto && frontend){
    if constexpr (frontend.has_hot_load) {
      std::cout << Time() << " Copy Frontend Hot Load\n";
      return frontend.hot_load();
    } else {
      return hot;
    }
  }, frontend);
}

void Controller::get_data() {
  std::visit([](auto && frontend){frontend.get_data();}, frontend);
}

std::map<std::string, double> Controller::data_load() {
  return std::visit([](auto && frontend){return frontend.data();}, frontend);
}

void GuiSetup(Controller &ctrl) {
  bool change = false;
  bool manual = false;
  if (not ctrl.init) {
    if (ImGui::Button(" Manual Init ")) {
      change = true;
      manual = true;
      ctrl.init = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Automatic Init ")) {
      change = true;
      manual = false;
      ctrl.init = true;
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
      ctrl.init = false;
    }
  }

  if (change) {
    if (ctrl.init) {
      ctrl.startup();
      ctrl.initialize(manual);
    } else {
      ctrl.close();
    }
  }

  // Each frontend will be very different so the setup must be dealt with
  // individually
  std::visit([&ctrl](auto && frontend){
    if constexpr (frontend.has_config) {
      ImGui::PushItemWidth(180.0f);
      if (ctrl.init) {
        ImGui::InputText("Server", &ctrl.server,
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
      } else {
        ImGui::InputText("Server", &ctrl.server,
                        ImGuiInputTextFlags_CharsNoBlank |
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
      }
      ImGui::PopItemWidth();
      ImGui::SameLine();
      ImGui::PushItemWidth(120.0f);
      if (ctrl.init) {
        ImGui::InputInt("port", &ctrl.port, 1, 100,
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll |
                        ImGuiInputTextFlags_ReadOnly);
      } else {
        ImGui::InputInt("port", &ctrl.port, 1, 100,
                        ImGuiInputTextFlags_AutoSelectAll |
                        ImGuiInputTextFlags_NoHorizontalScroll);
      }
      ImGui::PopItemWidth();
      
      if (ctrl.init and frontend.manual_run()) {
        if (ImGui::Button(" Run ")) {
          frontend.run();
          frontend.get_data();
          ctrl.data = frontend.data();
        }
      } else {
        ImGui::Text(" Run ");
      }
    }
  }, ctrl.frontend);

  if (ctrl.has_error()) {
    ctrl.init = false;
    ctrl.error = true;
  } else
    ctrl.error = false;
}



Controller parse(const File::ConfigParser& parser) try {
  const std::string_view type = parser("Frontend", "type");
  
  if (type == "Waspam") {
    int port = std::stoi(std::string(parser("Frontend", "port")));
    return Controller{Waspam(parser("Frontend", "path")), std::string(parser("Frontend", "server")), port};
  } else if (type == "DBR") {
    int port = std::stoi(std::string(parser("Frontend", "port")));
    return Controller{DBR(parser("Frontend", "path")), std::string(parser("Frontend", "server")), port};
  } else {
    std::ostringstream os;
    os << "Cannot understand type: " << type;
    throw std::runtime_error(os.str());
  }
} catch (std::exception& e) {
  std::ostringstream os;
  os << "Cannot parse Frontend with error:\n" << e.what();
  throw std::runtime_error(os.str());
}
}  // namespace Instrument::Frontend
