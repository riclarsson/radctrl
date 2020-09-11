#ifndef inverse_h
#define inverse_h

#include "atm.h"

namespace RTE::Inverse {
ENUMCLASS(BeamType, unsigned char, PencilBeam)

struct Computations {
  Eigen::MatrixXd J;  // Jacobian

  Eigen::VectorXd y;  // Radiation vector
  Eigen::VectorXd
      x;  // A holder of currently optimal 'true' model input (in iterative
          // solvers) or of the a priori contribution (in linear solvers)
  Eigen::VectorXd x0;  // A priori estimation of 'true' model input

  Eigen::MatrixXd Sy;   // Covariance matrix of radiation vector
  Eigen::MatrixXd Sx0;  // Covariance matrix of a priori estimation

  /** Covariance matrix */
  auto S() const noexcept {
    return (Sx0.inverse() + J.transpose() * Sy.inverse() * J).inverse();
  }

  /** Contribution matrix */
  auto D() const noexcept { return S() * J.transpose() * Sy.inverse(); }

  /** Averaging kernel */
  auto A() const noexcept { return D() * J; }

  /** Linear optimal estimation of 'true' model input */
  auto xlin() const noexcept { return x + D() * y; }

  Computations(Eigen::MatrixXd sy, Eigen::MatrixXd sx0, Eigen::VectorXd x)
      : J(sy.cols(), sx0.cols()),
        y(sy.cols()),
        x0(std::move(x)),
        Sy(std::move(sy)),
        Sx0(std::move(sx0)) {
    if (Sx0.cols() not_eq Sx0.rows() or Sy.cols() not_eq Sy.rows() or
        x0.size() not_eq Sx0.rows()) {
      std::cerr << "Non-square covariance matrices or bad a priori size\n";
      std::terminate();
    }
    x = S() * Sx0.inverse() * x0;  //
  }
};

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
      if (deriv == Derivative::Type::Line) {
        nt += 1;
      } else {
        nt += natmdata();
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

Eigen::VectorXd map_derivs_to_x(const Atmosphere::Atm& atm,
                                const std::vector<Absorption::Band>& bands,
                                const std::vector<Derivative::Target>& derivs);

void set_input_from_x(Atmosphere::Atm& atm,
                      std::vector<Absorption::Band>& bands,
                      const std::vector<Derivative::Target>& derivs,
                      const Eigen::VectorXd& x);

ForwardConvolution compute_forward(
    const Atmosphere::Atm& atm, const Geom::Nav& pos_los,
    const std::vector<Absorption::Band>& bands,
    const std::vector<Derivative::Target>& derivs,
    const std::vector<Frequency<FrequencyType::Freq>>& sensor_f_grid,
    const std::size_t stokes_dimension, const BeamType beamtype);
};  // namespace RTE::Inverse

#endif  // inverse_h
