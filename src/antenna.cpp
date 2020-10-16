#include "antenna.h"

#include <cmath>

#include "mathhelpers.h"

std::vector<Sensor::Antenna::Output> calc_airy_disk(
    const Geom::Nav& pos_los, const Atmosphere::Atm& atm,
    const Distance<DistanceType::meter> dist,
    const Altitude<AltitudeType::meter> alt_of_atm, double disksize,
    double wavelen, std::size_t peaks, std::size_t num,
    std::size_t azimuthals) {
  // Size of the problem
  const std::size_t N = 1 + (num - 1) * azimuthals;
  const std::vector<double> arot = linspace<double>(0, 360, azimuthals + 1);
  std::vector<Sensor::Antenna::Output> out;
  out.reserve(N);

  // Bessel part of problem
  const double kD = disksize * Constant::two_pi / wavelen;
  const double thmax =
      std::asin(Constant::bessel_1storder_1stkind_zeroes[peaks - 1] / kD);
  const std::vector<double> x = linspace<double>(0, thmax, num + 1);
  std::vector<double> y(num);
  for (std::size_t i = 1; i < num; i++) {
    const double sinth = std::sin(x[i]);
    y[i] = Constant::pow2(2 * BesselJ1(kD * sinth) / (kD * sinth));
  }
  y[0] = 1.0;  // div-0, but x / sinx is 1
  const double invsumy = 1.0 / sum(y);
  const double invsumy_azimuth = invsumy / azimuthals;

  auto [first_nav, first_bg] =
      Path::calc_single_geometric_path(pos_los, atm, dist, alt_of_atm);
  out.push_back(Sensor::Antenna::Output{first_nav, first_bg, invsumy});
  for (std::size_t i = 1; i < num; i++) {
    for (std::size_t j = 0; j < azimuthals; j++) {
      const Geom::Nav new_pos_los(pos_los, x[i], arot[j]);
      auto [this_nav, this_bg] =
          Path::calc_single_geometric_path(pos_los, atm, dist, alt_of_atm);
      out.push_back(
          Sensor::Antenna::Output{this_nav, this_bg, invsumy_azimuth * y[i]});
    }
  }

  return out;
}

std::vector<Sensor::Antenna::Output> Sensor::Antenna::calc(
    const Geom::Nav& pos_los, const Atmosphere::Atm& atm,
    const Distance<DistanceType::meter> dist,
    const Altitude<AltitudeType::meter> alt_of_atm,
    const Frequency<FrequencyType::Freq> mean_freq) const {
  switch (mtype) {
    case BeamType::PencilBeam: {
      auto [path, backg] =
          Path::calc_single_geometric_path(pos_los, atm, dist, alt_of_atm);
      return std::vector<Sensor::Antenna::Output>(1, Output{path, backg, 1});
    }
    case BeamType::AiryDisk:
      return calc_airy_disk(
          pos_los, atm, dist, alt_of_atm, data.airy_disk.disksize,
          Conversion::freq2wavelen(mean_freq), data.airy_disk.num_peaks,
          data.airy_disk.num_points, data.airy_disk.num_azimuth);
    case BeamType::FINAL: { /* Leave last */
    }
  }

  return {};
}
