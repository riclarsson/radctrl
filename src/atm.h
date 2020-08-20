#ifndef atm_h
#define atm_h

#include <array>
#include <iostream>
#include <vector>

#include "constants.h"
#include "enums.h"
#include "file.h"
#include "grids.h"
#include "species.h"
#include "timeclass.h"
#include "units.h"

namespace Atmosphere {
class LazyPoint;

class Point {
  friend class LazyPoint;

  Pressure<PressureType::Pa> P;
  Temperature<TemperatureType::K> T;
  Magnetism<MagnetismType::T> M;
  Wind<WindType::meters_per_second> W;
  std::vector<VMR<VMRType::ratio>> vmr;

 public:
  Point(Pressure<PressureType::Pa> p, Temperature<TemperatureType::K> t,
        Magnetism<MagnetismType::T> m, Wind<WindType::meters_per_second> w,
        const std::vector<VMR<VMRType::ratio>>& v) noexcept
      : P(p), T(t), M(m), W(w), vmr(v) {}

  Point(decltype(vmr)::size_type n = 0) noexcept
      : M({0, 0, 0}), W({0, 0, 0}), vmr(n) {}
  Point(const std::vector<Species::Isotope>& s) noexcept
      : M({0, 0, 0}), W({0, 0, 0}), vmr(s.size()) {
    for (size_t i = 0; i < s.size(); i++) vmr[i].isot(s[i]);
  }

  Point(Point&& ap) = default;
  Point(const Point& ap) = default;
  Point& operator=(const Point& ap) = default;
  Point& operator=(Point&& ap) = default;

  decltype(vmr)::size_type size() const { return vmr.size(); }
  void resize(decltype(vmr)::size_type n) { vmr.resize(n); }

  friend std::ostream& operator<<(std::ostream& os, Point p) {
    os << p.P << ' ' << p.T << ' ' << p.M << ' ' << p.W;
    for (auto x : p.vmr) os << ' ' << x;
    return os;
  }

  friend std::istream& operator>>(std::istream& is, Point& p) {
    is >> p.P >> p.T >> p.M >> p.W;
    for (auto& x : p.vmr) is >> x;
    return is;
  }

  Point& operator+=(const LazyPoint& x) noexcept;

  /** Fix that must be called after using any arithmetic */
  void expP() { P = std::exp(P); }

  Pressure<PressureType::Pa> Pres() const { return P; }
  Temperature<TemperatureType::K> Temp() const { return T; }
  Magnetism<MagnetismType::T> MagField() const { return M; }
  Wind<WindType::meters_per_second> WindField() const { return W; }
  const std::vector<VMR<VMRType::ratio>>& VolumeMixingRatios() const {
    return vmr;
  }

  friend Point operator*(double x, const Point& ap) noexcept {
    Point out{ap};

    out.P = x * std::log(out.P);
    out.T *= x;
    out.M *= x;
    out.W *= x;
    for (auto& v : out.vmr) v *= x;
    return out;
  }

  friend void savePoint(
      File::File<File::Operation::Write, File::Type::Xml>& file,
      const Point& ap) {
    file.new_child("Point");
    file.add_attribute("size", ap.size());
    file.add_attribute("Species", ap.specs());
    file << '\n';
    ap.savePureAscii(file);
    file << '\n';
    file.leave_child();
  }

  friend void savePoint(
      File::File<File::Operation::WriteBinary, File::Type::Xml>& file,
      const Point& ap) {
    file.new_child("Point");
    file.add_attribute("size", ap.size());
    file.add_attribute("Species", ap.specs());
    ap.saveBinary(file);
    file.leave_child();
  }

  friend void readPoint(
      File::File<File::Operation::Read, File::Type::Xml>& file, Point& ap) {
    auto child = file.get_child("Point");
    ap.vmr.resize(file.size());
    auto isot = file.get_vector_attribute<Species::Isotope>("Species");
    for (size_t i = 0; i < isot.size(); i++) {
      ap.vmr[i].isot(isot[i]);
    }
    std::istringstream data(child.text().as_string());
    ap.readPureAscii(data);
    file.leave_child();
  }

  friend void readPoint(
      File::File<File::Operation::ReadBinary, File::Type::Xml>& file,
      Point& ap) {
    file.get_child("Point");
    ap.vmr.resize(file.size());
    auto isot = file.get_vector_attribute<Species::Isotope>("Species");
    for (size_t i = 0; i < isot.size(); i++) {
      ap.vmr[i].isot(isot[i]);
    }
    ap.readBinary(file);
    file.leave_child();
  }

