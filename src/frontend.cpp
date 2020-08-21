#include "frontend.h"

#include "cli_parsing.h"
#include "timeclass.h"

namespace Instrument {
namespace Frontend {}
}  // namespace Instrument

template <class Frontend>
void runonce(std::string& server, int port, const std::string& python_file) {
  Frontend fe{python_file};

  fe.startup(server, port);
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.startup(\"" << server << "\", " << port
              << ").  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  fe.init();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.init().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  fe.run();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.run().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  fe.get_data();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.get_data().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  auto d = fe.data();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.data().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  fe.close();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.close().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  std::cout << Time() << '\n';
  for (auto& x : d) std::cout << x.first << ' ' << x.second << '\n';
}

template <class Frontend>
void run(std::string& server, int port, const std::string& python_file,
         const TimeStep dt) {
  if (dt.count() > 7 * 86400 or dt.count() < 0) {
    std::cerr
        << "Bad time: " << dt
        << ".  Allowed range 0 sec to ~ 1 week to avoid under-/overflow\n";
    std::exit(1);
  }

  Frontend fe{python_file};

  fe.startup(server, port);
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.startup(\"" << server << "\", " << port
              << ").  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  fe.init();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.init().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }

  for (;;) {
    Time wait_until{Time() +
                    dt};  // Set the wait_until time first in case operations
                          // inside are slower than the requested waiting

    fe.run();
    if (fe.has_error()) {
      std::cerr << "Failure calling fe.run().  Error:\n";
      std::cerr << fe.error_string() << '\n';
      std::exit(1);
    }

    fe.get_data();
    if (fe.has_error()) {
      std::cerr << "Failure calling fe.get_data().  Error:\n";
      std::cerr << fe.error_string() << '\n';
      std::exit(1);
    }

    auto d = fe.data();
    if (fe.has_error()) {
      std::cerr << "Failure calling fe.data().  Error:\n";
      std::cerr << fe.error_string() << '\n';
      std::exit(1);
    }

    std::cout << Time() << '\n';
    for (auto& x : d) std::cout << x.first << ' ' << x.second << '\n';

    Sleep(wait_until);
  }

  fe.close();
  if (fe.has_error()) {
    std::cerr << "Failure calling fe.close().  Error:\n";
    std::cerr << fe.error_string() << '\n';
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  CommandLine::App fe("Run a chopper machine");

  int machine;
  fe.NewRequiredOption("-m,--machine", machine,
                       "The class\n\t0: DBR (req. python driver)");
  std::string server;
  fe.NewRequiredOption("-s,--server", server,
                       "Device for the machine to connect to");
  int port = 1080;
  fe.NewDefaultOption("--port", port, "Port for the frontend");
  bool continue_running = false;
  fe.NewDefaultOption("-r,--run", continue_running,
                      "Runtime setting\n\t0: Shutdown after one run\n\t1: Run "
                      "until external interruption");
  double minimum_waittime = 5.0;
  fe.NewDefaultOption("-w,--waittime", minimum_waittime,
                      "Minimum wait time in seconds when running continuously");
  std::string pythonfile;
  fe.NewPlainOption("-p,--pythondriver", pythonfile,
                    "Python driving file for the machine (if applicable)");

  // Parse input options
  fe.Parse(argc, argv);

  if (machine == 0) {
    if (continue_running) {
      run<Instrument::Frontend::DBR>(server, port, pythonfile,
                                     TimeStep{minimum_waittime});
    } else {
      runonce<Instrument::Frontend::DBR>(server, port, pythonfile);
    }
  } else {
    std::cerr << "Bad machine, see --help\n";
    std::exit(1);
  }
}
