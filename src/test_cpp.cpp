#include <cmath>
#include <iostream>

#include <Eigen/Dense>

Eigen::IOFormat HeavyFmt(Eigen::FullPrecision, 0, ", ", ",\n", "[", "]", "[", "]");

void test_cmath() {
  constexpr unsigned N = 15;
  Eigen::MatrixXd out = Eigen::MatrixXd::Zero(N, N);
  
  double theta = 0.5;
  for (unsigned l=0; l<N; l++)
    for (unsigned m=0; m<N; m++)
      out(l, m) = std::sph_legendre(l, m, theta);
  std::cout << "x = \\\n" << out.format(HeavyFmt) << '\n';
}

int main() {
  test_cmath();
}
