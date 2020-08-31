#include "atmpath.h"

namespace Path {
std::vector<Point> calc_single_geometric_path(
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
  std::vector<Point> out{1, {nav, atm(pos)}};

  // Move the position until we hit the surface or leave the atmosphere
  for (;;) {
    bool hit_surface = nav.move(dist);
    pos = nav.ellipsoidPos();
    out.push_back({nav, atm(pos)});
    if (hit_surface or pos.h() > alt_of_atm) break;
  }

  // Replace the last position with one that is at the atmosphere boundary
  // if the position is outside the atmosphere
  pos = nav.ellipsoidPos();
  if (pos.h() > alt_of_atm) {
    nav.move(alt_of_atm, false);
    pos = nav.ellipsoidPos();
    out.back() = {nav, atm(pos)};
  }

  return out;
}

Distance<DistanceType::meter> dist(const Point& a, const Point& b) noexcept {
  return std::hypot(a.nav.x() - b.nav.x(), a.nav.y() - b.nav.y(),
                    a.nav.z() - b.nav.z());
}
}  // namespace Path
