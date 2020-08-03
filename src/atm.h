#ifndef atm_h
#define atm_h

#include <array>
#include <iostream>
#include <vector>

#include "constants.h"
#include "enums.h"
#include "grids.h"
#include "units.h"
#include "species.h"
#include "timeclass.h"
#include "vectors.h"

namespace Atmosphere {

class LazyAtmPoint;

class AtmPoint {
  friend class LazyAtmPoint;
  
  Pressure<PressureType::Pa> P;
  Temperature<TemperatureType::K> T;
  Magnetism<MagnetismType::T> M;
  Wind<WindType::meters_per_second> W;
  std::vector<VMR<VMRType::ratio>> vmr;
public:
  
  friend std::ostream& operator<<(std::ostream& os, AtmPoint p) {
    os << p.P << ' ' << p.T << ' ' << p.M << ' ' << p.W;
    for (auto x: p.vmr)
      os << ' ' << x;
    return os;
  }
  
  AtmPoint& operator+=(const LazyAtmPoint& x) noexcept;
  
  friend AtmPoint operator*(double x, const AtmPoint& ap) noexcept {
    AtmPoint out{ap};
    return out *= x;
  }
  
private:
  AtmPoint& operator*=(double x) noexcept {
    P *= x;
    T *= x;
    M *= x;
    W *= x;
    for (auto& v: vmr)
      v *= x;
    return *this;
  }
};  // AtmPoint

class LazyAtmPoint {
  const AtmPoint& ap;
  double scale;
public:
  LazyAtmPoint(const AtmPoint& a, double x) noexcept : ap(a), scale(x) {}
  typename std::remove_reference<decltype(ap.P)>::type P() const noexcept {auto out = ap.P; return out *= scale;}
  typename std::remove_reference<decltype(ap.T)>::type T() const noexcept {auto out = ap.T; return out *= scale;}
  typename std::remove_reference<decltype(ap.M)>::type M() const noexcept {auto out = ap.M; return out *= scale;}
  typename std::remove_reference<decltype(ap.W)>::type W() const noexcept {auto out = ap.W; return out *= scale;}
  typename std::remove_reference<decltype(ap.vmr[0])>::type vmr(size_t i) const noexcept {auto out{ap.vmr[i]}; return out *= scale;;}
};  // LazyAtmPoint

class Atm {
  Vector::UniqueSorted<Time> tid;
  Vector::UniqueSorted<Altitude<AltitudeType::meter>> alt;
  Vector::CyclicDegreesUniqueSorted<Coordinate<CoordinateType::deg>> lat;
  Vector::CyclicDegreesUniqueSorted<Coordinate<CoordinateType::deg>> lon;
  Grid<AtmPoint, 4> data;
public:
  
  friend std::ostream& operator<<(std::ostream& os, const Atm& a) {
    for (decltype(a.tid.size()) i=0; i<a.tid.size(); i++) {
      for (decltype(a.alt.size()) j=0; j<a.alt.size(); j++) {
        for (decltype(a.lat.size()) k=0; k<a.lat.size(); k++) {
          for (decltype(a.lon.size()) m=0; m<a.lon.size(); m++) {
            os << a.tid[i] << ' ' << a.alt[i] << ' ' << a.lat[j] << ' ' << a.lon[k] << a.data(i, j, k, m) << '\n';
          }
        }
      }
    }
    return os;
  }
  
  AtmPoint operator()(Time newtid, Altitude<AltitudeType::meter> newalt, Coordinate<CoordinateType::deg> newlat, Coordinate<CoordinateType::deg> newlon) const;
};  // Atm
}  // Atmosphere

#endif  // atm_h
