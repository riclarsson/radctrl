#include "cli_parsing.h"
#include "python_interface.h"
#include "instrument.h"
#include "xml_config.h"

bool run (const File::ConfigParser& parser) {
  std::cout << "Options Parsed\n\n" << parser << '\n';
  
  bool good = true;
  
  // Chopper declaration
  try {
    Instrument::Chopper::Controller chopper_ctrl = Instrument::Chopper::parse(parser);
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse Chopper with error:\n" << e.what() << '\n';
  }
  
  // Wobbler declaration
  try {
    Instrument::Wobbler::Controller wobbler_ctrl = Instrument::Wobbler::parse(parser);
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse Wobbler with error:\n" << e.what() << '\n';
  }
  
  // Housekeeping declaration
  try {
    Instrument::Housekeeping::Controller housekeeping_ctrl = Instrument::Housekeeping::parse(parser);
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse Housekeeping with error:\n" << e.what() << '\n';
  }
  
  // Frontend declaration
  try {
  Instrument::Frontend::Controller frontend_ctrl = Instrument::Frontend::parse(parser);
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse Frontend with error:\n" << e.what() << '\n';
  }
  
  // Backend declaration
  try {
    Instrument::Spectrometer::Controllers spectrometer_ctrls = Instrument::Spectrometer::parse(parser);
    
    // Deal with internal parsing
    const std::vector<std::string> specs = parser.keys("Backends");
    const int size = std::stoi(std::string(parser("Backends", "size")));
    for (int i=0; i<size; i++) {
      for (auto& key: specs) {
        if (key.find(" pos") not_eq std::string::npos) {
          const int pos = std::stoi(std::string(parser("Backends", key)));
          if (pos == i) {
            // At the right spectrometer
            const std::string controller_name{begin(key), end(key)-4};
            const File::ConfigParser internal_parser(std::string(parser("Backends", controller_name)),
                                                    std::vector<std::string>{controller_name});
            std::cout << "Internal Parser Backend Index " << i << '\n' << internal_parser << '\n';
          }
        }
      }
    }
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse Spectrometer(s) with error:\n" << e.what() << '\n';
  }
  
  // DataSaver declarations
  try {
    std::filesystem::path save_path{parser("Savepath", "path")};
    Instrument::DataSaver datasaver(save_path, "TEST_NAME");
  } catch (std::runtime_error& e) {
    good = false;
    std::cerr << "Couldn't parse DataSaver with error:\n" << e.what() << '\n';
  }
  
  return good;
}

int main(int argc, char ** argv) try {
  auto py = Python::createPython();
  
  CommandLine::App rad("Parse the XML file to check if configuration is working");
  
  std::string xmlfilename;
  rad.NewRequiredOption("--xml", xmlfilename, "Configuration file to parse");
  
  rad.Parse(argc, argv);
  
  if (run(File::ConfigParser(xmlfilename, {"Chopper", "Wobbler", "Housekeeping", "Frontend", "Backends", "Operations", "Savepath"}))) {
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Error: Failed parsing the file on this computer\n";
    return EXIT_FAILURE;
  }
} catch(std::runtime_error& e) {
  std::cerr << "Failed with error:\n" << e.what() << '\n';
  return EXIT_FAILURE;
}
