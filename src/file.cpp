#include "file.h"

namespace File {
std::vector<std::string> Devices(std::vector<std::string> contains,
                                       size_t maxN) {
  std::vector<std::string> out;
  for (auto &key : contains) {
    // Unnumbered
    const std::string path = std::string("/dev/tty") + key;
    for (auto &p : std::filesystem::directory_iterator("/dev/")) {
      const std::string dev = std::string{p.path().c_str()};
      if (path == dev) out.push_back(dev);
    }

    // Numbered
    for (size_t N = 0; N <= maxN; N++) {
      const std::string path =
          std::string("/dev/tty") + key + std::to_string(N);
      for (auto &p : std::filesystem::directory_iterator("/dev/")) {
        const std::string dev = std::string{p.path().c_str()};
        if (path == dev) out.push_back(dev);
      }
    }
  }

  return out;
}

std::vector<std::vector<double>> parse_columndata(File<Operation::Read, Type::Raw> file, std::size_t skiprows) {
  for(std::size_t i=0; i<skiprows; i++) file.getline();
  const std::string first_str = file.getline();
  std::istringstream first_line(first_str);
  std::vector<std::vector<double>> out(1);
  
  // Read the first line row
  constexpr std::size_t max_count = 1000;
  std::size_t cur_count = 0;
  while (not first_line.eof()) {
    double x;
    first_line >> x;
    cur_count++;
    out.back().emplace_back(x);
    if (cur_count >= max_count) {
      std::ostringstream os;
      os << "Reached max count (" << max_count << ") on first line.  Line:\n\t" << "'" << first_str << "'";
      throw std::runtime_error(os.str());
    }
  }
  
  const std::size_t n = out.back().size();
  while (true) {
    for (std::size_t i=0; i<n; i++) {
      double x;
      file >> x;
      if (not i and file.at_end())
        return out;
      else if (not i)
        out.emplace_back(n);
      else if (file.at_end())
        throw std::runtime_error("Reached end of file unexpectedly");
      out.back()[i] = x;
    }
  }
}
}
