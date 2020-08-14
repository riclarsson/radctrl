#include "asio_interface.h"

namespace Network {
class Serial {
  asio::serial_port ser;
  
public:
  Serial(const std::string& dev, int baudrate) : ser({}, dev) {
    ser.set_option(asio::serial_port::baud_rate(baudrate));
  }
};
}
