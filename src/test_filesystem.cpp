#include <algorithm>
#include <filesystem>
#include <iostream>

int main() {
  for (auto& p : std::filesystem::directory_iterator("/dev/")) {
    const std::string dev = std::string{p.path().c_str()};
    if (auto s = dev.find("/dev/ttyS"); s not_eq std::string::npos)
      std::cout << dev << '\n';
    if (auto s = dev.find("/dev/ttyUSB"); s not_eq std::string::npos)
      std::cout << dev << '\n';
  }
}
