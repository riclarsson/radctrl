#ifndef jacobian_h
#define jacobian_h

#include <Eigen/Core>

#include "atmpath.h"
#include "derivatives.h"
#include "grids.h"

namespace Derivative {
ENUMCLASS(PolarizationType, unsigned char, I, Q, U, V, IpQ, ImQ, IpU, ImU, IpV,
          ImV)

struct Polarization {
  PolarizationType pol;
  Grid<double, 2> rotation;

  Polarization(std::size_t N,
               double circular_rotation [[maybe_unused]]) noexcept
      : rotation(0, N, N) {
    if (N == 4) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = std::cos(2 * circular_rotation);
      rotation(1, 2) = -std::sin(2 * circular_rotation);
      rotation(2, 1) = std::sin(2 * circular_rotation);
      rotation(3, 3) = 1;
    } else if (N == 3) {
      rotation(0, 0) = 1;
      rotation(1, 1) = rotation(2, 2) = std::cos(2 * circular_rotation);
      rotation(1, 2) = -std::sin(2 * circular_rotation);
      rotation(2, 1) = std::sin(2 * circular_rotation);
    } else if (N == 2) {
      rotation(0, 0) = 1;
      rotation(1, 1) = std::cos(2 * circular_rotation);  // Destructive...
    } else if (N == 2) {
      rotation(0, 0) = 1;
    }
  }
};

class Jacobian {
  Eigen::MatrixXd mjac;
  std::vector<Target> targets;
  std::vector<Path::Point> points;
  std::vector<double> measurement_grid;
  std::vector<Polarization> polarization;

 public:
  Jacobian(const std::vector<Target> &t, const std::vector<Path::Point> &p,
           const std::vector<double> &g,
           const std::vector<Polarization> &polar) noexcept
      : mjac(Eigen::MatrixXd::Zero(polar.size() * g.size(),
                                   t.size() * p.size())),
        targets(t),
        points(p),
        measurement_grid(g),
        polarization(polar) {}
};
}  // namespace Derivative

#endif  // jacobian_h
