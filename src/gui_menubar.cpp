#include "gui.h"

namespace GUI {
namespace MainMenu {
void fullscreen(Config& cfg, GLFWwindow* window)
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(" Fullscreen ", "F11")) {
        if (not cfg.fullscreen) {
          glfwGetWindowSize(window, &cfg.width, &cfg.height);
          glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
          auto * monitor = GUI::get_current_monitor(window);
          const auto * mode = glfwGetVideoMode(monitor);
          glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
        }
        else
          glfwSetWindowMonitor(window, NULL, cfg.xpos, cfg.ypos, cfg.width, cfg.height, 0);
        
        cfg.fullscreen = not cfg.fullscreen;
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  
  if (ImGui::IsKeyPressed(GLFW_KEY_F11) or (cfg.fullscreen and ImGui::IsKeyPressed(GLFW_KEY_ESCAPE))) {
    if (not cfg.fullscreen) {
      glfwGetWindowSize(window, &cfg.width, &cfg.height);
      glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
      glfwGetWindowPos(window, &cfg.xpos, &cfg.ypos);
      auto * monitor = GUI::get_current_monitor(window);
      const auto * mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
      glfwSetWindowMonitor(window, NULL, cfg.xpos, cfg.ypos, cfg.width, cfg.height, 0);
    cfg.fullscreen = not cfg.fullscreen;
  }
}

void quitscreen(const Config& cfg, GLFWwindow* window)
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(" Quit ", "Ctrl+X"))
        glfwSetWindowShouldClose(window, 1);
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  
  if (cfg.io.KeyCtrl and ImGui::IsKeyPressed(GLFW_KEY_X)) {
    glfwSetWindowShouldClose(window, 1);
  }
}


size_t tabselect(Config& cfg)
{
  if (ImGui::BeginMainMenuBar()) {
    ImGui::Separator();
    
    if (ImGui::BeginTabBar("WindowSelection")) {
      for (size_t i=0; i<cfg.tabs.size(); i++) {
        if (ImGui::BeginTabItem(cfg.tabs[i].c_str())) {
          cfg.tabspos = i;
          ImGui::EndTabItem();
        }
      }
      ImGui::EndTabBar();
    }
    ImGui::EndMainMenuBar();
  }
  
  return cfg.tabspos;
}
}  // MainMenu
}  // GUI
