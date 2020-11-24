#ifndef plot_h
#define plot_h

#include <mutex>
#include <vector>

#include <file.h>

namespace Plot {
class AxisData {
  std::vector<bool> x;
  std::vector<bool> y;
  std::vector<std::string> name;
  std::vector<std::vector<double>> data;
public:
  int size() const noexcept {return int(data.size());}
  int datasize(int i) const noexcept {return data[i].size();}
  
  AxisData(std::vector<std::vector<double>>&& d) : x(d.size(), false), y(d.size(), false), name(d.size()), data(std::move(d)) {
    for (int i=0; i<size(); i++) {
      name[i] = std::string("line ") + std::to_string(i);
    }
  }
  AxisData(std::vector<std::string>&& k, std::vector<std::vector<double>>&& d) : x(d.size(), false), y(d.size(), false), name(std::move(k)), data(std::move(d)) {}
  AxisData(const AxisData& d) = default;
  AxisData& operator=(const AxisData& d) = default;
  AxisData() noexcept {}
  
  std::pair<double*, int> xaxis() {
    for (int i=0; i<size(); i++) {
      if (x[i]) return {data[i].data(), int(data[i].size())};
    }
    return {nullptr, 0};
  }
  
  std::pair<double*, int> yaxis(int i) {
    if (y[i])
      return {data[i].data(), int(data[i].size())};
    else 
      return {nullptr, 0};
  }
  
  bool isx(int i) {return x[i];}
  
  bool isy(int i) {return y[i];}
  
  void setx(int i) {x[i]=not x[i];}
  
  void sety(int i) {y[i]=not y[i];}
  
  std::string& line(int i) {return name[i];}
  
  std::vector<double>& get(int i) {return data[i];}
  
  bool is_regular() const noexcept {
    if (auto n = size(); n) {
      auto N = data[0].size();
      for (decltype(n) i=1; i<n; i++) if (N not_eq data[i].size()) return false;
      return true;
    } else {
      return true;
    }
  }
};
  
AxisData read_raw(const std::string& file, int skiprows);

void menubar(AxisData& d, std::string& x, std::string& y);

void plot(AxisData& d, std::string& x, std::string& y);

void draw(AxisData& d, std::string& x, std::string& y);
}

#endif  // plot_h
