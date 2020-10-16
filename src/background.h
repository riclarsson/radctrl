#ifndef background_h
#define background_h

#include "atmpath.h"
#include "enums.h"
#include "grids.h"
#include "mathhelpers.h"
#include "rtehelpers.h"
#include "timeclass.h"

namespace Background {

class InterPoints {
  LinearInterpPoint tid;
  LinearInterpPoint lat;
  LinearInterpPoint lon;

 public:
  constexpr InterPoints() noexcept : tid(1, 0), lat(1, 0), lon(1, 0) {}

  constexpr InterPoints(LinearInterpPoint time, LinearInterpPoint latitude,
                        LinearInterpPoint longitude) noexcept
      : tid(time), lat(latitude), lon(longitude) {}

  constexpr InterPoints(const InterPoints &ip) noexcept
      : tid(ip.tid), lat(ip.lat), lon(ip.lon) {}

  constexpr InterPoints &operator=(const InterPoints &ip) noexcept {
    tid = ip.tid;
    lat = ip.lat;
    lon = ip.lon;
    return *this;
  }

  class Uses {
    double w;
    size_t itid;
    size_t ilat;
    size_t ilon;

   public:
    constexpr Uses(double W = 1, size_t ti = 0, size_t la = 0,
                   size_t lo = 0) noexcept
        : w(W), itid(ti), ilat(la), ilon(lo) {}

    friend std::ostream &operator<<(std::ostream &os, const Uses &u) {
      return os << u.w << ' ' << u.itid << ' ' << u.ilat << ' ' << u.ilon;
    }

    constexpr Uses(Uses &&u) noexcept
        : w(u.w), itid(u.itid), ilat(u.ilat), ilon(u.ilon) {}

    constexpr Uses(const Uses &u) noexcept
        : w(u.w), itid(u.itid), ilat(u.ilat), ilon(u.ilon) {}

    constexpr Uses &operator=(const Uses &u) noexcept {
      w = u.w;
      itid = u.itid;
      ilat = u.ilat;
      ilon = u.ilon;
      return *this;
    }

    constexpr double weight() const noexcept { return w; }
    constexpr size_t tid() const noexcept { return itid; }
    constexpr size_t lat() const noexcept { return ilat; }
    constexpr size_t lon() const noexcept { return ilon; }
  };

  class Output {
    std::array<InterPoints::Uses, 8> data;

   public:
    constexpr InterPoints::Uses operator[](const size_t i) const noexcept {
      return data[i];
    }

    static constexpr size_t size() noexcept { return 8; }

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
        {Uses((0 + tid.weight()) * (0 + lat.weight()) * (0 + lon.weight()),
              tid.index() + 0, lat.index() + 0, lon.index() + 0),
         Uses((0 + tid.weight()) * (0 + lat.weight()) * (1 - lon.weight()),
              tid.index() + 0, lat.index() + 0, lon.index() + 1),
         Uses((0 + tid.weight()) * (1 - lat.weight()) * (0 + lon.weight()),
              tid.index() + 0, lat.index() + 1, lon.index() + 0),
         Uses((0 + tid.weight()) * (1 - lat.weight()) * (1 - lon.weight()),
              tid.index() + 0, lat.index() + 1, lon.index() + 1),
         Uses((1 - tid.weight()) * (0 + lat.weight()) * (0 + lon.weight()),
              tid.index() + 1, lat.index() + 0, lon.index() + 0),
         Uses((1 - tid.weight()) * (0 + lat.weight()) * (1 - lon.weight()),
              tid.index() + 1, lat.index() + 0, lon.index() + 1),
         Uses((1 - tid.weight()) * (1 - lat.weight()) * (0 + lon.weight()),
              tid.index() + 1, lat.index() + 1, lon.index() + 0),
         Uses((1 - tid.weight()) * (1 - lat.weight()) * (1 - lon.weight()),
              tid.index() + 1, lat.index() + 1, lon.index() + 1)}));
  }

  friend std::ostream &operator<<(std::ostream &os, const InterPoints &ip) {
    return os << ip.tid << ' ' << ip.lat << ' ' << ip.lon;
  }
};

class Surface {
  std::vector<Time> tid;
  std::vector<Coordinate<CoordinateType::lat>> lat;
  std::vector<Coordinate<CoordinateType::lon>> lon;
  Grid<Temperature<TemperatureType::K>, 3> temps;  // tid, lat, lon

 public:
  Surface(const Atmosphere::Atm &atm) noexcept
      : tid(atm.tidvec()),
        lat(atm.latvec()),
        lon(atm.lonvec()),
        temps(0, atm.ntid(), atm.nlat(), atm.nlon()) {
    for (std::size_t itid = 0; itid < atm.ntid(); itid++)
      for (std::size_t ilat = 0; ilat < atm.nlat(); ilat++)
        for (std::size_t ilon = 0; ilon < atm.nlon(); ilon++)
          temps(itid, ilat, ilon) = atm(itid, 0, ilat, ilon).Temp();
  }

  InterPoints interpPoints(
      const Time newtid, const Coordinate<CoordinateType::lat> newlat,
      const Coordinate<CoordinateType::lon> newlon) const noexcept;

  template <std::size_t N>
  std::pair<InterPoints, std::vector<RTE::RadVec<N>>> compute(
      const Time newtid, const Coordinate<CoordinateType::lat> newlat,
      const Coordinate<CoordinateType::lon> newlon,
      const std::vector<Frequency<FrequencyType::Freq>> &f) const noexcept {
    const InterPoints ip = interpPoints(newtid, newlat, newlon);
    const auto weights = ip.Weights();
    std::vector<RTE::RadVec<N>> out = RTE::source_vec_planck<N>(
        temps(weights[0].tid(), weights[0].lat(), weights[0].lon()), f);
    for (auto &x : out) x = weights[0].weight() * x;
    for (std::size_t i = 1; i < InterPoints::Output::size(); i++) {
      if (const double w = weights[i].weight(); w not_eq 0) {
        const std::vector<RTE::RadVec<N>> tmp = RTE::source_vec_planck<N>(
            temps(weights[i].tid(), weights[i].lat(), weights[i].lon()), f);
        std::transform(tmp.cbegin(), tmp.cend(), out.begin(), out.begin(),
                       [w](auto &t, auto &o) { return t * w + o; });
      }
    }
    return {ip, out};
  }
};

struct Space {
  static constexpr Temperature<TemperatureType::K> temp =
      Constant::cosmic_microwave_background_temperature;
};

class Background {
  Surface surface;

 public:
  Background(const Surface &surf) noexcept : surface(surf) {}

  template <std::size_t N>
  std::pair<InterPoints, std::vector<RTE::RadVec<N>>> compute(
      Path::BackgroundType type,
      const Geom::Pos<Geom::PosType::Ellipsoidal> &pos,
      const std::vector<Frequency<FrequencyType::Freq>> &f) const noexcept {
    switch (type) {
      case Path::BackgroundType::Surface:
        return surface.compute<N>(pos.t(), pos.lat(), pos.lon(), f);
      case Path::BackgroundType::Space:
        return {InterPoints(), RTE::source_vec_planck<N>(Space::temp, f)};
      case Path::BackgroundType::FINAL: { /*leave last*/
      }
    }
    return {};
  }
};
}  // namespace Background

#endif  // background_h
