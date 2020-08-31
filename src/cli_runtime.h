#ifndef cli_runtime_h
#define cli_runtime_h

#include <cli.h>
#include <clifilesession.h>

namespace CommandLine {
class Runtime;

class RuntimeSetup {
  bool ok;
  std::unique_ptr<cli::Menu> main;

 public:
  RuntimeSetup(const std::string &menu)
      : ok(true), main(std::make_unique<cli::Menu>(menu)) {}

  template <class Function>
  void Insert(const std::string &cmd, Function &&f, const std::string &desc) {
    if (ok) {
      main->Insert(cmd.c_str(), f, desc.c_str());
    } else {
      std::cerr << "Bad option addition, "
                << "*this has been made invalid [DEV ERROR]\n";
      std::terminate();
    }
  }
  void Insert(RuntimeSetup &c) {
    if (ok and c.ok) {
      main->Insert(std::move(c.main));
      c.ok = false;
    } else {
      std::cerr << "Bad menu addition, "
                << "*this has been made invalid [DEV ERROR]\n";
      std::terminate();
    }
  }

  [[nodiscard]] Runtime Run();
};  // RuntimeSetup

class Runtime {
  cli::CliFileSession input;

 public:
  Runtime(cli::Cli &&c) : input(c) { input.Start(); }
};

}  // namespace CommandLine

#endif  // cli_runttime_h
