#include "gui_macros.h"

namespace GUI {
namespace Windows {
template <unsigned WIDTH = 1, unsigned HEIGHT = 1, unsigned WIDTH_POS = 0,
          unsigned HEIGHT_POS = 0, unsigned WIDTH_EXTENT = 1,
          unsigned HEIGHT_EXTENT = 1>
bool sub(GLFWwindow* window, const ImVec2 origpos, const char* name) {
  static_assert(WIDTH, "None size window not allowed");
  static_assert(HEIGHT, "None size window not allowed");
  static_assert(WIDTH_EXTENT, "None extent window not allowed");
  static_assert(HEIGHT_EXTENT, "None extent window not allowed");
  static_assert(WIDTH > (WIDTH_POS + WIDTH_EXTENT - 1),
                "More width than possible");
  static_assert(HEIGHT > (HEIGHT_POS + HEIGHT_EXTENT - 1),
                "More height than possible");
  constexpr float wscale = 1.0f / float(WIDTH);
  constexpr float hscale = 1.0f / float(HEIGHT);

  // Cursors and sizes
  int width = 0, height = 0;
  glfwGetWindowSize(window, &width, &height);
  ImVec2 size = {(float(width) - origpos.x - 5.0f) * wscale,
                 (float(height) - 2 * origpos.y) * hscale};
  ImVec2 pos = {2.5f + origpos.x + size.x * WIDTH_POS,
                1.5f * origpos.y + size.y * HEIGHT_POS};
  size.x *= WIDTH_EXTENT;
  size.y *= HEIGHT_EXTENT;

  // Set a simple window frame
  ImGui::SetNextWindowPos(pos, 1);
  ImGui::SetNextWindowSize(size, 1);

  return ImGui::Begin(name, NULL,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                          ImGuiWindowFlags_NoSavedSettings |
                          ImGuiWindowFlags_NoResize |
                          ImGuiWindowFlags_NoScrollbar);
}
inline bool full(GLFWwindow* window, const ImVec2 origpos, const char* name) {
  return sub(window, origpos, name);
}
inline void end() { ImGui::End(); }
}  // namespace Windows
}  // namespace GUI
