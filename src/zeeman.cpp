#include "zeeman.h"

namespace Absorption {
  namespace Zeeman {
    Angles angles(double u, double v, double w, double z,
                  double a) noexcept {
                    using Conversion::cosd;
                    using Conversion::sind;
                    
                    // XYZ vectors normalized
                    const vec3 n{cosd(a) * sind(z), sind(a) * sind(z), cosd(z)};
                    const vec3 ev{cosd(a) * cosd(z), sind(a) * cosd(z), -sind(z)};
                    const auto normalized = vec3::norm(u, v, w);
                    const vec3 inplane = normalized.first - normalized.first.dot(n) * n;
                    
                    if (normalized.second == 0)
                      return {0, 0};
                    else
                      return {Conversion::acosd(n.dot(normalized.first)),
                        Conversion::atan2d(ev.dot(inplane), ev.cross(inplane).dot(n))};
                  }
  }
}
