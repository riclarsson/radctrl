#ifndef gui_plotting_h
#define gui_plotting_h

#include <implot.h>

#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <mutex>
#include <vector>

#include "gui_windows.h"

namespace GUI {
namespace Plotting {
class Data {
  bool use_second;
  std::array<std::vector<double>, 2> data;
  mutable std::mutex mtx;

 public:
  Data(const std::vector<double> &d) : use_second(true), data({d, d}) {}
  Data(size_t n)
      : use_second(true),
        data({std::vector<double>(n, 0), std::vector<double>(n, 0)}) {}
  Data(const Data &x) : use_second(true), data(x.data) {}

  double get(size_t i) const {
    mtx.lock();
    double x = data[use_second][i];
    mtx.unlock();
    return x;
  }

  void set(const std::vector<double> &newdata) {
    data[not use_second] = newdata;
    mtx.lock();
    use_second = not use_second;
    mtx.unlock();
  }

  size_t N() const {
    mtx.lock();
    auto x = data[use_second].size();
    mtx.unlock();
    return x;
  }
};  // Data

/** LineGetter is a function pointer required by the plotting tool */
typedef ImPlotPoint (*LineGetter)(void *, int);

class Line {
  mutable std::mutex mtx;

  std::string mname;
  Data xval;
  Data yval;
  size_t running_avg;
  double xscale;
  double yscale;
  double xoffset;
  double yoffset;

  double x(size_t i) const {
    mtx.lock();
    const double val = (xoffset + xval.get(i)) * xscale;
    mtx.unlock();
    return val;
  }
  double y(size_t i) const {
    mtx.lock();
    const double val = (yoffset + yval.get(i)) * yscale;
    mtx.unlock();
    return val;
  }

  ImPlotPoint avg(size_t i) const {
    ImPlotPoint out{0.0, 0.0};
    size_t num{0};

    mtx.lock();  // Access running_avg
    bool test_loop = num < running_avg;
    mtx.unlock();

    while (test_loop) {
      mtx.lock();  // Access running_avg
      const size_t j = running_avg * i + num;
      mtx.unlock();

      // Safety measures in case we are changing things
      if (j < xval.N()) out.x += x(j);
      if (j < yval.N()) out.y += y(j);

      num++;
      mtx.lock();  // Access running_avg
      test_loop = num < running_avg;
      mtx.unlock();
    }

    mtx.lock();  // Access running_avg
    out.x /= running_avg;
    out.y /= running_avg;
    mtx.unlock();
    return out;
  }

  LineGetter avgfun() const {
    return [](void *data, int i) { return static_cast<Line *>(data)->avg(i); };
  }

 public:
  Line(const std::string &name, const Data &X, const Data &Y)
      : mname(name),
        xval(X),
        yval(Y),
        running_avg(1),
        xscale(1),
        yscale(1),
        xoffset(0),
        yoffset(0) {
    if (X.N() not_eq Y.N()) throw std::runtime_error("Bad data size");
  }
  Line(const Line &other) noexcept
      : mname(other.mname),
        xval(other.xval),
        yval(other.yval),
        running_avg(other.running_avg),
        xscale(other.xscale),
        yscale(other.yscale),
        xoffset(other.xoffset),
        yoffset(other.yoffset) {}

  void setX(const std::vector<double> &x) { xval.set(x); }
  void setY(const std::vector<double> &y) { yval.set(y); }
  int size() const {
    mtx.lock();
    const int val = yval.N() / running_avg;
    mtx.unlock();
    return val;
  }
  LineGetter getter() const { return avgfun(); }
  const std::string &name() const { return mname; }
  void Xscale(double x) {
    mtx.lock();
    xscale = x;
    mtx.unlock();
  }
  void Yscale(double x) {
    mtx.lock();
    yscale = x;
    mtx.unlock();
  }
  void Xoffset(double x) {
    mtx.lock();
    xoffset = x;
    mtx.unlock();
  }
  void Yoffset(double x) {
    mtx.lock();
    yoffset = x;
    mtx.unlock();
  }
  double Xscale() const {
    mtx.lock();
    const double val = xscale;
    mtx.unlock();
    return val;
  }
  double Yscale() const {
    mtx.lock();
    const double val = yscale;
    mtx.unlock();
    return val;
  }
  double Xoffset() const {
    mtx.lock();
    const double val = xoffset;
    mtx.unlock();
    return val;
  }
  double Yoffset() const {
    mtx.lock();
    const double val = yoffset;
    mtx.unlock();
    return val;
  }
  void RunAvgCount(size_t n) {
    mtx.lock();
    running_avg = n;
    mtx.unlock();
  }
  size_t RunAvgCount() const {
    mtx.lock();
    const size_t val = running_avg;
    mtx.unlock();
    return val;
  }
};  // Line

class Frame {
  std::string mtitle;
  std::string mxlabel;
  std::string mylabel;
  std::vector<Line> frame;

