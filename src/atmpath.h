#ifndef atmpath_h
#define atmpath_h

#include "geom.h"
#include "atm.h"

namespace Path {
struct Point {
  Geom::Nav nav;
  Atmosphere::Point atm;
  friend std::ostream& operator<<(std::ostream& os, const Point& p) {return os << p.nav << ' ' << p.atm;}
};

std::vector<Point> calc_single_geometric_path(Geom::Nav nav,
                                              const Atmosphere::Atm& atm,
                                              const Distance<DistanceType::meter> dist,
                                              const Altitude<AltitudeType::meter> alt_of_atm);
}

#endif  // atmpath_h
