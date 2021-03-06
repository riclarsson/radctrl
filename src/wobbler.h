#ifndef wobbler_h
#define wobbler_h

#include <atomic>
#include <string>
#include <variant>
#include <vector>

#include "gui.h"
#include "python_interface.h"
#include "timeclass.h"
#include "xml_config.h"

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
  
  //! Explicit copy
  Controller(const Controller& c) noexcept :
  wob(c.wob), init(c.init.load()), error(c.error.load()), quit(c.quit.load()), run(c.run.load()), operating(c.operating.load()), waiting(c.waiting.load()), dev(c.dev), baudrate(c.baudrate), address(c.address), pos(c.pos)
  {}
  
  //! Explicit move
  Controller(Controller&& c) noexcept :
  wob(std::move(c.wob)), init(c.init.load()), error(c.error.load()), quit(c.quit.load()), run(c.run.load()), operating(c.operating.load()), waiting(c.waiting.load()), dev(std::move(c.dev)), baudrate(c.baudrate), address(c.address), pos(std::move(c.pos))
  {}
  
  void startup();
  
  void initialize(int pos, bool manual);
  
  void close();
  
  int get_pos();
  
  void change_pos(int pos);
  
  bool has_error();
  
  void wait();
  
  const std::string& error_string() const;
  
  void delete_error();
};

void GuiSetup(Controller &ctrl,
              const std::vector<std::string> &devs);

Controller parse(const File::ConfigParser& parser);
}  // namespace Wobbler
}  // namespace Instrument

#endif  // wobbler_h