 public:
  Frame(const std::string &title, const std::string &xlabel,
        const std::string &ylabel, const std::vector<Line> &plotdata)
      : mtitle(title), mxlabel(xlabel), mylabel(ylabel), frame(plotdata) {}
  size_t size() const { return frame.size(); }
  const std::string &title() const { return mtitle; }
  const std::string &xlabel() const { return mxlabel; }
  const std::string &ylabel() const { return mylabel; }
  decltype(frame.begin()) begin() { return frame.begin(); }
  decltype(frame.end()) end() { return frame.end(); }
  decltype(frame.cbegin()) cbegin() const { return frame.cbegin(); }
  decltype(frame.cend()) cend() const { return frame.cend(); }
  Line &operator[](size_t i) { return frame[i]; }
};  // Frame

void plot_frame(Frame &frame);

void frame_menuitem(Frame &frame);

template <size_t Height, size_t PartOfHeight>
class CAHA {
  std::string mname;
  Frame raw;
  Frame noise;
  Frame integration;
  Frame averaging;

 public:
  static constexpr size_t N = Height;
  static constexpr size_t M = PartOfHeight;
  static_assert(N >= M);

  CAHA(std::string plotname, const std::vector<std::vector<double>> &f)
      : mname(plotname),
        raw("tmp", "tmp", "tmp", {}),
        noise("tmp", "tmp", "tmp", {}),
        integration("tmp", "tmp", "tmp", {}),
        averaging("tmp", "tmp", "tmp", {}) {
    const std::string cold = "Cold ";
    const std::string target = "Target ";
    const std::string hot = "Hot ";
    const std::string sys = "System Noise ";
    const std::string avg = "Average Measurement ";
    const std::string last = "Last Measurement ";
    std::vector<Line> raws;
    std::vector<Line> noises;
    std::vector<Line> averagings;
    std::vector<Line> integrations;
    for (size_t i = 0; i < f.size(); i++) {
      raws.push_back(GUI::Plotting::Line(
        cold + std::to_string(i), f[i], f[i].size()));
      raws.push_back(GUI::Plotting::Line(
        target + std::to_string(i), f[i], f[i].size()));
      raws.push_back(GUI::Plotting::Line(
        hot + std::to_string(i), f[i], f[i].size()));
      noises.push_back(GUI::Plotting::Line(
        sys + std::to_string(i), f[i], f[i].size()));
      averagings.push_back(GUI::Plotting::Line(
        avg + std::to_string(i), f[i],
          f[i].size()));
      integrations.push_back(GUI::Plotting::Line(
        last + std::to_string(i), f[i],
          f[i].size()));
    }
    raw = Frame("Raw", "Frequency", "Counts", raws);
    noise = Frame("Noise", "Frequency", "Temperature [K]", noises);
    averaging = Frame("Averaging", "Frequency", "Temperature [K]", averagings);
    integration =
        Frame("Integration", "Frequency", "Temperature [K]", integrations);
  }

  Frame &Raw() { return raw; }
  Frame &Noise() { return noise; }
  Frame &Averaging() { return averaging; }
  Frame &Integration() { return integration; }

  const Frame &Raw() const { return raw; }
  const Frame &Noise() const { return noise; }
  const Frame &Averaging() const { return averaging; }
  const Frame &Integration() const { return integration; }

  void plot(GLFWwindow *window, const ImVec2 startpos) {
    const std::string raw_text = "raw";
    const std::string noi_text = "noise";
    const std::string itg_text = "integration";
    const std::string avg_text = "averaging";
    
    if (GUI::Windows::sub<2, Height, 0, 0, 1, PartOfHeight / 2>(
      window, startpos, (mname + raw_text).c_str())) {
      plot_frame(raw);
    }
    GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 0, PartOfHeight / 2, 1, PartOfHeight / 2>(
      window, startpos, (mname + noi_text).c_str())) {
      plot_frame(noise);
    }
    GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 1, 0, 1, PartOfHeight / 2>(
      window, startpos, (mname + itg_text).c_str())) {
      plot_frame(integration);
    }
    GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 1, PartOfHeight / 2, 1, PartOfHeight / 2>(
      window, startpos, (mname + avg_text).c_str())) {
      plot_frame(averaging);
    }
    GUI::Windows::end();
  }

  const std::string &name() const { return mname; }
};

