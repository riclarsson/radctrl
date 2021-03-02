#ifndef wobbler_h
#define wobbler_h

#include <atomic>
#include <string>
#include <variant>
#include <vector>

#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"

#include "wobbler/wobbler_waspam_orig.h"
#include "wobbler/wobbler_iram_orig.h"

namespace Instrument {
namespace Wobbler {

class Dummy {
  bool manual;
  bool error_found;
  int position;
  std::string error;

 public:
  using DataType = int;
  template <typename... Whatever>
  constexpr Dummy(Whatever...)
      : manual(false), error_found(false), position(4000), error("") {}
  void startup(const std::string &, int, char) {}
  void init(int pos, bool manual_press = false) {
    manual = manual_press;
    position = pos;
    if (not manual) {
      error = "Must be manual, is dummy";
      error_found = true;
    }
  }
  void close() {}
  void move(int pos) { position = pos; }
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
  void wait() const { Sleep(0.1); }
  DataType get_data() const { return position; }
};  // Dummy

typedef std::variant<Dummy, PythonOriginalWASPAM, PythonOriginalIRAM> Interface;

struct Controller {
  static constexpr size_t N = 4;
  
  Interface wob;

  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;

  std::string dev;
  int baudrate;
  char address;

  std::array<int, N> pos;
  
  Controller(Interface&& iface, const std::string &d, int b, char a) noexcept;
};

void GuiSetup(Controller &ctrl,
              const std::vector<std::string> &devs);
}  // namespace Wobbler
}  // namespace Instrument

#endif  // wobbler_h
