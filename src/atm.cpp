#include "atm.h"

#include <array>
#include <numeric>
#include <type_traits>

namespace Interp {
template <size_t N>
constexpr double sum(const std::array<double, N>& vals) {
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
Point& Point::operator+=(const LazyPoint& x) noexcept {
  P += x.P();
  T += x.T();
  M += x.M();
  W += x.W();
  for (size_t i = 0; i < vmr.size(); i++) vmr[i] += x.vmr(i);
  return *this;
}

Point Atm::operator()(Time newtid, Altitude<AltitudeType::meter> newalt,
                      Coordinate<CoordinateType::lat> newlat,
                      Coordinate<CoordinateType::lon> newlon) const {
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

  Point out{(tidw * altw * latw * lonw) *
            data(tidlow - tid.cbegin(), altlow - alt.cbegin(),
                 latlow - lat.cbegin(), lonlow - lon.cbegin())};  // 0000

  if (lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin(),
                 latlow - lat.cbegin(), lonlow - lon.cbegin() + 1),
            (tidw) * (altw) * (latw) * (1 - lonw)};  // 0001
  }
  if (latw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin(),
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin()),
            (tidw) * (altw) * (1 - latw) * (lonw)};  // 0010
  }
  if (latw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin(),
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin() + 1),
            (tidw) * (altw) * (1 - latw) * (1 - lonw)};  // 0011
  }
  if (altw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin(), lonlow - lon.cbegin()),
            (tidw) * (1 - altw) * (latw) * (lonw)};  // 0100
  }
  if (altw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin(), lonlow - lon.cbegin() + 1),
            (tidw) * (1 - altw) * (latw) * (1 - lonw)};  // 0101
  }
  if (altw not_eq 1 and latw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin()),
            (tidw) * (1 - altw) * (1 - latw) * (lonw)};  // 0110
  }
  if (altw not_eq 1 and lonw not_eq 1 and latw not_eq 1) {
    out += {data(tidlow - tid.cbegin(), altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin() + 1),
            (tidw) * (1 - altw) * (1 - latw) * (1 - lonw)};  // 0111
  }
  if (tidw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin(),
                 latlow - lat.cbegin(), lonlow - lon.cbegin()),
            (1 - tidw) * (altw) * (latw) * (lonw)};  // 1000
  }
  if (tidw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin(),
                 latlow - lat.cbegin(), lonlow - lon.cbegin() + 1),
            (1 - tidw) * (altw) * (latw) * (1 - lonw)};  // 1001
  }
  if (tidw not_eq 1 and latw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin(),
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin()),
            (1 - tidw) * (altw) * (1 - latw) * (lonw)};  // 1010
  }
  if (tidw not_eq 1 and latw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin(),
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin() + 1),
            (1 - tidw) * (altw) * (1 - latw) * (1 - lonw)};  // 1011
  }
  if (tidw not_eq 1 and altw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin(), lonlow - lon.cbegin()),
            (1 - tidw) * (1 - altw) * (latw) * (lonw)};  // 1100
  }
  if (tidw not_eq 1 and altw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin(), lonlow - lon.cbegin() + 1),
            (1 - tidw) * (1 - altw) * (latw) * (1 - lonw)};  // 1101
  }
  if (tidw not_eq 1 and altw not_eq 1 and latw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin()),
            (1 - tidw) * (1 - altw) * (1 - latw) * (lonw)};  // 1110
  }
  if (tidw not_eq 1 and altw not_eq 1 and latw not_eq 1 and lonw not_eq 1) {
    out += {data(tidlow - tid.cbegin() + 1, altlow - alt.cbegin() + 1,
                 latlow - lat.cbegin() + 1, lonlow - lon.cbegin() + 1),
            (1 - tidw) * (1 - altw) * (1 - latw) * (1 - lonw)};  // 1111
  }

  // Note that this is necessary to fix the arithmetic of adding pressures
  out.expP();

  return out;
}
}  // namespace Atmosphere
