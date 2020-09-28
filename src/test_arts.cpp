#include <iostream>

#include "arts/interface.h"

int main() try {
  auto x = ARTS::test();
  std::cout << '\n';
  std::cout << "DOH" << '\n';
  std::cout << x << '\n';
} catch(const std::exception& e) {
  std::cerr << "EXITING WITH ERROR:\n" << e.what() << '\n';
  return 1;
}
