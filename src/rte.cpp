#include "rte.h"

namespace RTE {
SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
B(Temperature<TemperatureType::K> T,
  Frequency<FrequencyType::Freq> f) noexcept {
  using Constant::c;
  using Constant::h;
  using Constant::k;
  using Constant::pow2;
  using Constant::pow3;
  using std::exp;
  return 2 * h * pow3(f) / (pow2(c) * (exp(h * f / (k * T)) - 1));
}

SpectralRadiance<PowerType::T, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
invB(SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                      FrequencyType::Freq>
         I,
     Frequency<FrequencyType::Freq> f) noexcept {
  using Constant::c;
  using Constant::h;
  using Constant::k;
  using Constant::pow2;
  using Constant::pow3;
  using std::log;
  [[likely]] if (I > 0.0) return h * f /
      (k * log(1 + 2 * h * pow3(f) / (I * pow2(c))));
  else return std::numeric_limits<double>::min();
}

SpectralRadiance<PowerType::T, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dinvBdI(SpectralRadiance<PowerType::W, SphericalAngleType::Steradian,
                         AreaType::m2, FrequencyType::Freq>
            I,
        Frequency<FrequencyType::Freq> f) noexcept {
  using Constant::c;
  using Constant::h;
  using Constant::k;
  using Constant::pow2;
  using Constant::pow3;
  using Constant::pow4;
  using std::log;
  [[likely]] if (I > 0.0) return 2 * pow2(h) * pow4(f) /
      (I * k * (I * pow2(c) + 2 * h * pow3(f)) *
       pow2(log((I * pow2(c) + 2 * h * pow3(f)) / (I * pow2(c)))));
  else return std::numeric_limits<double>::min();
}

SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dBdT(Temperature<TemperatureType::K> T,
     Frequency<FrequencyType::Freq> f) noexcept {
  using Constant::c;
  using Constant::h;
  using Constant::k;
  using Constant::pow2;
  using Constant::pow4;
  using std::exp;
  return 2 * pow4(f) * pow2(h) * exp(f * h / (T * k)) /
         (pow2(T) * pow2(c) * k * pow2(exp(f * h / (T * k)) - 1));
}

SpectralRadiance<PowerType::W, SphericalAngleType::Steradian, AreaType::m2,
                 FrequencyType::Freq>
dBdf(Temperature<TemperatureType::K> T,
     Frequency<FrequencyType::Freq> f) noexcept {
  using Constant::c;
  using Constant::h;
  using Constant::k;
  using Constant::pow2;
  using Constant::pow3;
  using std::exp;
  return 6 * pow2(f) * h / (pow2(c) * (exp(f * h / (T * k)) - 1)) -
         2 * pow3(f) * pow2(h) * exp(f * h / (T * k)) /
             (T * pow2(c) * k * pow2(exp(f * h / (T * k)) - 1));
}
}  // namespace RTE
