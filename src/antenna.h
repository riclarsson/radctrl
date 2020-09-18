#ifndef antenna_h
#define antenna_h

#include <Eigen/Dense>
#include <vector>

#include "atmpath.h"
#include "enums.h"

namespace Sensor {
ENUMCLASS(BeamType, unsigned char, PencilBeam, AiryDisk)

/* Antenna pattern calculations
 *
 * The calculations are simple but depend on BeamType, and each beam type has
 * different input requirements.
 *
 * If the beamtype is a pencil beam, no additional input is required and only
 * one path can be computed.
 *
 */
class Antenna {
  union Data {
    struct AiryDiskData {
      double disksize;
      std::size_t num_peaks;
      std::size_t num_points;
      std::size_t num_azimuth;
    };

    AiryDiskData airy_disk;

    constexpr Data() noexcept : airy_disk({0.0, 0, 0, 0}) {}

    constexpr Data(BeamType bt, double d, std::size_t npeaks, std::size_t n,
                   std::size_t naa) noexcept
        : airy_disk({d, npeaks, n, naa}) {
      if (BeamType::AiryDisk not_eq bt) {
        std::cerr << "Bad antenna type (not Airy disk)\n";
        std::terminate();
      }
    }
  };

  BeamType mtype;
  Data data;

 public:
  constexpr Antenna(BeamType bt) noexcept : mtype(bt) {
    if (mtype not_eq BeamType::PencilBeam) {
      std::cerr << "Bad antenna type (not pencil beam)\n";
      std::terminate();
    }
  }

  constexpr Antenna(BeamType bt, double d, std::size_t npeaks, std::size_t n,
                    std::size_t naa) noexcept
      : mtype(bt), data(bt, d, npeaks, n, naa) {
    if (mtype not_eq BeamType::AiryDisk) {
      std::cerr << "Bad antenna type (not airy disk)\n";
      std::terminate();
    }
  }

  BeamType Type() const noexcept { return mtype; }

  struct Output {
    std::vector<Path::Point> path;
    double weight;
  };

  std::vector<Output> calc(
      const Geom::Nav& pos_los, const Atmosphere::Atm& atm,
      const Distance<DistanceType::meter> dist,
      const Altitude<AltitudeType::meter> alt_of_atm,
      const Frequency<FrequencyType::Freq> mean_freq) const;
};

ENUMCLASS(PolarizationType, unsigned char, I, Q, U, V, IpQ, ImQ, IpU, ImU, IpV,
          ImV)

class Polarization {
  PolarizationType pt;
  Eigen::Matrix4d rotation;

 public:
  Polarization(const PolarizationType p,
               const Angle<AngleType::deg> circular_rotation = 0) noexcept
      : pt(p), rotation(Eigen::Matrix4d::Zero()) {
    using Conversion::cosd;
    using Conversion::sind;
    rotation(0, 0) = 1;
    rotation(1, 1) = rotation(2, 2) = cosd(2 * circular_rotation);
    rotation(1, 2) = -sind(2 * circular_rotation);
    rotation(2, 1) = sind(2 * circular_rotation);
    rotation(3, 3) = 1;
  }

  template <class T>
  double get_value(const T x) const {
    const Eigen::Vector4d tmp =
        (T::size() == 4)
            ? rotation * Eigen::Vector4d(x[0], x[1], x[2], x[3])
            : (T::size() == 3)
                  ? rotation * Eigen::Vector4d(x[0], x[1], x[2], 0)
                  : (T::size() == 2)
                        ? rotation * Eigen::Vector4d(x[0], x[1], 0, 0)
                        : rotation * Eigen::Vector4d(x[0], 0, 0, 0);
    switch (pt) {
      case PolarizationType::I:
        return tmp[0];
      case PolarizationType::Q:
        return tmp[1];
      case PolarizationType::U:
        return tmp[2];
      case PolarizationType::V:
        return tmp[3];
      case PolarizationType::IpQ:
        return tmp[0] + tmp[1];
      case PolarizationType::ImQ:
        return tmp[0] - tmp[1];
      case PolarizationType::IpU:
        return tmp[0] + tmp[2];
      case PolarizationType::ImU:
        return tmp[0] - tmp[2];
      case PolarizationType::IpV:
        return tmp[0] + tmp[3];
      case PolarizationType::ImV:
        return tmp[0] - tmp[3];
      case PolarizationType::FINAL: { /* Leave last */
      }
    }

    return std::numeric_limits<double>::quiet_NaN();
  }
};

ENUMCLASS (MeasurementUnit, char, Pure, PlanckBT)

struct Properties {
  MeasurementUnit unit;
  unsigned short stokes_dim;
  Antenna antenna;
  std::vector<Polarization> polar;
  std::vector<Frequency<FrequencyType::Freq>> f_grid;
};
}  // namespace Sensor

#endif  // antenna_h
