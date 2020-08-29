#include "chopper.h"

#include "cli_parsing.h"
#include "cli_runtime.h"

namespace Instrument {
namespace Chopper {}  // namespace Chopper
}  // namespace Instrument

template <class ChopperClass>
void run(const std::string &dev, int offset, double sleeptime,
         const std::string &python_file) {
  ChopperClass chop(python_file);

  chop.startup(dev, offset, sleeptime);
  if (chop.has_error()) {
    std::cerr << "Failure calling chop.startup(\"" << dev << "\", " << offset
              << ", " << sleeptime << ").  Error:\n";
    std::cerr << chop.error_string() << '\n';
    std::exit(1);
  }

  CommandLine::RuntimeSetup setup("chop");
  setup.Insert(
      "init",
      [&](std::ostream &out) {
        chop.init();
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Initialized\n";
        }
      },
      "Initialize the machine");
  setup.Insert(
      "cold",
      [&](std::ostream &out) {
        chop.run(Instrument::Chopper::ChopperPos::Cold);
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Moved to cold position\n";
        }
      },
      "Point at cold position");
  setup.Insert(
      "hot",
      [&](std::ostream &out) {
        chop.run(Instrument::Chopper::ChopperPos::Hot);
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Moved to hot position\n";
        }
      },
      "Point at hot position");
  setup.Insert(
      "ant",
      [&](std::ostream &out) {
        chop.run(Instrument::Chopper::ChopperPos::Antenna);
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Moved to antenna position\n";
        }
      },
      "Point at antenna position");
  setup.Insert(
      "ref",
      [&](std::ostream &out) {
        chop.run(Instrument::Chopper::ChopperPos::Reference);
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Moved to reference position\n";
        }
      },
      "Point at reference position");
  setup.Insert(
      "close",
      [&](std::ostream &out) {
        chop.close();
        if (chop.has_error()) {
          std::cerr << chop.error_string() << '\n';
          chop.delete_error();
        } else {
          out << "Closed down\n";
        }
      },
      "Close the machine");
  auto rt = setup.Run();
}

int main(int argc, char **argv) {
  // Start a python interpreter in case python code will be executed
  auto py = Python::createPython();

  CommandLine::App chop("Run a chopper machine");

  int machine;
  chop.NewRequiredOption("-m,--machine", machine,
                         "The class\n\t0: PythonOriginal (req. python driver)");
  std::string dev;
  chop.NewRequiredOption("-d,--dev", dev,
                         "Device for the machine to connect to");
  int offset = 1000;
  chop.NewDefaultOption("-o,--offset", offset,
                        "Offset of antenna (if implemented)");
  double sleeptime = 0.2;
  chop.NewDefaultOption("-s,--sleeptime", sleeptime,
                        "Sleeptime (if implemented)");
  std::string pythonfile;
  chop.NewPlainOption("-p,--pythondriver", pythonfile,
                      "Python driving file for the machine (if applicable)");

  // Parse input options
  chop.Parse(argc, argv);

  if (machine == 0) {
    run<Instrument::Chopper::PythonOriginal>(dev, offset, sleeptime,
                                             pythonfile);
  } else {
    std::cerr << "Bad machine, see --help\n";
    std::exit(1);
  }
}
