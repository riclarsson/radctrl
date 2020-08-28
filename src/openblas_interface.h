#ifndef openblas_interface_h
#define openblas_interface_h

#include <Eigen/Dense>

#include "complex.h"
#include "grids.h"

namespace OpenBLAS {
namespace Helper {
template <class base>
struct Inverse {
  int n;
  int lwork;
  Eigen::Matrix<base, -1, -1> work;
  Eigen::VectorXi ipiv;

  explicit Inverse() noexcept : n(0) {}

  bool resize_if_smaller(std::size_t N) {
    if (int(N) > n) {
      n = int(N);
      lwork = int(N);
      work.resize(1, N);
      ipiv.resize(N);
      return true;
    } else {
      return false;
    }
  }

  int* size() noexcept { return &n; }
  int* lsize() noexcept { return &lwork; }
  base* workdata() noexcept { return work.data(); }
  int* ipivdata() noexcept { return ipiv.data(); }
};

struct EigComplex {
  int n;
  int lwork;
  int ldvr;
  int ldvl;
  int lda;
  Eigen::VectorXcd work;
  Eigen::VectorXd rwork;
  Eigen::MatrixXcd copy;

  explicit EigComplex() noexcept : n(0) {}

  bool resize_if_smaller(const Eigen::MatrixXcd& orig) {
    const bool resize = orig.cols() > n;
    copy.noalias() = orig;
    if (resize) {
      n = int(orig.cols());
      lwork = 2 * n;
      ldvr = n;
      ldvl = n;
      lda = n;
      work.resize(lwork);
      rwork.resize(lwork);
      return true;
    } else {
      return false;
    }
  }

  int* size() noexcept { return &n; }
  int* ldr() noexcept { return &ldvr; }
  int* ldl() noexcept { return &ldvl; }
  int* ld() noexcept { return &lda; }
  int* lsize() noexcept { return &lwork; }
  Complex* copydata() noexcept { return copy.data(); }
  Complex* workdata() noexcept { return work.data(); }
  double* rightwork() noexcept { return rwork.data(); }
};
}  // namespace Helper

/** In-place inverse */
Eigen::MatrixXd& inv(Eigen::MatrixXd& m, Helper::Inverse<double>& comp);

/** In-place inverse */
Eigen::MatrixXcd& inv(Eigen::MatrixXcd& m, Helper::Inverse<Complex>& comp);

void eig(Eigen::VectorXcd& E, Eigen::MatrixXcd& L, Eigen::MatrixXcd& R,
         Eigen::MatrixXcd& A, Helper::EigComplex& comp);
}  // namespace OpenBLAS

#endif  // openblas_interface_h
