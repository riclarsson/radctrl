#ifndef cli_parsing_h
#define cli_parsing_h

#include <string>

#include <App.hpp>
#include <CLI.hpp>

namespace CommandLine {
  class App {
    CLI::App app;
  public:
    App(const std::string& desc, const std::string& name="") noexcept : app(desc, name) {}
    
  void Parse(int argc, char ** argv) {
    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
      std::exit(app.exit(e));
    }
  }
    
  template <class T>
  void NewPlainOption(const std::string& name, T& var, const std::string& help="") {app.add_option(name, var, help);}
  
  template <class T>
  void NewDefaultOption(const std::string& name, T& var, const std::string& help="") {app.add_option(name, var, help, true);}
  
  template <class T>
  void NewRequiredOption(const std::string& name, T& var, const std::string& help="") {app.add_option(name, var, help) -> required();}
  };
}

#endif  // cli_parsing_h
