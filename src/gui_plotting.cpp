#include "gui_plotting.h"

namespace GUI {
namespace Plotting {
void plot_frame(Frame &frame) {
  if (ImPlot::BeginPlot(frame.title().c_str(), frame.xlabel().c_str(),
                        frame.ylabel().c_str(), {-1, -1})) {
    for (auto &line : frame)
      ImPlot::PlotLine(line.name().c_str(), line.getter(), (void *)&line,
                       line.size());
    ImPlot::EndPlot();
  }
}

bool freq_scale(double &cur_freq_scale) {
  static constexpr std::array<const char *const, 6> targets{
      " THz ", " GHz ", " MHz ", " kHz ", " Hz  ", " mHz "};
  const int current = cur_freq_scale <= 1e-12
                          ? 0
                          : cur_freq_scale <= 1e-9
                                ? 1
                                : cur_freq_scale <= 1e-6
                                      ? 2
                                      : cur_freq_scale <= 1e-3
                                            ? 3
                                            : cur_freq_scale <= 1 ? 4 : 5;
  int i = current;

  bool newout = false;
  if (ImGui::Combo("X Scale ", &i, targets.data(), 6)) {
    newout = current not_eq i;
    if (/*targets[*/ i /*]*/ == /*targets[*/ 0 /*]*/)
      cur_freq_scale = 1e-12;
    else if (/*targets[*/ i /*]*/ == /*targets[*/ 1 /*]*/)
      cur_freq_scale = 1e-9;
    else if (/*targets[*/ i /*]*/ == /*targets[*/ 2 /*]*/)
      cur_freq_scale = 1e-6;
    else if (/*targets[*/ i /*]*/ == /*targets[*/ 3 /*]*/)
      cur_freq_scale = 1e-3;
    else if (/*targets[*/ i /*]*/ == /*targets[*/ 4 /*]*/)
      cur_freq_scale = 1;
    else if (/*targets[*/ i /*]*/ == /*targets[*/ 5 /*]*/)
      cur_freq_scale = 1e3;
  }
  return newout;
}

void line_menuitem(Line &line) {
  if (ImGui::BeginMenu(line.name().c_str())) {
    bool update = false;
    int ra = line.RunAvgCount();
    double xo = line.Xoffset(), xs = line.Xscale(), yo = line.Yoffset(),
           ys = line.Yscale();

    update |= ImGui::InputInt("Running Average", &ra, 1, 2,
                              ImGuiInputTextFlags_EnterReturnsTrue);
    if (ra < 1) ra = 1;
    ImGui::Separator();
    update |= freq_scale(xs);
    update |= ImGui::InputDouble("X Offset", &xo, 0.0, 0.0, "%.6f",
                                 ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::Separator();
    update |= ImGui::InputDouble("Y Scale ", &ys, 0.0, 0.0, "%.6f",
                                 ImGuiInputTextFlags_EnterReturnsTrue);
    if (ys <= 0) ys = std::numeric_limits<double>::min();
    update |= ImGui::InputDouble("Y Offset", &yo, 0.0, 0.0, "%.6f",
                                 ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::Separator();
    ImGui::EndMenu();

    if (update) {
      line.RunAvgCount(size_t(ra));
      line.Xoffset(xo);
      line.Xscale(xs);
      line.Yoffset(yo);
      line.Yscale(ys);
    }
  }
}

void frame_menuitem(Frame &frame) {
  if (ImGui::BeginMenu(frame.title().c_str())) {
    int ra = frame[0].RunAvgCount();
    double xo = frame[0].Xoffset(), xs = frame[0].Xscale(),
           yo = frame[0].Yoffset(), ys = frame[0].Yscale();

    if (ImGui::InputInt("Running Average", &ra, 1, 2,
                        ImGuiInputTextFlags_EnterReturnsTrue)) {
      if (ra < 1) ra = 1;

      for (auto &line : frame) {
        line.RunAvgCount(size_t(ra));
      }
    }

    ImGui::Separator();

    if (freq_scale(xs)) {
      for (auto &line : frame) {
        line.Xscale(xs);
      }
    }

    if (ImGui::InputDouble("X Offset", &xo, 0.0, 0.0, "%.6f",
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
      for (auto &line : frame) {
        line.Xoffset(xo);
      }
    }

    ImGui::Separator();

    if (ImGui::InputDouble("Y Scale ", &ys, 0.0, 0.0, "%.6f",
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
      if (ys <= 0) ys = std::numeric_limits<double>::min();

      for (auto &line : frame) {
        line.Yscale(ys);
      }
    }

    if (ImGui::InputDouble("Y Offset", &yo, 0.0, 0.0, "%.6f",
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
      for (auto &line : frame) {
        line.Yoffset(yo);
      }
    }

    ImGui::Separator();

    for (auto &line : frame) line_menuitem(line);
    ImGui::EndMenu();
  }
}
}  // namespace Plotting
}  // namespace GUI
