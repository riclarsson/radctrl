#ifndef atm_h
#define atm_h

#include <array>
#include <iostream>
#include <vector>

#include "constants.h"
#include "enums.h"
#include "file.h"
#include "grids.h"
#include "mathhelpers.h"
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
  std::vector<NLTE<NLTEType::ratio>> nlte;

 public:
  Point(Pressure<PressureType::Pa> p, Temperature<TemperatureType::K> t,
        Magnetism<MagnetismType::T> m, Wind<WindType::meters_per_second> w,
        const std::vector<VMR<VMRType::ratio>> &v) noexcept
      : P(p), T(t), M(m), W(w), vmr(v), nlte(0) {}

  Point(decltype(vmr)::size_type n = 0) noexcept
      : M({0, 0, 0}), W({0, 0, 0}), vmr(n), nlte(0) {}

  Point(const std::vector<Species::Isotope> &s) noexcept
      : M({0, 0, 0}), W({0, 0, 0}), vmr(s.size()), nlte(0) {
    for (size_t i = 0; i < s.size(); i++) vmr[i].isot(s[i]);
  }

  Point(const Point &ap) noexcept
      : P(ap.P), T(ap.T), M(ap.M), W(ap.W), vmr(ap.vmr), nlte(ap.nlte) {}
  Point &operator=(const Point &ap) noexcept {
    P = ap.P;
    T = ap.T;
    M = ap.M;
    W = ap.W;
    vmr = ap.vmr;
    nlte = ap.nlte;
    return *this;
  }

  decltype(vmr)::size_type size() const { return vmr.size(); }

  void resize(decltype(vmr)::size_type n) { vmr.resize(n); }

  friend std::ostream &operator<<(std::ostream &os, Point p) {
    os << p.P << ' ' << p.T << ' ' << p.M << ' ' << p.W;
    for (auto x : p.vmr) os << ' ' << x;
    return os;
  }

  friend std::istream &operator>>(std::istream &is, Point &p) {
    is >> p.P >> p.T >> p.M >> p.W;
    for (auto &x : p.vmr) is >> x;
    return is;
  }

  Point &operator+=(const LazyPoint &x) noexcept;

  /** Fix that must be called after using any arithmetic */
  void expP() noexcept { P = std::exp(P); }

  Pressure<PressureType::Pa> Pres() const noexcept { return P; }

  Temperature<TemperatureType::K> Temp() const noexcept { return T; }

  void Temp(Temperature<TemperatureType::K> x) noexcept { T = x; }

  Magnetism<MagnetismType::T> MagField() const noexcept { return M; }

  Magnetism<MagnetismType::T> &MagField() noexcept { return M; }

  Wind<WindType::meters_per_second> WindField() const noexcept { return W; }

  Wind<WindType::meters_per_second> &WindField() noexcept { return W; }

  const std::vector<VMR<VMRType::ratio>> &VolumeMixingRatios() const {
    return vmr;
  }

  double VolumeMixingRatio(Species::Isotope s) const noexcept {
    if (auto v = std::find_if(vmr.cbegin(), vmr.cend(),
                              [s](auto &v) { return v.isot() == s; });
        v == vmr.cend())
      return 0;
    else
      return v->value();
  }

  void VolumeMixingRatio(Species::Isotope s, double vmrval) noexcept {
    if (auto v = std::find_if(vmr.begin(), vmr.end(),
                              [s](auto &v) { return v.isot() == s; });
        v not_eq vmr.end())
      v->value() = vmrval;
  }

  std::pair<double, double> NonLTERatios(
      std::pair<std::size_t, std::size_t> ids) const noexcept {
    return {nlte[ids.first], nlte[ids.second]};
  }

  NLTE<NLTEType::ratio> NonLTERatio(std::size_t id) const noexcept {
    return nlte[id];
  }

  NLTE<NLTEType::ratio> &NonLTERatio(std::size_t id) noexcept {
    return nlte[id];
  }

  double NumberDensity() const noexcept {
    using Constant::k;
    return P / (k * T);
  }

  double NumberDensityPressureDerivative() const noexcept {
    using Constant::k;
    return 1 / (k * T);
  }

  double NumberDensityTemperatureDerivative() const noexcept {
    using Constant::k;
    using Constant::pow2;
    return -P / (k * pow2(T));
  }

  double FaceWindSpeed(double za, double aa) const noexcept {
    using Conversion::cosd;
    using Conversion::sind;
    const double z = 180 - za;
    const double a = aa <= 0 ? aa + 180 : aa - 180;
    return W.v() * cosd(a) * sind(z) + W.u() * sind(a) * sind(z) +
           W.w() * cosd(z);
  }

  double FaceWindSpeedDerivativeU(double za, double aa) const noexcept {
    using Conversion::cosd;
    using Conversion::sind;
    const double z = 180 - za;
    const double a = aa <= 0 ? aa + 180 : aa - 180;
    return sind(a) * sind(z);
  }

  double FaceWindSpeedDerivativeV(double za, double aa) const noexcept {
    using Conversion::cosd;
    using Conversion::sind;
    const double z = 180 - za;
    const double a = aa <= 0 ? aa + 180 : aa - 180;
    return cosd(a) * sind(z);
  }

  double FaceWindSpeedDerivativeW(double za, double) const noexcept {
    using Conversion::cosd;
    const double z = 180 - za;
    return cosd(z);
  }

  double DopplerShiftRatio(double za, double aa) const noexcept {
    return 1.0 - FaceWindSpeed(za, aa) / Constant::c;
  }

  double DopplerShiftRatioDerivativeU(double za, double aa) const noexcept {
    return -FaceWindSpeedDerivativeU(za, aa) / Constant::c;
  }

  double DopplerShiftRatioDerivativeV(double za, double aa) const noexcept {
    return -FaceWindSpeedDerivativeV(za, aa) / Constant::c;
  }

  double DopplerShiftRatioDerivativeW(double za, double aa) const noexcept {
    return -FaceWindSpeedDerivativeW(za, aa) / Constant::c;
  }

  friend Point operator*(double x, const Point &ap) noexcept {
    Point out{ap};

    out.P = x * std::log(out.P);
    out.T *= x;
    out.M *= x;
    out.W *= x;
    for (auto &v : out.vmr) v *= x;
    for (auto &n : out.nlte) n *= x;
    return out;
  }

  friend void savePoint(
      File::File<File::Operation::Write, File::Type::Xml> &file,
      const Point &ap) {
    file.new_child("Point");
    file.add_attribute("size", ap.size());
    file.add_attribute("Species", ap.specs());
    file.add_attribute("NLTE", ap.nlte.size());
    file << '\n';
    ap.savePureAscii(file);
    file << '\n';
    file.leave_child();
  }

  friend void savePoint(
      File::File<File::Operation::WriteBinary, File::Type::Xml> &file,
      const Point &ap) {
    file.new_child("Point");
    file.add_attribute("size", ap.size());
    file.add_attribute("Species", ap.specs());
    file.add_attribute("NLTE", ap.nlte.size());
    ap.saveBinary(file);
    file.leave_child();
  }

  friend void readPoint(
      File::File<File::Operation::Read, File::Type::Xml> &file, Point &ap) {
    auto child = file.get_child("Point");
    ap.vmr.resize(file.size());
    auto isot = file.get_vector_attribute<Species::Isotope>("Species");
    ap.nlte.resize(file.get_attribute("Species").as_int());
    for (size_t i = 0; i < isot.size(); i++) {
      ap.vmr[i].isot(isot[i]);
    }
    std::istringstream data(child.text().as_string());
    ap.readPureAscii(data);
    file.leave_child();
  }

  friend void readPoint(
      File::File<File::Operation::ReadBinary, File::Type::Xml> &file,
      Point &ap) {
    file.get_child("Point");
    ap.vmr.resize(file.size());
    auto isot = file.get_vector_attribute<Species::Isotope>("Species");
    ap.nlte.resize(file.get_attribute("Species").as_int());
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
  void savePureAscii(Output &file) const {
    file << P << ' ' << T << ' ' << M << ' ' << W;
    for (auto x : vmr) file << ' ' << x.value();
    for (auto n : nlte) file << ' ' << n.value();
  }

  template <typename Output>
  void saveBinary(Output &file) const {
    file.write(P);
    file.write(T);
    file.write(M);
    file.write(W);
    for (auto x : vmr) file.write(x.value());
    file.write(nlte);
  }

  template <typename Input>
  void readPureAscii(Input &file) {
    file >> P >> T >> M >> W;
    for (auto &x : vmr) file >> x.value();
    for (auto &n : nlte) file >> n;
  }

  template <typename Input>
  void readBinary(Input &file) {
    file.read(P);
    file.read(T);
    file.read(M);
    file.read(W);
    for (auto &x : vmr) file.read(x.value());
    file.read(nlte);
  }
};  // Point