  std::vector<Species::Isotope> specs() const {
    std::vector<Species::Isotope> s;
    for (auto x : vmr) s.push_back(x.isot());
    return s;
  }

  template <typename Output>
  void savePureAscii(Output& file) const {
    file << P << ' ' << T << ' ' << M << ' ' << W;
    for (auto x : vmr) file << ' ' << x.value();
  }

  template <typename Output>
  void saveBinary(Output& file) const {
    file.write(P);
    file.write(T);
    file.write(M);
    file.write(W);
    for (auto x : vmr) file.write(x.value());
  }

  template <typename Input>
  void readPureAscii(Input& file) {
    file >> P >> T >> M >> W;
    for (auto& x : vmr) file >> x.value();
  }

  template <typename Input>
  void readBinary(Input& file) {
    file.read(P);
    file.read(T);
    file.read(M);
    file.read(W);
    for (auto& x : vmr) file.read(x.value());
  }
};  // Point

class LazyPoint {
  const Point& ap;
  double scale;

 public:
  LazyPoint(const Point& a, double x) noexcept : ap(a), scale(x) {}
  typename std::remove_reference<decltype(ap.P)>::type P() const noexcept {
    auto out = std::log(ap.P);
    return out *= scale;
  }
  typename std::remove_reference<decltype(ap.T)>::type T() const noexcept {
    auto out = ap.T;
    return out *= scale;
  }
  typename std::remove_reference<decltype(ap.M)>::type M() const noexcept {
    auto out = ap.M;
    return out *= scale;
  }
  typename std::remove_reference<decltype(ap.W)>::type W() const noexcept {
    auto out = ap.W;
    return out *= scale;
  }
  typename std::remove_reference<decltype(ap.vmr[0])>::type vmr(
      size_t i) const noexcept {
    auto out{ap.vmr[i]};
    return out *= scale;
    ;
  }
};  // LazyPoint

class Atm {
  std::vector<Time> tid;
  std::vector<Altitude<AltitudeType::meter>> alt;
  std::vector<Coordinate<CoordinateType::lat>> lat;
  std::vector<Coordinate<CoordinateType::lon>> lon;
  Grid<Point, 4> data;

  bool ok() const {
    auto specs = operator()(0, 0, 0, 0).specs();
    for (auto& x : data.data()) {
      if (specs not_eq x.specs()) return false;
    }
    return true;
  }

 public:
  Atm(const std::vector<Time>& t,
      const std::vector<Altitude<AltitudeType::meter>>& a,
      const std::vector<Coordinate<CoordinateType::lat>>& la,
      const std::vector<Coordinate<CoordinateType::lon>>& lo,
      const Grid<Point, 4>& d)
      : tid(t), alt(a), lat(la), lon(lo), data(d) {
    if (not ok()) throw std::runtime_error("Bad atmosphere");
  }
  Atm(std::size_t t = 0, std::size_t a = 0, std::size_t la = 0,
      std::size_t lo = 0, std::size_t s = 0)
      : tid(t), alt(a), lat(la), lon(lo), data(s, {t, a, la, lo}) {}

