#include "openblas_interface.h"

namespace OpenBLAS {

extern "C" void dgetrf_(int *m, int *n, double *A, int *lda, int *ipiv,
                        int *info);
extern "C" void dgetri_(int *n, double *A, int *lda, int *ipiv, double *work,
                        int *lwork, int *info);
extern "C" void zgetrf_(int *m, int *n, Complex *A, int *lda, int *ipiv,
                        int *info);
extern "C" void zgetri_(int *n, Complex *A, int *lda, int *ipiv, Complex *work,
                        int *lwork, int *info);
extern "C" void zgeev_(char *jobvl, char *jobvr, int *n, Complex *A, int *lda,
                       Complex *W, Complex *VL, int *ldvl, Complex *VR,
                       int *ldvr, Complex *work, int *lwork, double *rwork,
                       int *info);

Eigen::MatrixXd &inv(Eigen::MatrixXd &m, Helper::Inverse<double> &comp) {
  if (m.cols() not_eq m.rows()) {
    std::cerr << "Non-square matrix\n";
    std::exit(1);
  }

  comp.resize_if_smaller(m.cols());

  int info;
  dgetrf_(comp.size(), comp.size(), m.data(), comp.size(), comp.ipivdata(),
          &info);
  dgetri_(comp.size(), m.data(), comp.size(), comp.ipivdata(), comp.workdata(),
          comp.lsize(), &info);

  return m;
}

Eigen::MatrixXcd &inv(Eigen::MatrixXcd &m, Helper::Inverse<Complex> &comp) {
  if (m.cols() not_eq m.rows()) {
    std::cerr << "Non-square matrix\n";
    std::exit(1);
  }

  comp.resize_if_smaller(m.cols());

  int info;
  zgetrf_(comp.size(), comp.size(), m.data(), comp.size(), comp.ipivdata(),
          &info);
  zgetri_(comp.size(), m.data(), comp.size(), comp.ipivdata(), comp.workdata(),
          comp.lsize(), &info);

  return m;
}

void eig(Eigen::VectorXcd &E, Eigen::MatrixXcd &L, Eigen::MatrixXcd &R,
         Eigen::MatrixXcd &A, Helper::EigComplex &comp) {
  if (A.cols() not_eq A.rows() or L.cols() not_eq L.rows() or
      R.cols() not_eq R.rows() or E.size() not_eq A.cols()) {
    std::cerr << "Bad sizes\n";
    std::exit(1);
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
