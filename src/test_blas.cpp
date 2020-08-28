#include <Eigen/Eigenvalues>

#include "openblas_interface.h"

void test001() {
  const std::size_t N = 3;
  Eigen::MatrixXd m{Eigen::MatrixXd::Zero(N, N)};
  for (std::size_t i = 0; i < N; i++) m(i, i) = i + 3;
  m(N - 1, N - 2) = 1;
  std::cout << m << '\n';
  std::cout << m.inverse() << '\n';
}

void test002() {
  const std::size_t N = 3;
  Eigen::MatrixXcd m{Eigen::MatrixXcd::Zero(N, N)};
  for (std::size_t i = 0; i < N; i++) m(i, i) = Complex(i + 3, 5);
  m(N - 1, N - 2) = 1;
  m(N - 2, N - 3) = Complex(10, -10);
  
  Eigen::VectorXcd e(N);
  Eigen::MatrixXcd R(N, N), L(N, N);
  OpenBLAS::Helper::EigComplex comp;
  
  std::cout << '\n' << m << '\n';
  OpenBLAS::eig(e, L, R, m, comp);
  std::cout << e << '\n' << L << '\n' << R << '\n' << m << '\n';

  Eigen::ComplexEigenSolver<Eigen::MatrixXcd> solver(m);
  Eigen::MatrixXcd veig = solver.eigenvectors();
  Eigen::VectorXcd eeig = solver.eigenvalues();

//   for (size_t i = 0; i < N; i++) {
//     bool negative = false;
//     bool imaginary_larger = false;
//     double max = 0;
//     for (size_t j = 0; j < N; j++) {
//       double thisabs = std::abs(veig(i, j));
//       if (max < thisabs) {
//         max = thisabs;
//         if (std::abs(veig(i, j).imag()) > std::abs(veig(i, j).real())) {
//           imaginary_larger = true;
//           if (veig(i, j).imag() < 0)
//             negative = true;
//           else
//             negative = false;
//         } else {
//           imaginary_larger = false;
//           if (veig(i, j).real() < 0)
//             negative = true;
//           else
//             negative = false;
//         }
//       }
//     }
// 
//     if (negative and imaginary_larger)
//       veig.col(i) *= Complex(0, 1);
//     else if (negative)
//       veig.col(i) *= -1;
//     else if (imaginary_larger)
//       veig.col(i) *= Complex(0, -1);
//   }

  std::cout << eeig << '\n';
  std::cout << veig << '\n';
    
    std::cout << "Eigen:\n" << eeig << "\nLAPACK:\n" << e << '\n' << '\n';
    std::cout << "Eigen:\n" << veig << "\nLAPACK:\n" << R << '\n';
}

int main() {
  //   test001();
  test002();
}
