#ifndef jacobian_h
#define jacobian_h

#include <Eigen/Core>

#include "derivatives.h"

namespace Derivative {

class Jacobian {
  Eigen::MatrixXd mjac;

 public:
  Jacobian(std::size_t nr, std::size_t nc) noexcept
      : mjac(Eigen::MatrixXd::Zero(nr, nc)) {}
};
}  // namespace Derivative

#endif  // jacobian_h
