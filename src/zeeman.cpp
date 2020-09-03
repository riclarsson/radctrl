#include "zeeman.h"

#include <Eigen/Dense>

namespace Absorption {
namespace Zeeman {
Eigen::Vector3d los_xyz_by_uvw_local(double u, double v, double w) {
  return Eigen::Vector3d(v, u, w).normalized();
}

Eigen::Vector3d los_xyz_by_za_local(double z, double a) {
  using Conversion::cosd;
  using Conversion::sind;
  return Eigen::Vector3d(cosd(a) * sind(z), sind(a) * sind(z), cosd(z));
}

Eigen::Vector3d ev_xyz_by_za_local(double z, double a) {
  using Conversion::cosd;
  using Conversion::sind;
  return Eigen::Vector3d(cosd(a) * cosd(z), sind(a) * cosd(z), -sind(z));
}

Calc angles_with_derivatives(Magnetism<MagnetismType::T> mag,
                             Angle<AngleType::deg> z,
                             Angle<AngleType::deg> a) noexcept {
  using Constant::pow2;
  using Conversion::acosd;
  using Conversion::atan2d;
  using Conversion::rad2deg;

  Calc output;

  // XYZ vectors normalized
  const Eigen::Vector3d n = los_xyz_by_za_local(z, a);
  const Eigen::Vector3d ev = ev_xyz_by_za_local(z, a);
  const Eigen::Vector3d nH = los_xyz_by_uvw_local(mag.u(), mag.v(), mag.w());

  output.calc.H = std::hypot(mag.u(), mag.v(), mag.w());
  if (output.calc.H == 0) {
    return {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  }

  // Normalized vector (which are also the magnetic field derivatives)
  output.dv.H = nH[0];
  output.du.H = nH[1];
  output.dw.H = nH[2];

  // Compute theta (and its derivatives if possible)
  const double cos_theta = n.dot(nH);
  output.calc.theta = acosd(cos_theta);
  if (cos_theta not_eq 1 and cos_theta not_eq -1) {
    const Eigen::Vector3d dtheta =
        (nH * cos_theta - n) / (output.calc.H * std::sqrt(1 - pow2(cos_theta)));
    output.dv.theta = dtheta[0];
    output.du.theta = dtheta[1];
    output.dw.theta = dtheta[2];
  } else {
    output.dv.theta = 0;
    output.du.theta = 0;
    output.dw.theta = 0;
  }

  // Compute eta (and its derivatives if possible)
  const Eigen::Vector3d inplane = nH - nH.dot(n) * n;
  const double y = ev.cross(inplane).dot(n);
  const double x = ev.dot(inplane);
  output.calc.eta = atan2d(y, x);
  if (x not_eq 0 or y not_eq 0) {
    const Eigen::Vector3d deta =
        n.cross(nH) / (output.calc.H * (pow2(x) + pow2(y)));
    output.dv.eta = deta[0];
    output.du.eta = deta[1];
    output.dw.eta = deta[2];
  } else {
    output.dv.eta = 0;
    output.du.eta = 0;
    output.dw.eta = 0;
  }

  return output;
}
}  // namespace Zeeman
}  // namespace Absorption
