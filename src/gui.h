#ifndef gui_h
#define gui_h

#include <vector>
#include <string>

#include <imgui.h>
#include <implot.h>

#include "gui_macros.h"

namespace GUI {
/** A global config for all things GUI */
struct Config {
  /** The io variable of ImGui, has a lot of key presses */
  ImGuiIO& io;
  
  /** Should we go fullscreen? */
  bool fullscreen;
  
  /** Are we dealing with the error? */
  int active_errors;
  
  /** Window positions; used when fullscreen is toggled on and off to return window to good position and size */
  int width, height, xpos, ypos;
  
  /** List of tabs */
  size_t tabspos;
  std::vector<std::string> tabs;
  
  Config(bool fullscreen_on=false) : io(ImGui::GetIO()),
  fullscreen(fullscreen_on), active_errors(0), width(1280), height(720), xpos(50), ypos(50),
  tabspos(0), tabs({"Main"})
  {
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.IniFilename = NULL;  // Don't save any initialization file
  }
};  // Config

inline void LayoutAndStyleSettings() {
  auto& style = ImGui::GetStyle();
  style.FramePadding = {0.0f, 0.0f};
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.WindowPadding = {0.0f, 0.0f};
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 0.1f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 1.0f;
  style.AntiAliasedLinesUseTex = true;
  style.AntiAliasedFill = true;
  style.ChildRounding = 0.0f;
  
  style.Colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
  style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  style.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  style.Colors[ImGuiCol_Border]                 = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
  style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_Button]                 = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_Header]                 = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
  style.Colors[ImGuiCol_Separator]              = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_Tab]                    = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
  style.Colors[ImGuiCol_TabActive]              = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
  style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
  style.Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

GLFWmonitor* get_current_monitor(GLFWwindow * window);

namespace MainMenu {
void fullscreen(Config& cfg, GLFWwindow* window);
void quitscreen(const Config& cfg, GLFWwindow* window);
size_t tabselect(Config& cfg);
}  // MainMenu

namespace Windows {
template <unsigned WIDTH=1, unsigned HEIGHT=1, unsigned WIDTH_POS=0, unsigned HEIGHT_POS=0, unsigned WIDTH_EXTENT=1, unsigned HEIGHT_EXTENT=1>
bool sub(GLFWwindow* window, const ImVec2 origpos, const char * name)
{
  static_assert(WIDTH, "None size window not allowed");
  static_assert(HEIGHT, "None size window not allowed");
  static_assert(WIDTH_EXTENT, "None extent window not allowed");
  static_assert(HEIGHT_EXTENT, "None extent window not allowed");
  static_assert(WIDTH > (WIDTH_POS + WIDTH_EXTENT - 1), "More width than possible");
  static_assert(HEIGHT > (HEIGHT_POS + HEIGHT_EXTENT - 1), "More height than possible");
  constexpr float wscale = 1.0f / float(WIDTH);
  constexpr float hscale = 1.0f / float(HEIGHT);
  
  //Cursors and sizes
  int width = 0, height = 0;
  glfwGetWindowSize(window, &width, &height);
  ImVec2 size = {(float(width)-origpos.x - 5.0f)*wscale, (float(height)-2*origpos.y)*hscale};
  ImVec2 pos = {2.5f + origpos.x + size.x*WIDTH_POS, 1.5f*origpos.y + size.y*HEIGHT_POS};
  size.x *= WIDTH_EXTENT;
  size.y *= HEIGHT_EXTENT;
  
  // Set a simple window frame
  ImGui::SetNextWindowPos(pos, 1);
  ImGui::SetNextWindowSize(size, 1);
  
  return ImGui::Begin(name, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
}
inline bool full(GLFWwindow* window, const ImVec2 origpos, const char* name) {return sub(window, origpos, name);}
inline void end() {ImGui::End();}
}

}  // GUI

#endif  // gui_h