template <size_t Height, size_t PartOfHeight, size_t N>
void plot_combined(GLFWwindow *window, const ImVec2 startpos,
                   std::array<CAHA<Height, PartOfHeight>, N> &cahas) {
  const std::string space = " ";
  if (GUI::Windows::sub<1, Height, 0, 0, 1, PartOfHeight / 3>(
          window, startpos, "Combined Last Measurement Integration TILE")) {
    if (ImPlot::BeginPlot(cahas[0].Integration().title().c_str(),
                          cahas[0].Integration().xlabel().c_str(),
                          cahas[0].Integration().ylabel().c_str(), {-1, -1})) {
      for (auto &caha : cahas)
        for (auto &line : caha.Integration())
          ImPlot::PlotLine(
            (caha.name() + space + line.name()).c_str(),
              line.getter(), (void *)&line, line.size());
      ImPlot::EndPlot();
    }
  }
  GUI::Windows::end();
  if (GUI::Windows::sub<1, Height, 0, PartOfHeight / 3, 1, PartOfHeight / 3>(
          window, startpos, "Combined Average Measurement Integration TILE")) {
    if (ImPlot::BeginPlot(cahas[0].Averaging().title().c_str(),
                          cahas[0].Averaging().xlabel().c_str(),
                          cahas[0].Averaging().ylabel().c_str(), {-1, -1})) {
      for (auto &caha : cahas)
        for (auto &line : caha.Averaging())
          ImPlot::PlotLine(
            (caha.name() + space + line.name()).c_str(),
              line.getter(), (void *)&line, line.size());
      ImPlot::EndPlot();
    }
  }
  GUI::Windows::end();
  if (GUI::Windows::sub<1, Height, 0, 2 * PartOfHeight / 3, 1,
                        PartOfHeight / 3>(window, startpos,
                                          "Combined Noise TILE")) {
    if (ImPlot::BeginPlot(cahas[0].Noise().title().c_str(),
                          cahas[0].Noise().xlabel().c_str(),
                          cahas[0].Noise().ylabel().c_str(), {-1, -1})) {
      for (auto &caha : cahas)
        for (auto &line : caha.Noise())
          ImPlot::PlotLine(
            (caha.name() + space + line.name()).c_str(),
              line.getter(), (void *)&line, line.size());
      ImPlot::EndPlot();
    }
  }
  GUI::Windows::end();
}

template <size_t Height, size_t PartOfHeight>
void caha_menuitem(CAHA<Height, PartOfHeight> &caha) {
  if (ImGui::BeginMenu(caha.name().c_str())) {
    frame_menuitem(caha.Raw());
    ImGui::Separator();
    frame_menuitem(caha.Noise());
    ImGui::Separator();
    frame_menuitem(caha.Integration());
    ImGui::Separator();
    frame_menuitem(caha.Averaging());
    ImGui::EndMenu();
  }
}

template <size_t Height, size_t PartOfHeight, size_t N>
void caha_mainmenu(std::array<CAHA<Height, PartOfHeight>, N> &cahas) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Plots")) {
      for (auto &caha : cahas) {
        caha_menuitem(caha);
        ImGui::Separator();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

class ListOfLines {
  struct LineData {
    std::vector<double> data;
    std::string name;
    std::string ylabel;
    bool do_plot;
    std::size_t line_pos;
    std::size_t ylabel_pos;
    
    LineData(std::size_t current_size, const std::string& nm, const std::string& yl) : 
      data(current_size, std::numeric_limits<double>::quiet_NaN()),
      name(nm), ylabel(yl), do_plot(false), line_pos(0), ylabel_pos(-1) {}
  };
  
  mutable std::mutex update;
  std::size_t n, i;
  std::vector<double> x;
  std::vector<LineData> data;
  std::string mtitle;
  std::string mxlabel;
  std::array<std::string, 3> mylabels;
  std::vector<Line> lines;
  
public:
  ListOfLines(std::size_t N, const std::string& title, const std::string& xlabel) : n(N), i(0), mtitle(title), mxlabel(xlabel) {};
  
  void update_size(std::size_t N) {
    update.lock();
    if (N < n) {
      x.resize(N);
      for (auto& d: data) d.data.resize(N);
    }
    n=N;
    update.unlock();
  }
  
  void update_data(const double& xnew, const std::map<std::string, double>& datanew) {
    update.lock();
    if (i >= n)
      i = 0;
    
    if (x.size() < n) {  // push_back
      for (auto& d: data) {
        if (const auto entry = datanew.find(d.name); datanew.end() not_eq entry) {
          d.data.push_back(entry -> second);
        } else {
          d.data.push_back(std::numeric_limits<double>::quiet_NaN());
        }
      }
      x.push_back(xnew);
      
    } else {  // replace
      for (auto& d: data) {
        if (const auto entry = datanew.find(d.name); datanew.end() not_eq entry) {
          d.data[i] = entry -> second;
        } else {
          d.data[i] = std::numeric_limits<double>::quiet_NaN();
        }
      }
      x[i] = xnew;
    }
    
    // Update
    i++;
    update.unlock();
  }
  
  void new_dataline(const std::string& name, const std::string& ylabel) {
    update.lock();
    if (not std::any_of(data.cbegin(), data.cend(), [&](auto& d){return d.name == name;}))
      data.push_back(LineData(n, name, ylabel));
    update.unlock();
  }
  
  void update_lines() {
    update.lock();
    for (auto& d: data) {
      if (d.do_plot) {
        lines[d.line_pos].setX(x);
        lines[d.line_pos].setY(d.data);
      }
    }
    update.unlock();
  }
};

}  // namespace Plotting
}  // namespace GUI

#endif  // gui_plotting_h
