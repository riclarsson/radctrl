#ifndef gui_plotting_h
#define gui_plotting_h

#include <algorithm>
#include <array>
#include <mutex>
#include <vector>

#include <implot.h>

namespace GUI {
namespace Plotting {
class Data {
bool use_second;
size_t mN;
std::array<std::vector<float>, 2> data;
mutable std::mutex mtx;

public:
  Data(const std::vector<float>& d) : use_second(true), mN(d.size()), data({d, d}) {}
  Data(size_t N) : use_second(true), mN(N), data({std::vector<float>(N, 0), std::vector<float>(N, 0)}) {}
  Data(const Data& x) : use_second(true), mN(x.mN), data(x.data) {}
  
  float get(size_t i) const {
    mtx.lock();
    float x = data[use_second][i];
    mtx.unlock();
    return x;
  }
  
  void set(const std::vector<float>& newdata) {
    if (newdata.size() not_eq mN) 
      throw std::runtime_error("Bad data");
    
    data[not use_second] = newdata;
    mtx.lock();
    use_second = not use_second;
    mtx.unlock();
  }
  
  size_t N() const {return mN;}
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
  
  float x(size_t i) const {return xval.get(i);}
  float y(size_t i) const {return yval.get(i);}
  
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
  void setX(const std::vector<float>& x) {xval.set(x);}
  void setY(const std::vector<float>& y) {yval.set(y);}
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
};  // Frame

void plot_frame(Frame& frame);
}  // Plotting
}  // GUI

#endif  // gui_plotting_h
