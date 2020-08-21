#include "housekeeping.h"

#include "cli_parsing.h"

namespace Instrument {
namespace Housekeeping {}  // namespace Housekeeping
}  // namespace Instrument

template <class Housekeeping>
void runonce(std::string& dev, int baud, const std::string& python_file) {
  Housekeeping hk{python_file};

  hk.startup(dev, baud);
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.startup(\"" << dev << "\", " << baud
              << ").  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  hk.init();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.init().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  hk.run();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.run().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  hk.get_data();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.get_data().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  auto d = hk.data();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.data().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  hk.close();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.close().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  std::cout << Time();
  for (auto& x : d) std::cout << ' ' << x.first << ' ' << x.second;
  std::cout << '\n';
}

template <class Housekeeping>
void run(std::string& dev, int baud, const std::string& python_file,
         const TimeStep dt) {
  if (dt.count() > 7 * 86400 or dt.count() < 0) {
    std::cerr
        << "Bad time: " << dt
        << ".  Allowed range 0 sec to ~ 1 week to avoid under-/overflow\n";
    std::exit(1);
  }

  Housekeeping hk{python_file};

  hk.startup(dev, baud);
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.startup(\"" << dev << "\", " << baud
              << ").  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  hk.init();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.init().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }

  for (;;) {
    Time wait_until{Time() +
                    dt};  // Set the wait_until time first in case operations
                          // inside are slower than the requested waiting

    hk.run();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.run().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::exit(1);
    }

    hk.get_data();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.get_data().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::exit(1);
    }

    auto d = hk.data();
    if (hk.has_error()) {
      std::cerr << "Failure calling hk.data().  Error:\n";
      std::cerr << hk.error_string() << '\n';
      std::exit(1);
    }

    std::cout << Time();
    for (auto& x : d) std::cout << ' ' << x.first << ' ' << x.second;
    std::cout << '\n';

    Sleep(wait_until);
  }

  hk.close();
  if (hk.has_error()) {
    std::cerr << "Failure calling hk.close().  Error:\n";
    std::cerr << hk.error_string() << '\n';
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  CommandLine::App hk("Run a housekeeping machine");

  int machine;
  hk.NewRequiredOption("-m,--machine", machine,
                       "The class\n\t0: Agilent34970A (no python driver)");
  std::string dev;
  hk.NewRequiredOption("-d,--dev", dev, "Device for the machine to connect to");
  int baud;
  hk.NewRequiredOption("-b,--baud", baud, "Baud rate");
  bool continue_running = false;
  hk.NewDefaultOption("-r,--run", continue_running,
                      "Runtime setting\n\t0: Shutdown after one run\n\t1: Run "
                      "until external interruption");
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
      run<Instrument::Housekeeping::Agilent34970A>(dev, baud, pythonfile,
                                                   TimeStep{minimum_waittime});
    } else {
      runonce<Instrument::Housekeeping::Agilent34970A>(dev, baud, pythonfile);
    }
  } else {
    std::cerr << "Bad machine, see --help\n";
    std::exit(1);
  }
}
