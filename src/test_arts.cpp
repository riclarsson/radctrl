#include <iostream>

#include "arts/interface.h"

int main() {
  auto x = ARTS::test();
  std::cout << '\n';
  std::cout << "DOH" << '\n';
  std::cout << x << '\n';
}
