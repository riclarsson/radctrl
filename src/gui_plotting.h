#ifndef gui_plotting_h
#define gui_plotting_h

#include <algorithm>
#include <array>
#include <mutex>
#include <vector>

#include <implot.h>

#include "gui_windows.h"

namespace GUI {
namespace Plotting {
class Data {
bool use_second;
std::array<std::vector<double>, 2> data;
mutable std::mutex mtx;

public:
  Data(const std::vector<double>& d) : use_second(true), data({d, d}) {}
  Data(size_t n) : use_second(true), data({std::vector<double>(n, 0), std::vector<double>(n, 0)}) {}
  Data(const Data& x) : use_second(true), data(x.data) {}
  
  double get(size_t i) const {
    mtx.lock();
    double x = data[use_second][i];
    mtx.unlock();
    return x;
  }
  
  void set(const std::vector<double>& newdata) {
    data[not use_second] = newdata;
    mtx.lock();
    use_second = not use_second;
    mtx.unlock();
  }
  
  size_t N() const {mtx.lock(); auto x = data[use_second].size(); mtx.unlock(); return x;}
};  // Data


/** LineGetter is a function pointer required by the plotting tool */
typedef ImPlotPoint (* LineGetter)(void *, int);

class Line {
  std::string mname;
  Data xval;
  Data yval;
  size_t running_avg;
  double xscale;
  double yscale;
  
  double x(size_t i) const {return xval.get(i);}
  double y(size_t i) const {return yval.get(i);}
  
  ImPlotPoint avg(size_t i) const {
    ImPlotPoint out{0.0, 0.0};
    size_t num{0};
    while (num < running_avg) {
      out.x += x(running_avg*i+num);
      out.y += y(running_avg*i+num);
      num++;
    }
    out.x *= xscale / running_avg;
    out.y *= yscale / running_avg;
    return out;
  }
  
  LineGetter avgfun() const {
    return [](void * data, int i){return static_cast<Line *>(data) -> avg(i);};
  }

public:
  Line(const std::string& name, const Data& X, const Data& Y) : mname(name), xval(X), yval(Y), running_avg(1), xscale(1.0f), yscale(1.0f) {
    if (X.N() not_eq Y.N())
      throw std::runtime_error("Bad data size");
  }
  void setX(const std::vector<double>& x) {xval.set(x);}
  void setY(const std::vector<double>& y) {yval.set(y);}
  int size() const {return yval.N() / running_avg;}
  LineGetter getter() const {return avgfun();}
  const std::string& name() const {return mname;}
};  // Line

class Frame {
  std::string mtitle;
  std::string mxlabel;
  std::string mylabel;
  std::vector<Line> frame;
public:
  Frame(const std::string& title, const std::string& xlabel, const std::string& ylabel, const std::vector<Line>& plotdata) :
  mtitle(title), mxlabel(xlabel), mylabel(ylabel), frame(plotdata) {}
  size_t size() const {return frame.size();}
  const std::string& title() const {return mtitle;}
  const std::string& xlabel() const {return mxlabel;}
  const std::string& ylabel() const {return mylabel;}
  decltype(frame.begin()) begin() {return frame.begin();}
  decltype(frame.end()) end() {return frame.end();}
  decltype(frame.cbegin()) cbegin() const {return frame.cbegin();}
  decltype(frame.cend()) cend() const {return frame.cend();}
  Line& operator[](size_t i)  {return frame[i];}
};  // Frame

void plot_frame(Frame& frame);

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
  
  CAHA(std::string plotname, const std::vector<std::vector<double>>& f) : mname(plotname), raw("tmp", "tmp", "tmp", {}),
  noise("tmp", "tmp", "tmp", {}), integration("tmp", "tmp", "tmp", {}), averaging("tmp", "tmp", "tmp", {}) {
    std::vector<Line> raws;
    std::vector<Line> noises;
    std::vector<Line> averagings;
    std::vector<Line> integrations;
    for (size_t i=0; i<f.size(); i++) {
      raws.push_back(GUI::Plotting::Line(std::string{"Cold "} + std::to_string(i), f[i], f[i].size()));
      raws.push_back(GUI::Plotting::Line(std::string{"Target "} + std::to_string(i), f[i], f[i].size()));
      raws.push_back(GUI::Plotting::Line(std::string{"Hot "} + std::to_string(i), f[i], f[i].size()));
      noises.push_back(GUI::Plotting::Line(std::string{"System Noise "} + std::to_string(i), f[i], f[i].size()));
      averagings.push_back(GUI::Plotting::Line(std::string{"Average Measurement "} + std::to_string(i), f[i], f[i].size()));
      integrations.push_back(GUI::Plotting::Line(std::string{"Last Measurement "} + std::to_string(i), f[i], f[i].size()));
    }
    raw = Frame("Raw", "Frequency", "Counts", raws);
    noise = Frame("Noise", "Frequency", "Temperature [K]", noises);
    averaging = Frame("Averaging", "Frequency", "Temperature [K]", averagings);
    integration = Frame("Integration", "Frequency", "Temperature [K]", integrations);
  }
  
