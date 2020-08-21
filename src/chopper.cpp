#include "chopper.h"
#include "cli_parsing.h"
#include "cli_runtime.h"

namespace Instrument {
namespace Chopper {}  // namespace Chopper
}  // namespace Instrument

template <class ChopperClass>
void run (const std::string& dev, int offset, double sleeptime, const std::string& python_file) {
  ChopperClass chop(python_file);
  
  chop.startup(dev, offset, sleeptime);
  if (chop.has_error()) {
    std::cerr << "Failure calling chop.startup(\""<<dev<<"\", "<<offset<<", "<<sleeptime<<").  Error:\n";
    std::cerr << chop.error_string() << '\n';
    std::exit(1);
  }
  
  CommandLine::RuntimeSetup setup("chop");
  setup.Insert("cold", [&chop](std::ostream& out){out<<"Moving to cold position\n"; chop.run(Instrument::Chopper::ChopperPos::Cold);}, "Point at cold position");
  setup.Insert("hot", [&chop](std::ostream& out){out<<"Moving to hot position\n"; chop.run(Instrument::Chopper::ChopperPos::Hot);}, "Point at hot position");
  setup.Insert("ant", [&chop](std::ostream& out){out<<"Moving to antenna position\n"; chop.run(Instrument::Chopper::ChopperPos::Antenna);}, "Point at antenna position");
  setup.Insert("ref", [&chop](std::ostream& out){out<<"Moving to reference position\n"; chop.run(Instrument::Chopper::ChopperPos::Reference);}, "Point at reference position");
  setup.Insert("init", [&chop](std::ostream& out){out<<"Initialize (run first)\n"; chop.init();}, "Initialize the machine");
  setup.Insert("close", [&chop](std::ostream& out){out<<"Close down (run last)\n"; chop.close();}, "Close the machine");
  auto rt = setup.Run();
}

int main(int argc, char ** argv) {
  CommandLine::App chop("Run a chopper machine");
  
  int machine;
  chop.NewRequiredOption("-m,--machine", machine, "The chopper class\n\tOptions:\n\t\t0: PythonOriginal (req. python driver)");
  std::string dev;
  chop.NewRequiredOption("-d,--dev", dev, "Device for the machine to connect to");
  int offset;
  chop.NewRequiredOption("-o,--offset", offset, "Offset of antenna (if implemented)");
  double sleeptime;
  chop.NewRequiredOption("-s,--sleeptime", sleeptime, "Sleeptime (if implemented)");
  std::string pythonfile;
  chop.NewPlainOption("-p,--pythondriver", pythonfile, "Python driving file for the Housekeeping machine (if applicable)");
  
  // Parse input options
  chop.Parse(argc, argv);
  
  if (machine == 0) {
    run<Instrument::Chopper::PythonOriginal>(dev, offset, sleeptime, pythonfile);
  } else {
    std::cerr << "Bad machine, see --help\n";
    std::exit(1);
  }
}
