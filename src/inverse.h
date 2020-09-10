#ifndef inverse_h
#define inverse_h

#include "atm.h"
#include "forward.h"

namespace RTE::Inverse {
ENUMCLASS(BeamType, char, PencilBeam)
ENUMCLASS(Stokes, char, Full, None)

struct ForwardConvolution {
  std::size_t ntid;
  std::size_t nalt;
  std::size_t nlat;
  std::size_t nlon;
  std::size_t nstokes;
  Eigen::VectorXd rad;
  Eigen::MatrixXd jac;

  ForwardConvolution(std::size_t nf, std::size_t stokes,
                     const Atmosphere::Atm& atm,
                     const std::vector<Derivative::Target>& derivs) noexcept
      : ntid(atm.ntid()),
        nalt(atm.nalt()),
        nlat(atm.nlat()),
        nlon(atm.nlon()),
        nstokes(stokes),
        rad(Eigen::VectorXd::Zero(stokes * nf)) {
    std::size_t nt = 0;
    for (auto& deriv : derivs) {
      if (deriv == Derivative::Type::Atm) {
        nt += natmdata();
      } else {
        nt += 1;
      }
    }

    jac = Eigen::MatrixXd::Zero(stokes * nf, nt);
  }

  std::size_t natmdata() const noexcept { return ntid * nalt * nlat * nlon; }
  std::size_t iatmdata(std::size_t itid, std::size_t ialt, std::size_t ilat,
                       std::size_t ilon) const noexcept {
    [[unlikely]] if (ilon > nlon or ilat > nlat or ialt > nlat or itid > ntid) {
      std::cerr << "Out of range\n";
      std::terminate();
    }
    return ilon + ilat * nlon + ialt * nlat * nlon + itid * nalt * nlat * nlon;
  }
};

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<1>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight = 1);

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<2>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight = 1);

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<3>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight = 1);

void map_to_jacobian(ForwardConvolution& res,
                     const Forward::Results<4>& partres,
                     const std::vector<Derivative::Target>& derivs,
                     const std::vector<Atmosphere::InterPoints>& interp_weights,
                     const double path_weight = 1);

ForwardConvolution compute_forward(
    const Atmosphere::Atm& atm, const Geom::Nav& pos_los,
    const std::vector<Absorption::Band>& bands,
    const std::vector<Derivative::Target>& derivs,
    const std::vector<Frequency<FrequencyType::Freq>>& sensor_f_grid,
    const Stokes stokes_dimension, const BeamType beamtype);
};  // namespace RTE::Inverse

#endif  // inverse_h
