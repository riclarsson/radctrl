#include "housekeeping.h"

#include "cli_parsing.h"

namespace Instrument {
namespace Housekeeping {}  // namespace Housekeeping
}  // namespace Instrument

template <class Housekeeping>
void runonce(std::string &dev, int baud, const std::string &python_file) {
  Housekeeping hk{python_file};

  hk.startup(dev, baud);
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.startup(\"" << dev << "\", " << baud
              << ").  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  hk.init();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.init().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  hk.run();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.run().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  hk.get_data();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.get_data().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  auto d = hk.data();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.data().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  hk.close();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.close().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  std::cout << Time() << '\n';
  for (auto &x : d) std::cout << x.first << ' ' << x.second << '\n';
}

template <class Housekeeping>
void run(std::string &dev, int baud, const std::string &python_file,
         const TimeStep dt, bool clear_terminal) {
  if (dt.count() > 7 * 86400 or dt.count() < 0) {
    std::cerr
        << "Bad time: " << dt
        << ".  Allowed range 0 sec to ~ 1 week to avoid under-/overflow\n";
    std::terminate();
  }

  Housekeeping hk{python_file};

  hk.startup(dev, baud);
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.startup(\"" << dev << "\", " << baud
              << ").  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  hk.init();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.init().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }

  for (;;) {
    Time wait_until{Time() +
                    dt};  // Set the wait_until time first in case operations
                          // inside are slower than the requested waiting

    hk.run();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.run().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::terminate();
    }

    hk.get_data();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.get_data().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::terminate();
    }

    auto d = hk.data();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.data().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::terminate();
    }

    if (clear_terminal) std::printf("\033c");
    std::cout << Time() << '\n';
    for (auto &x : d) std::cout << x.first << ' ' << x.second << '\n';

    Sleep(wait_until);
  }

  hk.close();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.close().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::terminate();
  }
}

int main(int argc, char **argv) {
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  CommandLine::App hk("Run a housekeeping machine");

  int machine;
  hk.NewRequiredOption("-m,--machine", machine,
                       "The class\n"
                       "\t0: Agilent34970A (no python driver)"
                       "\t1: PythonSensors (req. python driver)");
  std::string dev;
  hk.NewRequiredOption("-d,--dev", dev, "Device for the machine to connect to");
  int baud;
  hk.NewRequiredOption("-b,--baud", baud, "Baud rate");
  bool continue_running = false;
  hk.NewDefaultOption("-r,--run", continue_running,
                      "Runtime setting\n\t0: Shutdown after one run\n\t1: Run "
                      "until external interruption");
  bool clear_terminal = false;
  hk.NewDefaultOption(
      "-c,--clear", clear_terminal,
      "Clear terminal setting\n\t0: Do not clear\n\t1: Do clear");
  double minimum_waittime = 5.0;
  hk.NewDefaultOption("-w,--waittime", minimum_waittime,
                      "Minimum wait time in seconds when running continuously");
  std::string pythonfile;
  hk.NewPlainOption("-p,--pythondriver", pythonfile,
                    "Python driving file for the machine (if applicable)");

  // Parse input options
  hk.Parse(argc, argv);

  if (machine == 0) {
    if (continue_running) {
      run<Instrument::Housekeeping::Agilent34970A>(
          dev, baud, pythonfile, TimeStep{minimum_waittime}, clear_terminal);
    } else {
      runonce<Instrument::Housekeeping::Agilent34970A>(dev, baud, pythonfile);
    }
  } else if (machine == 1) {
    if (continue_running) {
      run<Instrument::Housekeeping::PythonSensors>(
          dev, baud, pythonfile, TimeStep{minimum_waittime}, clear_terminal);
    } else {
      runonce<Instrument::Housekeeping::PythonSensors>(dev, baud, pythonfile);
    }
  } else {
    std::cerr << "Bad machine, see --help\n";
    std::terminate();
  }
}
