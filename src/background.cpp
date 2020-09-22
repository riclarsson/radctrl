#include "background.h"

namespace Background {
InterPoints Surface::interpPoints(
    const Time newtid, const Coordinate<CoordinateType::lat> newlat,
    const Coordinate<CoordinateType::lon> newlon) const noexcept {
  LinearInterpPoint tidout(1, 0), latout(1, 0), lonout(1, 0);

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

  return {tidout, latout, lonout};
}
}  // namespace Background
