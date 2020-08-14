#ifndef asio_interface_h
#define asio_interface_h

#include <iostream>

#include <asio.hpp>

namespace Network {
class Serial {
  asio::serial_port ser;
  
public:
  Serial() : ser({}) {
  }
  
  void set_baudrate(size_t baudrate) {
    ser.set_option(asio::serial_port::baud_rate(baudrate));
  }
  
  void open(const std::string& dev) {ser.open(dev);}
  
  void close() {ser.close();}
  
  void write(const std::string& data) {
    asio::error_code ec;
    asio::write(ser, asio::const_buffer(data.data(), data.size()), ec);
    if (ec)
      std::cerr << ec << '\n';
  }
  
  std::string readline() {
    std::string data;
    asio::error_code ec;
    size_t n = asio::read_until(ser, asio::dynamic_buffer(data), '\n', ec);
    if (ec)
      std::cerr << ec << '\n';
    return data.substr(0, n);
  }
};
}

#endif  // asio_interface_h
