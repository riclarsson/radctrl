#include "interface.h"

#include <physics_funcs.h>
#include <absorption.h>
#include <rte.h>

namespace ARTS {
  double test() {
    return invplanck(planck(1e10, 150.1), 1e10);
  }
}
