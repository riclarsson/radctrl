#include "grids.h"

void test001() {
  Grid<double, 2> g(0, 4, 4);
  std::cout << g << '\n' << '\n';

  for (std::size_t i = 0; i <= 16; i++) g.data()[i] = i + 1;

  std::cout << "element order flat:\n" << g << '\n' << '\n';

  std::size_t i = 0;
  for (std::size_t j = 0; j < 4; j++) {
    for (std::size_t k = 0; k < 4; k++) {
      g(j, k) = i + 1;
      i++;
    }
  }

  std::cout << "element order by index access operator()(outer, inner):\n"
            << g << '\n'
            << '\n';
}

int main() { test001(); }
