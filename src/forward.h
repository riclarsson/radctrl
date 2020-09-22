#ifndef forward_h
#define forward_h

#include "antenna.h"
#include "background.h"
#include "enums.h"
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

  Results(const std::vector<RadVec<N>>& rad0, const std::size_t np,
          const std::size_t nf, const std::size_t nt) noexcept
      : x({}, np, nf), T({}, np, nf), dx({}, nt, np, nf) {
    if (nf not_eq rad0.size() or np == 0) {
      std::cerr << "Bad sizes\n";
      std::terminate();
    }

    for (size_t i = 0; i < nf; i++) {
      x(np - 1, i) = rad0[i];
    }
  }

  void convert_to_planck(const std::vector<Frequency<FrequencyType::Freq>>& f) {
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

Results<1> compute(const std::vector<RadVec<1>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path);

Results<2> compute(const std::vector<RadVec<2>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path);

Results<3> compute(const std::vector<RadVec<3>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path);

Results<4> compute(const std::vector<RadVec<4>>& rad0,
                   const std::vector<Frequency<FrequencyType::Freq>>& f_grid,
                   const std::vector<Derivative::Target>& targets,
                   const std::vector<Absorption::Band>& bands,
                   const std::vector<Path::Point>& path);

inline std::size_t num_derivs(const std::vector<Derivative::Target>& derivs,
                              std::size_t natm, std::size_t nsurf) noexcept {
  std::size_t n = 0;
  for (auto& deriv : derivs) n += deriv.JacobianCount(natm, nsurf);
  return n;
}

struct Convolution {
  std::size_t ntid;
  std::size_t nalt;
  std::size_t nlat;
  std::size_t nlon;
  std::size_t npolarizations;
  Eigen::VectorXd rad;
  Eigen::MatrixXd jac;

  Convolution(std::size_t nf, std::size_t polarizations,
              const Atmosphere::Atm& atm,
              const std::vector<Derivative::Target>& derivs) noexcept
      : ntid(atm.ntid()),
        nalt(atm.nalt()),
        nlat(atm.nlat()),
        nlon(atm.nlon()),
        npolarizations(polarizations),
        rad(Eigen::VectorXd::Zero(polarizations * nf)),
        jac(Eigen::MatrixXd::Zero(polarizations * nf,
                                  num_derivs(derivs, natmdata(), 0))) {
  }  // FIXME: SURFACE VARIABLE COUNT

  std::size_t natmdata() const noexcept { return ntid * nalt * nlat * nlon; }
  std::size_t iatmdata(std::size_t itid, std::size_t ialt, std::size_t ilat,
                       std::size_t ilon) const noexcept {
    if (ilon > nlon or ilat > nlat or ialt > nalt or itid > ntid) {
      std::cerr << "Out of range\n";
      std::cerr << '\t' << "NumLon:" << ' ' << nlon << ',' << ' '
                << "ILon: " << ilon << '\n';
      std::cerr << '\t' << "NumLat:" << ' ' << nlat << ',' << ' '
                << "ILat: " << ilat << '\n';
      std::cerr << '\t' << "NumAlt:" << ' ' << nalt << ',' << ' '
                << "IAlt: " << ialt << '\n';
      std::cerr << '\t' << "NumTid:" << ' ' << ntid << ',' << ' '
                << "ITid: " << itid << '\n';
      std::terminate();
    }
    return ilon + ilat * nlon + ialt * nlat * nlon + itid * nalt * nlat * nlon;
  }
};

Convolution compute_convolution(
    const Atmosphere::Atm& atm, const Background::Background& background,
    const Geom::Nav& pos_los, const std::vector<Absorption::Band>& bands,
    const std::vector<Derivative::Target>& derivs,
    const Sensor::Properties& sensor_prop,
    const Distance<DistanceType::meter> layer_thickness);

}  // namespace RTE::Forward

#endif  // forward_h
