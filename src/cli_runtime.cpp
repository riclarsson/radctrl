#include "cli_runtime.h"

namespace CommandLine {
Runtime RuntimeSetup::Run() {
  if (ok) {
    cli::Cli cli(std::move(main));
    cli.ExitAction([](auto &out) {
      out << "Thanks for using.  Have a nice day!  Exiting...\n";
    });
    return cli;
  } else {
    std::cerr << "Bad run\n";
    std::terminate();
  }
}
}  // namespace CommandLine
