#ifndef plot_h
#define plot_h

#include <mutex>
#include <vector>

#include <file.h>

namespace Plot {
class AxisData {
  mutable std::mutex mtx;
  std::vector<bool> x;
  std::vector<bool> y;
  std::vector<std::vector<double>> data;
public:
  AxisData(std::vector<std::vector<double>>&& d) : x(d.size(), false), y(d.size(), false), data(std::move(d)) {}
  AxisData(const AxisData& d) noexcept { d.mtx.lock(); x = d.x; y = d.y; data = d.data; d.mtx.unlock(); }
  AxisData& operator=(const AxisData& d) noexcept { d.mtx.lock(); mtx.lock(); x = d.x; y = d.y; data = d.data; mtx.unlock(); d.mtx.unlock(); return *this; }
  AxisData() noexcept {}
};
  
AxisData read_raw(const std::string& file, std::size_t skiprows);

void menubar(AxisData& d, std::string& x, std::string& y);

void plot(AxisData& d, std::string& x, std::string& y);

void draw(AxisData& d, std::string& x, std::string& y);
}

#endif  // plot_h
