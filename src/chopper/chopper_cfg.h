#ifndef chopper_cfg_h
#define chopper_cfg_h

#include "../enums.h"

namespace Instrument::Chopper {

/*! List of all possible Chopper named states */
ENUMCLASS(ChopperPos, int, Cold, Hot, Antenna, Reference)

}

#endif  // chopper_cfg_h