  Frame& Raw() {return raw;}
  Frame& Noise() {return noise;}
  Frame& Averaging() {return averaging;}
  Frame& Integration() {return integration;}
  
  const Frame& Raw() const {return raw;}
  const Frame& Noise() const {return noise;}
  const Frame& Averaging() const {return averaging;}
  const Frame& Integration() const {return integration;}
  
  void plot(GLFWwindow* window, const ImVec2 startpos) {
    if (GUI::Windows::sub<2, Height, 0, 0, 1, PartOfHeight/2>(window, startpos, (mname + std::string{"raw"}).c_str())) {
      plot_frame(raw);
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 0, PartOfHeight/2, 1, PartOfHeight/2>(window, startpos, (mname + std::string{"noise"}).c_str())) {
      plot_frame(noise);
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 1, 0, 1, PartOfHeight/2>(window, startpos, (mname + std::string{"integration"}).c_str())) {
      plot_frame(integration);
    } GUI::Windows::end();
    if (GUI::Windows::sub<2, Height, 1, PartOfHeight/2, 1, PartOfHeight/2>(window, startpos, (mname + std::string{"averaging"}).c_str())) {
      plot_frame(averaging);
    } GUI::Windows::end();
  }
  
  const std::string& name() const {return mname;}
};

template <size_t Height, size_t PartOfHeight, size_t N>
void plot_combined(GLFWwindow* window, const ImVec2 startpos, std::array<CAHA<Height, PartOfHeight>, N>& frames) {
  if (GUI::Windows::sub<1, Height, 0, 0, 1, PartOfHeight/3>(window, startpos, "Combined Last Measurement Integration TILE")) {
    if (ImPlot::BeginPlot(frames[0].Integration().title().c_str(), frames[0].Integration().xlabel().c_str(), frames[0].Integration().ylabel().c_str(), {-1, -1})) {
      for (auto& frame: frames)
        for (auto& line: frame.Integration())
          ImPlot::PlotLine((frame.name() + std::string{" "} + line.name()).c_str(), line.getter(), (void*)&line, line.size());
      ImPlot::EndPlot();
    }
  } GUI::Windows::end();
  if (GUI::Windows::sub<1, Height, 0, PartOfHeight/3, 1, PartOfHeight/3>(window, startpos, "Combined Average Measurement Integration TILE")) {
    if (ImPlot::BeginPlot(frames[0].Averaging().title().c_str(), frames[0].Averaging().xlabel().c_str(), frames[0].Averaging().ylabel().c_str(), {-1, -1})) {
      for (auto& frame: frames)
        for (auto& line: frame.Averaging())
          ImPlot::PlotLine((frame.name() + std::string{" "} + line.name()).c_str(), line.getter(), (void*)&line, line.size());
      ImPlot::EndPlot();
    }
  } GUI::Windows::end();
  if (GUI::Windows::sub<1, Height, 0, 2*PartOfHeight/3, 1, PartOfHeight/3>(window, startpos, "Combined Noise TILE")) {
    if (ImPlot::BeginPlot(frames[0].Noise().title().c_str(), frames[0].Noise().xlabel().c_str(), frames[0].Noise().ylabel().c_str(), {-1, -1})) {
      for (auto& frame: frames)
        for (auto& line: frame.Noise())
          ImPlot::PlotLine((frame.name() + std::string{" "} + line.name()).c_str(), line.getter(), (void*)&line, line.size());
      ImPlot::EndPlot();
    }
  } GUI::Windows::end();
}

}  // Plotting
}  // GUI

#endif  // gui_plotting_h
