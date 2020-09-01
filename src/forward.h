#ifndef forward_h
#define forward_h

#include "grids.h"
#include "jacobian.h"
#include "rtehelpers.h"
#include "xsec.h"

namespace RTE {
namespace Forward {
struct Calculations {
  std::vector<Frequency<FrequencyType::Freq>> f_grid;
  std::vector<Derivative::Target> targets;
  std::vector<Polarization> polarization;
  std::vector<Absorption::Band> bands;
  std::vector<Path::Point> path;

  size_t nrows() const { return polarization.size() * f_grid.size(); }
  size_t ncols() const { return targets.size() * path.size(); }
};

template <size_t N>
struct Results {
  Grid<RadVec<N>, 2> x;
  Derivative::Jacobian dx;

  Results(const std::vector<RadVec<N>> &rad0,
          const Calculations &calcs) noexcept
      : x({}, calcs.path.size(), calcs.f_grid.size()),
        dx(calcs.nrows(), calcs.ncols()) {
    if (calcs.f_grid.size() not_eq rad0.size()) {
      std::cerr << "Bad sizes\n";
      std::terminate();
    }

    for (size_t i = 0; i < calcs.f_grid.size(); i++) {
      x(calcs.path.size() - 1, i) = rad0[i];
    }
  }

  [[nodiscard]] std::vector<RadVec<N>> sensor_results() const {
    std::vector<RadVec<N>> out(x.sizes()[1]);
    for (std::size_t i = 0; i < x.sizes()[1]; i++) {
      out[i] = x(0, i);
    }
    return out;
  }
};

Results<1> compute(const std::vector<RadVec<1>> &rad0,
                   const Calculations &calcs);

Results<2> compute(const std::vector<RadVec<2>> &rad0,
                   const Calculations &calcs);

Results<3> compute(const std::vector<RadVec<3>> &rad0,
                   const Calculations &calcs);

Results<4> compute(const std::vector<RadVec<4>> &rad0,
                   const Calculations &calcs);
}  // namespace Forward
}  // namespace RTE

#endif  // forward_h