class LazyPoint {
  const Point &ap;
  double scale;

 public:
  LazyPoint(const Point &a, double x) noexcept : ap(a), scale(x) {}
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
  }
};  // LazyPoint

class InterPoints {
  LinearInterpPoint tid;
  LinearInterpPoint alt;
  LinearInterpPoint lat;
  LinearInterpPoint lon;

 public:
  constexpr InterPoints() noexcept
      : tid(1, 0), alt(1, 0), lat(1, 0), lon(1, 0) {}

  constexpr InterPoints(LinearInterpPoint time, LinearInterpPoint altitude,
                        LinearInterpPoint latitude,
                        LinearInterpPoint longitude) noexcept
      : tid(time), alt(altitude), lat(latitude), lon(longitude) {}

  constexpr InterPoints(const InterPoints &ip) noexcept
      : tid(ip.tid), alt(ip.alt), lat(ip.lat), lon(ip.lon) {}

  constexpr InterPoints &operator=(const InterPoints &ip) noexcept {
    tid = ip.tid;
    alt = ip.alt;
    lat = ip.lat;
    lon = ip.lon;
    return *this;
  }

  class Uses {
    double w;
    size_t itid;
    size_t ialt;
    size_t ilat;
    size_t ilon;

   public:
    constexpr Uses(double W = 1, size_t ti = 0, size_t al = 0, size_t la = 0,
                   size_t lo = 0) noexcept
        : w(W), itid(ti), ialt(al), ilat(la), ilon(lo) {}
    friend std::ostream &operator<<(std::ostream &os, const Uses &u) {
      return os << u.w << ' ' << u.itid << ' ' << u.ialt << ' ' << u.ilat << ' '
                << u.ilon;
    }
    constexpr Uses(Uses &&u) noexcept
        : w(u.w), itid(u.itid), ialt(u.ialt), ilat(u.ilat), ilon(u.ilon) {}
    constexpr Uses(const Uses &u) noexcept
        : w(u.w), itid(u.itid), ialt(u.ialt), ilat(u.ilat), ilon(u.ilon) {}
    constexpr Uses &operator=(const Uses &u) noexcept {
      w = u.w;
      itid = u.itid;
      ialt = u.ialt;
      ilat = u.ilat;
      ilon = u.ilon;
      return *this;
    }
    constexpr double weight() const noexcept { return w; }
    constexpr size_t tid() const noexcept { return itid; }
    constexpr size_t alt() const noexcept { return ialt; }
    constexpr size_t lat() const noexcept { return ilat; }
    constexpr size_t lon() const noexcept { return ilon; }
  };

