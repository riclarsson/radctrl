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

InterPoints Atm::interpPoints(
    const Time newtid, const Altitude<AltitudeType::meter> newalt,
    const Coordinate<CoordinateType::lat> newlat,
    const Coordinate<CoordinateType::lon> newlon) const noexcept {
  LinearInterpPoint tidout(1, 0), altout(1, 0), latout(1, 0), lonout(1, 0);

  if (tid.size() > 1) {
    auto tidpos = std::find_if(tid.cbegin(), tid.cend(), [newtid](auto a) {
      return not(a < newtid) and not(a == newtid);
    });
    auto tidlow = (tidpos == tid.cbegin()) ? tid.cbegin() : tidpos - 1;
    auto tidw = (tidpos == tid.cend())
                    ? 1.0
                    : Interp::weight(newtid.Seconds(), tidlow->Seconds(),
                                     tidpos->Seconds());
    tidout = LinearInterpPoint(tidw, size_t(tidlow - tid.cbegin()));
  }

  if (alt.size() > 1) {
    auto altpos = std::find_if(alt.cbegin(), alt.cend(),
                               [newalt](auto a) { return newalt <= a; });
    auto altlow = (altpos == alt.cbegin()) ? alt.cbegin() : altpos - 1;
    auto altw = (altpos == alt.cend())
                    ? 1.0
                    : Interp::weight(newalt, altlow->value(), altpos->value());
    altout = LinearInterpPoint(altw, size_t(altlow - alt.cbegin()));
  }

  if (lat.size() > 1) {
    auto latpos = std::find_if(lat.cbegin(), lat.cend(),
                               [newlat](auto a) { return newlat <= a; });
    auto latlow = (latpos == lat.cbegin()) ? lat.cbegin() : latpos - 1;
    auto latw = (latpos == lat.cend())
                    ? 1.0
                    : Interp::weight(newlat, latlow->value(), latpos->value());
    latout = LinearInterpPoint(latw, size_t(latlow - lat.cbegin()));
  }

  if (lon.size() > 1) {
    auto lonpos = std::find_if(lon.cbegin(), lon.cend(),
                               [newlon](auto a) { return newlon <= a; });
    auto lonlow = (lonpos == lon.cbegin()) ? lon.cbegin() : lonpos - 1;
    auto lonw = (lonpos == lon.cend())
                    ? 1.0
                    : Interp::weight(newlon, lonlow->value(), lonpos->value());
    lonout = LinearInterpPoint(lonw, size_t(lonlow - lon.cbegin()));
  }

  return {tidout, altout, latout, lonout};
}

Point Atm::operator()(const InterPoints &aip) const noexcept {
  const auto map = aip.Weights();

  Point out(map[0].weight() *
            data(map[0].tid(), map[0].alt(), map[0].lat(), map[0].lon()));
  for (size_t i = 1; i < map.size(); i++) {
    if (map[i].weight() not_eq 0) {
      out += LazyPoint(
          data(map[i].tid(), map[i].alt(), map[i].lat(), map[i].lon()),
          map[i].weight());
    }
  }

  // Note that this is necessary to fix the arithmetic of adding pressures
  out.expP();

  return out;
}
}  // namespace Atmosphere
