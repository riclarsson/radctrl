#include "atm.h"

#include <array>
#include <numeric>
#include <type_traits>

namespace Interp {
template <size_t N>
constexpr double sum(const std::array<double, N> &vals) {
  return std::accumulate(vals.cbegin(), vals.cend(), 0);
}

constexpr double weight(double x, double x0, double x1) {
  return x0 == x1 ? 1.0 : (x - x1) / (x0 - x1);
}

template <typename... Xs>
constexpr std::array<double, sizeof...(Xs) - 1> weight(double x, Xs... xs) {
  const std::array<double, sizeof...(Xs)> gx = {xs...};
  std::array<double, sizeof...(Xs) - 1> w;
  w.fill(0);
  for (size_t i = 0; i < sizeof...(Xs) - 1; i++)
    for (size_t j = 0; j < sizeof...(Xs); j++)
      if (i not_eq j) w[i] += (x - gx[j]) / (gx[i] - gx[j]);
  return w;
}
}  // namespace Interp

namespace Atmosphere {
Point &Point::operator+=(const LazyPoint &x) noexcept {
  P += x.P();
  T += x.T();
  M += x.M();
  W += x.W();
  for (size_t i = 0; i < vmr.size(); i++) vmr[i] += x.vmr(i);
  return *this;
}

AtmInterPoints forwardmap(
  const std::vector<Time> & tid,
  const Time newtid,
  const std::vector<Altitude<AltitudeType::meter>> & alt,
  const Altitude<AltitudeType::meter> newalt,
  const std::vector<Coordinate<CoordinateType::lat>> & lat,
  const Coordinate<CoordinateType::lat> newlat,
  const std::vector<Coordinate<CoordinateType::lon>> & lon,
  const Coordinate<CoordinateType::lon> newlon) noexcept {
    auto tidpos = std::find_if(tid.cbegin(), tid.cend(), [newtid](auto a) {
      return not(a < newtid) and not(a == newtid);
    });
    auto tidlow = (tidpos == tid.cbegin()) ? tid.cbegin() : tidpos - 1;
    auto tidw = (tidpos == tid.cend())
    ? 1.0
    : Interp::weight(newtid.Seconds(), tidlow->Seconds(),
                     tidpos->Seconds());
    
    auto altpos = std::find_if(alt.cbegin(), alt.cend(),
                               [newalt](auto a) { return newalt <= a; });
    auto altlow = (altpos == alt.cbegin()) ? alt.cbegin() : altpos - 1;
    auto altw = (altpos == alt.cend())
    ? 1.0
    : Interp::weight(newalt, altlow->value(), altpos->value());
    
    auto latpos = std::find_if(lat.cbegin(), lat.cend(),
                               [newlat](auto a) { return newlat <= a; });
    auto latlow = (latpos == lat.cbegin()) ? lat.cbegin() : latpos - 1;
    auto latw = (latpos == lat.cend())
    ? 1.0
    : Interp::weight(newlat, latlow->value(), latpos->value());
    
    auto lonpos = std::find_if(lon.cbegin(), lon.cend(),
                               [newlon](auto a) { return newlon <= a; });
    auto lonlow = (lonpos == lon.cbegin()) ? lon.cbegin() : lonpos - 1;
    auto lonw = (lonpos == lon.cend())
    ? 1.0
    : Interp::weight(newlon, lonlow->value(), lonpos->value());
    
    return {
      {tidw, size_t(tidlow - tid.cbegin())},
      {altw, size_t(altlow - alt.cbegin())},
      {latw, size_t(latlow - lat.cbegin())},
      {lonw, size_t(lonlow - lon.cbegin())}};
}

Point Atm::operator()(Time newtid, Altitude<AltitudeType::meter> newalt,
                      Coordinate<CoordinateType::lat> newlat,
                      Coordinate<CoordinateType::lon> newlon) const {
  const auto aip = forwardmap(tid, newtid, alt, newalt, lat, newlat, lon, newlon);

  Point out{(aip.tid.w * aip.alt.w * aip.lat.w * aip.lon.w) *
            data(aip.tid.i, aip.alt.i,
                 aip.lat.i, aip.lon.i)};  // 0000

  if (aip.lon.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i,
                 aip.lat.i, aip.lon.i + 1),
            (aip.tid.w) * (aip.alt.w) * (aip.lat.w) * (1 - aip.lon.w)};  // 0001
  }
  if (aip.lat.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i,
                 aip.lat.i + 1, aip.lon.i),
            (aip.tid.w) * (aip.alt.w) * (1 - aip.lat.w) * (aip.lon.w)};  // 0010
  }
  if (aip.lat.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i,
                 aip.lat.i + 1, aip.lon.i + 1),
            (aip.tid.w) * (aip.alt.w) * (1 - aip.lat.w) * (1 - aip.lon.w)};  // 0011
  }
  if (aip.alt.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i + 1,
                 aip.lat.i, aip.lon.i),
            (aip.tid.w) * (1 - aip.alt.w) * (aip.lat.w) * (aip.lon.w)};  // 0100
  }
  if (aip.alt.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i + 1,
                 aip.lat.i, aip.lon.i + 1),
            (aip.tid.w) * (1 - aip.alt.w) * (aip.lat.w) * (1 - aip.lon.w)};  // 0101
  }
  if (aip.alt.w not_eq 1 and aip.lat.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i + 1,
                 aip.lat.i + 1, aip.lon.i),
            (aip.tid.w) * (1 - aip.alt.w) * (1 - aip.lat.w) * (aip.lon.w)};  // 0110
  }
  if (aip.alt.w not_eq 1 and aip.lon.w not_eq 1 and aip.lat.w not_eq 1) {
    out += {data(aip.tid.i, aip.alt.i + 1,
                 aip.lat.i + 1, aip.lon.i + 1),
            (aip.tid.w) * (1 - aip.alt.w) * (1 - aip.lat.w) * (1 - aip.lon.w)};  // 0111
  }
  if (aip.tid.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i,
                 aip.lat.i, aip.lon.i),
            (1 - aip.tid.w) * (aip.alt.w) * (aip.lat.w) * (aip.lon.w)};  // 1000
  }
  if (aip.tid.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i,
                 aip.lat.i, aip.lon.i + 1),
            (1 - aip.tid.w) * (aip.alt.w) * (aip.lat.w) * (1 - aip.lon.w)};  // 1001
  }
  if (aip.tid.w not_eq 1 and aip.lat.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i,
                 aip.lat.i + 1, aip.lon.i),
            (1 - aip.tid.w) * (aip.alt.w) * (1 - aip.lat.w) * (aip.lon.w)};  // 1010
  }
  if (aip.tid.w not_eq 1 and aip.lat.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i,
                 aip.lat.i + 1, aip.lon.i + 1),
            (1 - aip.tid.w) * (aip.alt.w) * (1 - aip.lat.w) * (1 - aip.lon.w)};  // 1011
  }
  if (aip.tid.w not_eq 1 and aip.alt.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i + 1,
                 aip.lat.i, aip.lon.i),
            (1 - aip.tid.w) * (1 - aip.alt.w) * (aip.lat.w) * (aip.lon.w)};  // 1100
  }
  if (aip.tid.w not_eq 1 and aip.alt.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i + 1,
                 aip.lat.i, aip.lon.i + 1),
            (1 - aip.tid.w) * (1 - aip.alt.w) * (aip.lat.w) * (1 - aip.lon.w)};  // 1101
  }
  if (aip.tid.w not_eq 1 and aip.alt.w not_eq 1 and aip.lat.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i + 1,
                 aip.lat.i + 1, aip.lon.i),
            (1 - aip.tid.w) * (1 - aip.alt.w) * (1 - aip.lat.w) * (aip.lon.w)};  // 1110
  }
  if (aip.tid.w not_eq 1 and aip.alt.w not_eq 1 and aip.lat.w not_eq 1 and aip.lon.w not_eq 1) {
    out += {data(aip.tid.i + 1, aip.alt.i + 1,
                 aip.lat.i + 1, aip.lon.i + 1),
            (1 - aip.tid.w) * (1 - aip.alt.w) * (1 - aip.lat.w) * (1 - aip.lon.w)};  // 1111
  }

  // Note that this is necessary to fix the arithmetic of adding pressures
  out.expP();

  return out;
}
}  // namespace Atmosphere
