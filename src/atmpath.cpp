#include "atmpath.h"

namespace Path {
  std::pair<std::vector<Point>, BackgroundType> calc_single_geometric_path(
    Geom::Nav nav, const Atmosphere::Atm& atm,
    const Distance<DistanceType::meter> dist,
    const Altitude<AltitudeType::meter> alt_of_atm) {
  if (dist == 0.0)
    throw std::runtime_error(
        "Cannot perform calculations with 0-distance moves");

  // Get ellipsoidal position
  auto pos = nav.ellipsoidPos();

  // Move to the top of the atmosphere if outside it
  if (pos.h() > alt_of_atm) {
    bool hit_atmosphere = nav.move(alt_of_atm, true);
    if (not hit_atmosphere) return {};  // Leave if we cannot hit the atmosphere
  }

  // The first position
  std::vector<Point> out;
  out.reserve(100);
  std::pair<Atmosphere::InterPoints, Atmosphere::Point> data = atm(pos);
  out.push_back(Point{nav, data});

  // Move the position until we hit the surface or leave the atmosphere
  for (;;) {
    bool hit_surface = nav.move(dist);
    pos = nav.ellipsoidPos();
    data = atm(pos);
    out.push_back({nav, data});
    if (hit_surface or pos.h() > alt_of_atm) break;
  }

  // Replace the last position with one that is at the atmosphere boundary
  // if the position is outside the atmosphere
  pos = nav.ellipsoidPos();
  if (pos.h() > alt_of_atm) {
    nav.move(alt_of_atm, false);
    pos = nav.ellipsoidPos();
    data = atm(pos);
    out.back() = Point{nav, data};
  }

  if (pos.h() > alt_of_atm)
    return {out, BackgroundType::Space};
  else 
    return {out, BackgroundType::Surface};
}

Distance<DistanceType::meter> dist(const Point& a, const Point& b) noexcept {
  return std::hypot(a.nav.x() - b.nav.x(), a.nav.y() - b.nav.y(),
                    a.nav.z() - b.nav.z());
}
}  // namespace Path
