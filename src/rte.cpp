#include "rte.h"

namespace RTE {
double B(Temperature<TemperatureType::K> T, Frequency<FrequencyType::Freq> f) noexcept {
  using std::exp;
  using Constant::c;
  using Constant::k;
  using Constant::h;
  using Constant::pow2;
  using Constant::pow3;
  return 2*h*pow3(f) / (pow2(c) * (exp(h*f / (k*T)) - 1));
}

double dBdT(Temperature<TemperatureType::K> T, Frequency<FrequencyType::Freq> f) noexcept {
  using std::exp;
  using Constant::c;
  using Constant::k;
  using Constant::h;
  using Constant::pow2;
  using Constant::pow4;
  return 2*pow4(f)*pow2(h)*exp(f*h/(T*k)) / (pow2(T)*pow2(c)*k*pow2(exp(f*h/(T*k)) - 1));
}

double dBdf(Temperature<TemperatureType::K> T, Frequency<FrequencyType::Freq> f) noexcept {
  using std::exp;
  using Constant::c;
  using Constant::k;
  using Constant::h;
  using Constant::pow2;
  using Constant::pow3;
  return 6*pow2(f)*h/(pow2(c)*(exp(f*h/(T*k)) - 1)) - 2*pow3(f)*pow2(h)*exp(f*h/(T*k)) / (T*pow2(c)*k*pow2(exp(f*h/(T*k)) - 1));
}
}  // RTE
