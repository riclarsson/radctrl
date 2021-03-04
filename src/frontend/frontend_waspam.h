#ifndef frontend_waspam_h
#define frontend_waspam_h

#include <filesystem>
#include <string>

#include "../python_interface.h"

namespace Instrument::Frontend {
  class Waspam {
    bool manual;
    std::string mname;
    std::map<std::string, double> database;
    bool error_found;
    std::string error;
    
  public:
    static constexpr bool has_cold_load = false;
    static constexpr bool has_hot_load = false;
    static constexpr bool has_config = false;
    using DataType = std::map<std::string, double>;
    
    template <typename... Whatever>
    constexpr Waspam(Whatever...)
    : manual(false), mname("Waspam"), error_found(false), error("") {}
    template <typename... Whatever>
    void startup(Whatever...) {}
    void init(bool manual_press) { manual = manual_press; }
    void close() {}
    void run() {}
    void get_data() const {}
    DataType data() const { return database; }
    bool manual_run() { return manual; }
    const std::string &error_string() const { return error; }
    bool has_error() { return error_found; }
    void delete_error() {
      error_found = false;
      error = "";
    }
    const std::string &name() const { return mname; }
  };
}

#endif  // frontend_waspam_h
