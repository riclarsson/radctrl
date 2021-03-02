#ifndef wobbler_iram_orig_h
#define wobbler_iram_orig_h

#include <filesystem>
#include <string>

#include "../python_interface.h"

namespace Instrument::Wobbler {
  class PythonOriginalIRAM {
    bool manual;
    bool error_found;
    int position;
    std::string error;
    
    Python::ClassInterface PyClass;
    Python::ClassInstance PyInst;
    Python::Function initfun;
    Python::Function movefun;
    Python::Function shutdown;
    Python::Function waitfun;
    Python::Function query;
    
  public:
    using DataType = int;
    
    PythonOriginalIRAM(const std::filesystem::path &path);
    
    void startup(std::string &dev, int baudrate, char address);
    
    void init(int pos, bool manual_press = false);
    
    void close();
    
    void move(int pos);
    
    void wait() const;
    
    DataType get_data() const;
    
    DataType get_data_raw() const;
    
    bool manual_run();
    
    const std::string &error_string() const;
    
    bool has_error();
    
    void delete_error();
  };  // PythonOriginalIRAM
}  // Instrument::Wobbler

#endif  // wobbler_iram_orig_h
