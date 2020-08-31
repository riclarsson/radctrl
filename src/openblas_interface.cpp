#include "openblas_interface.h"

namespace OpenBLAS {

extern "C" void zgeev_(char *jobvl, char *jobvr, int *n, Complex *A, int *lda,
                       Complex *W, Complex *VL, int *ldvl, Complex *VR,
                       int *ldvr, Complex *work, int *lwork, double *rwork,
                       int *info);

void eig(Eigen::VectorXcd &E, Eigen::MatrixXcd &L, Eigen::MatrixXcd &R,
         Eigen::MatrixXcd &A, Helper::EigComplex &comp) {
  if (A.cols() not_eq A.rows() or L.cols() not_eq L.rows() or
      R.cols() not_eq R.rows() or E.size() not_eq A.cols()) {
    std::cerr << "Bad sizes\n";
    std::terminate();
  }

  comp.resize_if_smaller(A);

  char l = L.cols() == A.cols() ? 'V' : 'R';
  char r = R.cols() == A.cols() ? 'V' : 'R';
  int info;

  zgeev_(&l, &r, comp.size(), comp.copydata(), comp.ld(), E.data(), L.data(),
         comp.ldl(), R.data(), comp.ldr(), comp.workdata(), comp.lsize(),
         comp.rightwork(), &info);
}
}  // namespace OpenBLAS
