#ifndef arts_interface_h
#define arts_interface_h

#include "autoarts.h"

namespace ARTS {
Workspace init(std::size_t screen = 0, std::size_t file = 0,
               std::size_t agenda = 0);

double test();
}  // namespace ARTS

#endif  // arts_interface_h
