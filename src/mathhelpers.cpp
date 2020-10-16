#include "mathhelpers.h"

#include <cmath>

double BesselJ1(double x) noexcept { return std::cyl_bessel_j(1.0, x); }
