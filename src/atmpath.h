#ifndef atmpath_h
#define atmpath_h

#include "atm.h"
#include "geom.h"

namespace Path {
struct Point {
  Geom::Nav nav;
  Atmosphere::InterPoints ip;
  Atmosphere::Point atm;

  Point(
      const Geom::Nav &n,
      const std::pair<Atmosphere::InterPoints, Atmosphere::Point> &ipp) noexcept
      : nav(n), ip(ipp.first), atm(ipp.second) {}

  Point(const Geom::Nav &n, const Atmosphere::Point &p) noexcept
      : nav(n), atm(p) {}

  friend std::ostream &operator<<(std::ostream &os, const Point &p) {
    return os << p.nav << ' ' << p.atm;
  }

  double DopplerShiftRatio() const noexcept {
    const auto los = nav.sphericalLos();
    return atm.DopplerShiftRatio(los.za(), los.aa());
  }

  double DopplerShiftRatioDerivativeU() const noexcept {
    const auto los = nav.sphericalLos();
    return atm.DopplerShiftRatioDerivativeU(los.za(), los.aa());
  }

  double DopplerShiftRatioDerivativeV() const noexcept {
    const auto los = nav.sphericalLos();
    return atm.DopplerShiftRatioDerivativeV(los.za(), los.aa());
  }

  double DopplerShiftRatioDerivativeW() const noexcept {
    const auto los = nav.sphericalLos();
    return atm.DopplerShiftRatioDerivativeW(los.za(), los.aa());
  }
};

std::vector<Point> calc_single_geometric_path(
    Geom::Nav nav, const Atmosphere::Atm &atm,
    const Distance<DistanceType::meter> dist,
    const Altitude<AltitudeType::meter> alt_of_atm);

Distance<DistanceType::meter> dist(const Point &a, const Point &b) noexcept;
}  // namespace Path

#endif  // atmpath_h
