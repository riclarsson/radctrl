#include <Eigen/Eigenvalues>
#include <iomanip>

#include "openblas_interface.h"

void test001() {
  const std::size_t N = 3;
  Eigen::MatrixXcd m{Eigen::MatrixXcd::Zero(N, N)};
  for (std::size_t i = 0; i < N; i++) m(i, i) = Complex(i + 3, 5);
  m(N - 1, N - 2) = 1;
  m(N - 2, N - 3) = Complex(10, -10);

  Eigen::VectorXcd e(N);
  Eigen::MatrixXcd R(N, N), L(N, N);
  BLAS::Helper::EigComplex comp;

  std::cout << std::setprecision(16) << '\n' << m << '\n';
  BLAS::eig(e, L, R, m, comp);
  std::cout << e << '\n' << L << '\n' << R << '\n' << m << '\n';

  Eigen::ComplexEigenSolver<Eigen::MatrixXcd> solver(m);
  Eigen::MatrixXcd veig = solver.eigenvectors();
  Eigen::VectorXcd eeig = solver.eigenvalues();

  std::cout << eeig << '\n';
  std::cout << veig << '\n';

  std::cout << "Eigen:\n" << eeig << "\nLAPACK:\n" << e << '\n' << '\n';
  std::cout << "Eigen:\n" << veig << "\nLAPACK:\n" << R << '\n' << '\n';

  std::cout << "A:\n" << m << '\n';
  std::cout << "Eigen A=PeP^{-1}:\n"
            << veig * eeig.asDiagonal() * veig.inverse() << '\n';
  std::cout << "LAPACK A=PeP^{-1}:\n"
            << R * e.asDiagonal() * R.inverse() << '\n';

  std::cout << "eeig:\n" << eeig << '\n';
  std::cout << "Eigen e=P^{-1}AP:\n"
            << (veig.inverse() * m * veig).diagonal() << '\n';
  std::cout << "e:\n" << e << '\n';
  std::cout << "LAPACK e=P^{-1}AP:\n"
            << (R.inverse() * m * R).diagonal() << '\n';
}

int main() { test001(); }