  friend std::ostream& operator<<(std::ostream& os, const Atm& a) {
    if (not a.ok()) throw std::runtime_error("Bad atmosphere");
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            os << a.tid[i] << ' ' << a.alt[j] << ' ' << a.lat[k] << ' '
               << a.lon[m] << ' ' << a.data(i, j, k, m);
            if (i < a.tid.size() - 1 or j < a.alt.size() - 1 or
                k < a.lat.size() - 1 or m < a.lon.size() - 1)
              os << '\n';
          }
        }
      }
    }
    return os;
  }

  friend std::istream& operator>>(std::istream& is, Atm& a) {
    if (not a.ok()) throw std::runtime_error("Bad atmosphere");
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            is >> a.tid[i] >> a.alt[j] >> a.lat[k] >> a.lon[m] >>
                a.data(i, j, k, m);
          }
        }
      }
    }
    return is;
  }

  Point& operator()(size_t i, size_t j, size_t k, size_t m) {
    return data(i, j, k, m);
  }
  const Point& operator()(size_t i, size_t j, size_t k, size_t m) const {
    return data(i, j, k, m);
  }

  Point operator()(Time newtid, Altitude<AltitudeType::meter> newalt,
                   Coordinate<CoordinateType::lat> newlat,
                   Coordinate<CoordinateType::lon> newlon) const;
  template <typename Pos>
  Point operator()(Pos pos) const {
    return operator()(pos.t(), pos.h(), pos.lat(), pos.lon());
  }

  friend void saveAtm(File::File<File::Operation::Write, File::Type::Xml>& file,
                      const Atm& a) {
    if (not a.ok()) throw std::runtime_error("Bad atmosphere");

    file.new_child("Atm");
    file.add_attribute("Time", a.tid.size());
    file.add_attribute("Altitudes", a.alt.size());
    file.add_attribute("Latitudes", a.lat.size());
    file.add_attribute("Longitudes", a.lon.size());
    file.add_attribute("Species", a(0, 0, 0, 0).specs());

    file.new_child("Time");
    file << '\n' << a.tid;
    file.leave_child();

    file.new_child("Altitudes");
    file << '\n' << a.alt;
    file.leave_child();

    file.new_child("Latitudes");
    file << '\n' << a.lat;
    file.leave_child();

    file.new_child("Longitudes");
    file << '\n' << a.lon;
    file.leave_child();

    file.new_child("Data");
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            file << '\n';
            a(i, j, k, m).savePureAscii(file);
          }
        }
      }
    }
    file << '\n';
    file.leave_child();

    file.leave_child();
  }

  friend void saveAtm(
      File::File<File::Operation::WriteBinary, File::Type::Xml>& file,
      const Atm& a) {
    if (not a.ok()) throw std::runtime_error("Bad atmosphere");

    file.new_child("Atm");
    file.add_attribute("Time", a.tid.size());
    file.add_attribute("Altitudes", a.alt.size());
    file.add_attribute("Latitudes", a.lat.size());
    file.add_attribute("Longitudes", a.lon.size());
    file.add_attribute("Species", a(0, 0, 0, 0).specs());

    file.new_child("Time");
    file.write(a.tid);
    file.leave_child();

    file.new_child("Altitudes");
    file.write(a.alt);
    file.leave_child();

    file.new_child("Latitudes");
    file.write(a.lat);
    file.leave_child();

    file.new_child("Longitudes");
    file.write(a.lon);
    file.leave_child();

    file.new_child("Data");
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            a(i, j, k, m).saveBinary(file);
          }
        }
      }
    }
    file.leave_child();

    file.leave_child();
  }

  friend void readAtm(File::File<File::Operation::Read, File::Type::Xml>& file,
                      Atm& a) {
    file.get_child("Atm");
    a.tid.resize(file.get_attribute("Time").as_int());
    a.alt.resize(file.get_attribute("Altitudes").as_int());
    a.lat.resize(file.get_attribute("Latitudes").as_int());
    a.lon.resize(file.get_attribute("Longitudes").as_int());
    auto specs = file.get_vector_attribute<Species::Isotope>("Species");

    file.get_child("Time");
    file >> a.tid;
    file.leave_child();

    file.get_child("Altitudes");
    file >> a.alt;
    file.leave_child();

    file.get_child("Latitudes");
    file >> a.lat;
    file.leave_child();

    file.get_child("Longitudes");
    file >> a.lon;
    file.leave_child();

    a.data.reset(a.tid.size(), a.alt.size(), a.lat.size(), a.lon.size(), specs);
    auto data = std::istringstream(file.get_child("Data").text().as_string());
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            a(i, j, k, m).readPureAscii(data);
          }
        }
      }
    }
    file.leave_child();

    file.leave_child();
  }

  friend void readAtm(
      File::File<File::Operation::ReadBinary, File::Type::Xml>& file, Atm& a) {
    file.get_child("Atm");
    a.tid.resize(file.get_attribute("Time").as_int());
    a.alt.resize(file.get_attribute("Altitudes").as_int());
    a.lat.resize(file.get_attribute("Latitudes").as_int());
    a.lon.resize(file.get_attribute("Longitudes").as_int());
    auto specs = file.get_vector_attribute<Species::Isotope>("Species");

    file.get_child("Time");
    file.read(a.tid);
    file.leave_child();

    file.get_child("Altitudes");
    file.read(a.alt);
    file.leave_child();

    file.get_child("Latitudes");
    file.read(a.lat);
    file.leave_child();

    file.get_child("Longitudes");
    file.read(a.lon);
    file.leave_child();

    a.data.reset(a.tid.size(), a.alt.size(), a.lat.size(), a.lon.size(), specs);
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            a(i, j, k, m).readBinary(file);
          }
        }
      }
    }
    file.leave_child();

    file.leave_child();
  }
};  // Atm
}  // namespace Atmosphere

#endif  // atm_h
