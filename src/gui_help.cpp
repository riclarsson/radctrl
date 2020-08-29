#include "gui.h"

static int mini(int x, int y) { return x < y ? x : y; }
static int maxi(int x, int y) { return x > y ? x : y; }

namespace GUI {
GLFWmonitor *get_current_monitor(GLFWwindow *window) {
  int nmonitors, i;
  int wx, wy, ww, wh;
  int mx, my;
  int bestoverlap;
  GLFWmonitor *bestmonitor;
  GLFWmonitor **monitors;
  const GLFWvidmode *mode;

  bestoverlap = 0;
  bestmonitor = NULL;

  glfwGetWindowPos(window, &wx, &wy);
  glfwGetWindowSize(window, &ww, &wh);
  monitors = glfwGetMonitors(&nmonitors);

  for (i = 0; i < nmonitors; i++) {
    mode = glfwGetVideoMode(monitors[i]);
    glfwGetMonitorPos(monitors[i], &mx, &my);

    int mw = mode->width;
    int mh = mode->height;

    int overlap = maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) *
                  maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

    if (bestoverlap < overlap) {
      bestoverlap = overlap;
      bestmonitor = monitors[i];
    }
  }

  return bestmonitor;
}
}  // namespace GUI
