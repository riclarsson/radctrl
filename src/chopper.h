#ifndef chopper_h
#define chopper_h

#include <atomic>
#include <filesystem>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "enums.h"
#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"

#include "chopper/chopper_orig.h"

namespace Instrument {
namespace Chopper {
class Dummy {
  bool manual;
  ChopperPos pos;
  bool error_found;
  std::string error;

 public:
  using DataType = ChopperPos;
  template <typename... Whatever>
  constexpr Dummy(Whatever...)
      : manual(false), pos(ChopperPos::Cold), error_found(false), error("") {}
  void startup(const std::string &, int, double) {}
  void init(bool manual_press) {
    manual = manual_press;
    if (not manual) {
      error = "Must be manual, is dummy";
      error_found = true;
    }
  }
  void close() {}
  void run(ChopperPos x) {
    Sleep(0.1);
    pos = x;
  }
  DataType get_data_raw() { return ChopperPos::FINAL; }
  DataType get_data() { return pos; }
  bool manual_run() { return manual; }
  const std::string &error_string() const { return error; }
  bool has_error() { return error_found; }
  void delete_error() {
    error_found = false;
    error = "";
  }
};  // Dummy

typedef std::variant<Dummy, PythonOriginal> Interface;

struct Controller {
  static constexpr size_t N = 4;

  Interface chop;
  
  std::atomic<bool> init;
  std::atomic<bool> error;
  std::atomic<bool> quit;
  std::atomic<bool> run;
  std::atomic<bool> operating;
  std::atomic<bool> waiting;
  std::atomic<bool> newdata;

  std::string dev;
  int offset;
  double sleeptime;

  ChopperPos lasttarget;
  std::array<ChopperPos, N> pos;
  Controller(Interface&& iface, const std::string &d, int o, double s,
             ChopperPos pos1, ChopperPos pos2, ChopperPos pos3, ChopperPos pos4
  ) noexcept
      : chop(iface),
        init(false),
        error(false),
        quit(false),
        run(false),
        operating(false),
        waiting(false),
        newdata(false),
        dev(d),
        offset(o),
        sleeptime(s),
        lasttarget(ChopperPos::FINAL),
        pos({pos1, pos2, pos3, pos4}) {}
};

void GuiSetup(Controller &ctrl,
              const std::vector<std::string> &devs);
}  // namespace Chopper
}  // namespace Instrument

#undef PATH
#endif  // chopper_h
