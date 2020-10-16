#ifndef openblas_interface_h
#define openblas_interface_h

#include <Eigen/Dense>

#include "complex.h"
#include "grids.h"

namespace BLAS {
namespace Helper {
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

  bool resize_if_smaller(const Eigen::MatrixXcd &orig) {
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

  int *size() noexcept { return &n; }
  int *ldr() noexcept { return &ldvr; }
  int *ldl() noexcept { return &ldvl; }
  int *ld() noexcept { return &lda; }
  int *lsize() noexcept { return &lwork; }
  Complex *copydata() noexcept { return copy.data(); }
  Complex *workdata() noexcept { return work.data(); }
  double *rightwork() noexcept { return rwork.data(); }
};
}  // namespace Helper

void eig(Eigen::VectorXcd &E, Eigen::MatrixXcd &L, Eigen::MatrixXcd &R,
         Eigen::MatrixXcd &A, Helper::EigComplex &comp);
}  // namespace BLAS

#endif  // openblas_interface_h
