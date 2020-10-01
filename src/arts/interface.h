#ifndef interface_h
#define interface_h

#include <autoarts.h>

#include "arts_agenda.h"

namespace ARTS {
Workspace init(std::size_t screen = 0, std::size_t file = 0,
               std::size_t agenda = 0);

double test();
}  // namespace ARTS

#endif  // interface_h
