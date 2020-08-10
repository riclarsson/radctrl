#include "gui_plotting.h"

namespace GUI {
namespace Plotting {
void plot_frame(Frame& frame) {
  if (ImPlot::BeginPlot(frame.title().c_str(), frame.xlabel().c_str(), frame.ylabel().c_str(), {-1, -1})) {
    for (auto& line: frame)
      ImPlot::PlotLine(line.name().c_str(), line.getter(), (void*)&line, line.size());
    ImPlot::EndPlot();
  }
}
}  // Plotting
}  // GUI
