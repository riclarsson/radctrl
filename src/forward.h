#ifndef forward_h
#define forward_h

#include "grids.h"
#include "jacobian.h"
#include "rtehelpers.h"
#include "xsec.h"

namespace RTE::Forward {
struct Calculations {
  std::vector<Frequency<FrequencyType::Freq>> f_grid;  // size: nf
  std::vector<Derivative::Target> targets;             // size: nt
  std::vector<Absorption::Band> bands;                 // size: nb
  std::vector<Path::Point> path;                       // size: np
};

template <size_t N>
struct Results {
  Grid<RadVec<N>, 2> x;   // size (np, nf)
  Grid<TraMat<N>, 2> T;   // size (np, nf)
  Grid<RadVec<N>, 3> dx;  // size (nt, np, nf)

  Results(const std::vector<RadVec<N>> &rad0,
          const Calculations &calcs) noexcept
      : x({}, calcs.path.size(), calcs.f_grid.size()),
        T({}, calcs.path.size(), calcs.f_grid.size()),
        dx({}, calcs.targets.size(), calcs.path.size(), calcs.f_grid.size()) {
    if (calcs.f_grid.size() not_eq rad0.size() or calcs.path.size() == 0) {
      std::cerr << "Bad sizes\n";
      std::terminate();
    }

    for (size_t i = 0; i < calcs.f_grid.size(); i++) {
      x(calcs.path.size() - 1, i) = rad0[i];
    }
  }

  void convert_to_planck(const std::vector<Frequency<FrequencyType::Freq>> &f) {
    RTE::to_planck(x, dx, f);
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
}  // namespace RTE::Forward

#endif  // forward_h
