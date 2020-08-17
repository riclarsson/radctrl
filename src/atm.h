#ifndef atm_h
#define atm_h

#include <array>
#include <iostream>
#include <vector>

#include "constants.h"
#include "enums.h"
#include "file.h"
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
  AtmPoint(Pressure<PressureType::Pa> p, Temperature<TemperatureType::K> t,
           Magnetism<MagnetismType::T> m, Wind<WindType::meters_per_second> w,
           const std::vector<VMR<VMRType::ratio>>& v) noexcept : P(p), T(t), M(m), W(w), vmr(v) {}
  
  AtmPoint(decltype(vmr)::size_type n=0) noexcept : M({0,0,0}), W({0,0,0}), vmr(n) {}
  
  decltype(vmr)::size_type size() const {return vmr.size();}
  
  friend std::ostream& operator<<(std::ostream& os, AtmPoint p) {
    os << p.P << ' ' << p.T << ' ' << p.M << ' ' << p.W;
    for (auto x: p.vmr)
      os << ' ' << x;
    return os;
  }
  
  friend std::istream& operator>>(std::istream& is, AtmPoint& p) {
    is >> p.P >> p.T >> p.M >> p.W;
    for (auto& x: p.vmr)
      is >> x;
    return is;
  }
  
  AtmPoint& operator+=(const LazyAtmPoint& x) noexcept;
  
  friend AtmPoint operator*(double x, const AtmPoint& ap) noexcept {
    AtmPoint out{ap};
    return out *= x;
  }
  
  friend void saveAtmPoint(File::File<File::Operation::Write, File::Type::Xml>& file, const AtmPoint& ap) {
    file.new_child("AtmPoint");
    file.add_attribute("size", ap.size());
    file.add_attribute("Species", ap.specs());
    file << '\n';
    ap.savePureAscii(file);
    file << '\n';
    file.leave_child();
  }
  
  friend void readAtmPoint(File::File<File::Operation::Read, File::Type::Xml>& file, AtmPoint& ap) {
    auto child = file.get_child("AtmPoint");
    ap.vmr.resize(file.size());
    auto isot = file.get_vector_attribute<Species::Isotope>("Species");
    for (size_t i=0; i<isot.size(); i++) {ap.vmr[i].isot(isot[i]);}
    std::istringstream data(child.text().as_string());
    ap.readPureAscii(data);
    file.leave_child();
  }
  
private:
  template <typename Output>
  void savePureAscii(Output& file) const {
    file << P << ' ' << T << ' ' << M << ' ' << W;
    for (auto x: vmr) file << ' ' << x.value();
  }
  
  template <typename Input>
  void readPureAscii(Input& file) {
    file >> P >> T >> M >> W;
    for (auto& x: vmr) readRatioOnly(file, x);
  }
  
  std::vector<Species::Isotope> specs() const {
    std::vector<Species::Isotope> s;
    for (auto x: vmr) s.push_back(x.isot());
    return s;
  }
  
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
