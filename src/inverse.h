#ifndef inverse_h
#define inverse_h

#include <Eigen/Dense>

#include "antenna.h"
#include "atm.h"
#include "derivatives.h"
#include "lbl.h"

namespace RTE::Inverse {

struct Results {
  Eigen::VectorXd y;
  Eigen::VectorXd x;
  Eigen::VectorXd fx;
  Eigen::MatrixXd J;
  Eigen::MatrixXd S;
  Eigen::MatrixXd D;
  Eigen::MatrixXd A;
  // FIXME: Add sine-fits and polynominal fits
};

ENUMCLASS(ComputationsType, unsigned char, Linear)

struct Computations {
  ComputationsType ct;  // Type of computations
  bool done;            // Checks whether we should continue

  Eigen::MatrixXd J;  // Jacobian

  Eigen::VectorXd y;         // Measurements
  Eigen::VectorXd fx;        // Simulations
  const Eigen::VectorXd x0;  // A priori estimation of 'true' model input

  const Eigen::MatrixXd Sy;   // Covariance matrix of radiation vector
  const Eigen::MatrixXd Sx0;  // Covariance matrix of a priori estimation

  /** Covariance matrix */
  auto S() const noexcept {
    return (Sx0.inverse() + J.transpose() * Sy.inverse() * J).inverse();
  }

  /** Contribution matrix */
  auto D() const noexcept { return S() * J.transpose() * Sy.inverse(); }

  /** Averaging kernel */
  auto A() const noexcept { return D() * J; }

  /** Linear optimal estimation of 'true' model input */
  Eigen::VectorXd update_x() noexcept {
    switch (ct) {
      case ComputationsType::Linear:
        done = true;
        return fx + D() * y;          //  fx holds a priori estimation
      case ComputationsType::FINAL: { /* Leave last */
      }
    }
    done = true;
    return x0;
  }

  bool is_done() const noexcept { return done; }

  Computations(ComputationsType comp_type, Eigen::MatrixXd sy,
               Eigen::MatrixXd sx0, Eigen::VectorXd xinit,
               Eigen::VectorXd yinit)
      : ct(comp_type),
        done(false),
        J(sy.cols(), sx0.cols()),
        y(std::move(yinit)),
        x0(std::move(xinit)),
        Sy(std::move(sy)),
        Sx0(std::move(sx0)) {
    if (Sx0.cols() not_eq Sx0.rows() or Sy.cols() not_eq Sy.rows() or
        x0.size() not_eq Sx0.rows() or yinit.size() not_eq Sy.cols()) {
      std::cerr << "Non-square covariance matrices AND/OR bad a priori size "
                   "AND/OR bad measurement size\n";
      std::terminate();
    }
    if (ct == ComputationsType::Linear) fx = S() * Sx0.inverse() * x0;
  }
};

Eigen::VectorXd map_input_to_x(const Atmosphere::Atm& atm,
                               const std::vector<Absorption::Band>& bands,
                               const std::vector<Derivative::Target>& derivs);

void set_input_from_x(Atmosphere::Atm& atm,
                      std::vector<Absorption::Band>& bands,
                      const Geom::Nav& pos_los,
                      const std::vector<Derivative::Target>& derivs,
                      const Eigen::VectorXd& x);

Results compute(Atmosphere::Atm& atm, std::vector<Absorption::Band>& bands,
                const std::vector<Derivative::Target>& derivs,
                const Sensor::Properties& sensor_prop,
                const Distance<DistanceType::meter> layer_thickness,
                const Eigen::MatrixXd& sy, const Eigen::MatrixXd& sx,
                const Eigen::VectorXd& y, const Eigen::VectorXd& x0);

}  // namespace RTE::Inverse

#endif  // inverse_h
