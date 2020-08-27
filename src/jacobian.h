#ifndef jacobian_h
#define jacobian_h

#include <Eigen/Core>

#include "atmpath.h"
#include "derivatives.h"
#include "grids.h"

namespace Derivative {
ENUMCLASS(PolarizationType, unsigned char, I, Q, U, V, IpQ, ImQ, IpU, ImU, IpV,
          ImV)

template <size_t N>
struct Polarization {
  PolarizationType pol;
  Grid<double, 2> rotation;
  
  Polarization(double circular_rotation [[maybe_unused]]) noexcept
  : rotation(0, N, N) {
    if constexpr (N == 4) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = std::cos(2 * circular_rotation);
      rotation(1, 2) = -std::sin(2 * circular_rotation);
      rotation(2, 1) = std::sin(2 * circular_rotation);
      rotation(3, 3) = 1;
    } else if constexpr (N == 3) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = std::cos(2 * circular_rotation);
      rotation(1, 2) = -std::sin(2 * circular_rotation);
      rotation(2, 1) = std::sin(2 * circular_rotation);
    } else if constexpr (N == 2) {
      rotation(0, 0) = 1;
      rotation(1, 1) = std::cos(2 * circular_rotation);  // Destructive...
    } else if constexpr (N == 2) {
      rotation(0, 0) = 1;
    }
  }
};
  
class Jacobian {
  Eigen::MatrixXd mjac;
  std::vector<Target> targets;
  std::vector<Path::Point> points;
  std::vector<double> measurement_grid;
  std::size_t num_meas_per_gridpoint;

 public:
  Jacobian(const std::vector<Target>& t, const std::vector<Path::Point>& p,
           const std::vector<double>& g,
           std::size_t n) noexcept
      : mjac(Eigen::MatrixXd::Zero(n * g.size(), t.size() * p.size())),
        targets(t),
        points(p),
        measurement_grid(g),
        num_meas_per_gridpoint(n) {}
};
}  // namespace Derivative

#endif  // jacobian_h
