#ifndef forward_h
#define forward_h

#include "grids.h"
#include "jacobian.h"
#include "rtehelpers.h"
#include "xsec.h"

namespace RTE::Forward {
template <size_t N>
struct Results {
  Grid<RadVec<N>, 2> x;   // size (np, nf)
  Grid<TraMat<N>, 2> T;   // size (np, nf)
  Grid<RadVec<N>, 3> dx;  // size (nt, np, nf)

  Results(const std::vector<RadVec<N>> &rad0,
          const std::size_t np, const std::size_t nf, const std::size_t nt) noexcept
      : x({}, np, nf),
        T({}, np, nf),
        dx({}, nt, np, nf) {
    if (nf not_eq rad0.size() or np == 0) {
      std::cerr << "Bad sizes\n";
      std::terminate();
    }

    for (size_t i = 0; i < nf; i++) {
      x(np - 1, i) = rad0[i];
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
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path
                  );

Results<2> compute(const std::vector<RadVec<2>> &rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path
                  );

Results<3> compute(const std::vector<RadVec<3>> &rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path
                  );

Results<4> compute(const std::vector<RadVec<4>> &rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path
                  );
}  // namespace RTE::Forward

#endif  // forward_h