  class Output {
    std::array<InterPoints::Uses, 16> data;

   public:
    constexpr InterPoints::Uses operator[](const size_t i) const noexcept {
      return data[i];
    }
    static constexpr size_t size() noexcept { return 16; }
    friend std::ostream &operator<<(std::ostream &os, const Output &o) {
      os << o.data[0];
      for (size_t i = 1; i < o.size(); i++) os << '\n' << o.data[i];
      return os;
    }
    constexpr Output(decltype(data) d) noexcept {
      for (size_t i = 0; i < size(); i++) data[i] = d[i];
    }
    constexpr Output() noexcept {
      for (auto &x : data) x = InterPoints::Uses();
    }
  };

  constexpr Output Weights() const noexcept {
    return Output(std::array<Uses, Output::size()>(
        {Uses((0 + tid.weight()) * (0 + alt.weight()) * (0 + lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 0, alt.index() + 0, lat.index() + 0,
              lon.index() + 0),
         Uses((0 + tid.weight()) * (0 + alt.weight()) * (0 + lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 0, alt.index() + 0, lat.index() + 0,
              lon.index() + 1),
         Uses((0 + tid.weight()) * (0 + alt.weight()) * (1 - lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 0, alt.index() + 0, lat.index() + 1,
              lon.index() + 0),
         Uses((0 + tid.weight()) * (0 + alt.weight()) * (1 - lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 0, alt.index() + 0, lat.index() + 1,
              lon.index() + 1),
         Uses((0 + tid.weight()) * (1 - alt.weight()) * (0 + lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 0, alt.index() + 1, lat.index() + 0,
              lon.index() + 0),
         Uses((0 + tid.weight()) * (1 - alt.weight()) * (0 + lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 0, alt.index() + 1, lat.index() + 0,
              lon.index() + 1),
         Uses((0 + tid.weight()) * (1 - alt.weight()) * (1 - lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 0, alt.index() + 1, lat.index() + 1,
              lon.index() + 0),
         Uses((0 + tid.weight()) * (1 - alt.weight()) * (1 - lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 0, alt.index() + 1, lat.index() + 1,
              lon.index() + 1),
         Uses((1 - tid.weight()) * (0 + alt.weight()) * (0 + lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 1, alt.index() + 0, lat.index() + 0,
              lon.index() + 0),
         Uses((1 - tid.weight()) * (0 + alt.weight()) * (0 + lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 1, alt.index() + 0, lat.index() + 0,
              lon.index() + 1),
         Uses((1 - tid.weight()) * (0 + alt.weight()) * (1 - lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 1, alt.index() + 0, lat.index() + 1,
              lon.index() + 0),
         Uses((1 - tid.weight()) * (0 + alt.weight()) * (1 - lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 1, alt.index() + 0, lat.index() + 1,
              lon.index() + 1),
         Uses((1 - tid.weight()) * (1 - alt.weight()) * (0 + lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 1, alt.index() + 1, lat.index() + 0,
              lon.index() + 0),
         Uses((1 - tid.weight()) * (1 - alt.weight()) * (0 + lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 1, alt.index() + 1, lat.index() + 0,
              lon.index() + 1),
         Uses((1 - tid.weight()) * (1 - alt.weight()) * (1 - lat.weight()) *
                  (0 + lon.weight()),
              tid.index() + 1, alt.index() + 1, lat.index() + 1,
              lon.index() + 0),
         Uses((1 - tid.weight()) * (1 - alt.weight()) * (1 - lat.weight()) *
                  (1 - lon.weight()),
              tid.index() + 1, alt.index() + 1, lat.index() + 1,
              lon.index() + 1)}));
  }

  friend std::ostream &operator<<(std::ostream &os, const InterPoints &ip) {
    return os << ip.tid << ' ' << ip.alt << ' ' << ip.lat << ' ' << ip.lon;
  }
};

class Atm {
  std::vector<Time> tid;
  std::vector<Altitude<AltitudeType::meter>> alt;
  std::vector<Coordinate<CoordinateType::lat>> lat;
  std::vector<Coordinate<CoordinateType::lon>> lon;
  Grid<Point, 4> data;

  bool ok() const {
    auto specs = operator()(0, 0, 0, 0).specs();
    for (auto &x : data) {
      if (specs not_eq x.specs()) return false;
    }
    return true;
  }

 public:
  Atm(const std::vector<Time> &t,
      const std::vector<Altitude<AltitudeType::meter>> &a,
      const std::vector<Coordinate<CoordinateType::lat>> &la,
      const std::vector<Coordinate<CoordinateType::lon>> &lo,
      const Grid<Point, 4> &d)
      : tid(t), alt(a), lat(la), lon(lo), data(d) {
    if (not ok()) throw std::runtime_error("Bad atmosphere");
  }

  Atm(std::size_t t = 0, std::size_t a = 0, std::size_t la = 0,
      std::size_t lo = 0, std::size_t s = 0)
      : tid(t), alt(a), lat(la), lon(lo), data(s, t, a, la, lo) {}

  size_t ntid() const noexcept { return tid.size(); }
  size_t nalt() const noexcept { return alt.size(); }
  size_t nlat() const noexcept { return lat.size(); }
  size_t nlon() const noexcept { return lon.size(); }
  
  const std::vector<Time> & tidvec() const noexcept { return tid; }
  const std::vector<Altitude<AltitudeType::meter>> & altvec() const noexcept { return alt; }
  const std::vector<Coordinate<CoordinateType::lat>> & latvec() const noexcept { return lat; }
  const std::vector<Coordinate<CoordinateType::lon>> & lonvec() const noexcept { return lon; }

  friend std::ostream &operator<<(std::ostream &os, const Atm &a) {
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

  friend std::istream &operator>>(std::istream &is, Atm &a) {
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

  Point &operator()(size_t i, size_t j, size_t k, size_t m) noexcept {
    return data(i, j, k, m);
  }

  const Point &operator()(size_t i, size_t j, size_t k,
                          size_t m) const noexcept {
    return data(i, j, k, m);
  }

  Point operator()(const InterPoints &aip) const noexcept;

  InterPoints interpPoints(
      const Time newtid, const Altitude<AltitudeType::meter> newalt,
      const Coordinate<CoordinateType::lat> newlat,
      const Coordinate<CoordinateType::lon> newlon) const noexcept;

  Point operator()(
      const Time newtid, const Altitude<AltitudeType::meter> newalt,
      const Coordinate<CoordinateType::lat> newlat,
      const Coordinate<CoordinateType::lon> newlon) const noexcept {
    return operator()(interpPoints(newtid, newalt, newlat, newlon));
  }

  template <typename Pos>
  std::pair<InterPoints, Point> operator()(Pos pos) const {
    auto ip = interpPoints(pos.t(), pos.h(), pos.lat(), pos.lon());
    return {ip, operator()(ip)};
  }

  friend void saveAtm(File::File<File::Operation::Write, File::Type::Xml> &file,
                      const Atm &a) {
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
      File::File<File::Operation::WriteBinary, File::Type::Xml> &file,
      const Atm &a) {
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

  friend void readAtm(File::File<File::Operation::Read, File::Type::Xml> &file,
                      Atm &a) {
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

    a.data.resize(a.tid.size(), a.alt.size(), a.lat.size(), a.lon.size());
    auto data = std::istringstream(file.get_child("Data").text().as_string());
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            a(i, j, k, m) = specs;
            a(i, j, k, m).readPureAscii(data);
          }
        }
      }
    }
    file.leave_child();

    file.leave_child();
  }

  friend void readAtm(
      File::File<File::Operation::ReadBinary, File::Type::Xml> &file, Atm &a) {
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

    a.data.resize(a.tid.size(), a.alt.size(), a.lat.size(), a.lon.size());
    for (decltype(a.tid.size()) i = 0; i < a.tid.size(); i++) {
      for (decltype(a.alt.size()) j = 0; j < a.alt.size(); j++) {
        for (decltype(a.lat.size()) k = 0; k < a.lat.size(); k++) {
          for (decltype(a.lon.size()) m = 0; m < a.lon.size(); m++) {
            a(i, j, k, m) = specs;
            a(i, j, k, m).readBinary(file);
          }
        }
      }
    }
    file.leave_child();

    file.leave_child();
  }

  Altitude<AltitudeType::meter> max_alt() const noexcept { return max(alt); }
};  // Atm
}  // namespace Atmosphere

#endif  // atm_h
