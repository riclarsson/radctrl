#ifndef instrument_h
#define instrument_h

#include <functional>
#include <vector>

namespace Instrument {
template <class SensorClass>
class Sensor {
  SensorClass sens;

public:
  Sensor(SensorClass&& s) noexcept : sens(std::move(s)) {}

  template <typename ... A>
  void startup(A ... args) {sens.startup(args ...);}

  template <typename ... A>
  void init(A ... args) {sens.init(args ...);}

  template <typename ... A>
  void run(A ... args) {sens.run(args ...);}

  template <typename ... A>
  void close(A ... args) {sens.close(args ...);}

  template <typename ... A>
  typename SensorClass::DataType get(A ... args) {return sens.get_data(args ...);}

  template <typename ... A>
  std::vector<double> datavec(A ... args) {return sens.get_data(args ...);}
};

template <typename First, typename ... Others>
struct Backends {
  static constexpr unsigned N = sizeof...(Others);
  static constexpr bool is_housekeeper = false;
  static constexpr bool is_chopping = false;
  static constexpr bool is_wobbling = false;
  static constexpr bool is_frontend = false;
  static constexpr bool is_backend = First::is_backend and (N>1) ? Backends<Others...>::is_backend : true;
  static_assert(is_backend);

  First first;
  Backends<Others...> others;

  Backends(const First& f, const Others& ... o) noexcept : first(f), others(o...) {}

  void run() {first.run(); if constexpr (N > 1) others.run();}
  void init() {first.init(); if constexpr (N > 1) others.init();}
  void close() {first.close(); if constexpr (N > 1) others.close();}
  std::vector<double> data(int i) noexcept {if (not i) return first.datavec(); else if (i > 0) return others.data(i-1); else std::exit(1);}
};
}

#endif  // instrument_h
