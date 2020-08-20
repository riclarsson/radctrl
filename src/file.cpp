#include "file.h"

std::vector<std::string> File::Devices(std::vector<std::string> contains,
                                       size_t maxN) {
  std::vector<std::string> out;
  for (auto& key : contains) {
    // Unnumbered
    const std::string path = std::string("/dev/tty") + key;
    for (auto& p : std::filesystem::directory_iterator("/dev/")) {
      const std::string dev = std::string{p.path().c_str()};
      if (path == dev) out.push_back(dev);
    }

    // Numbered
    for (size_t N = 0; N <= maxN; N++) {
      const std::string path =
          std::string("/dev/tty") + key + std::to_string(N);
      for (auto& p : std::filesystem::directory_iterator("/dev/")) {
        const std::string dev = std::string{p.path().c_str()};
        if (path == dev) out.push_back(dev);
      }
    }
  }

  return out;
}
