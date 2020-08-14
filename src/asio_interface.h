#ifndef asio_interface_h
#define asio_interface_h

#include <iostream>

#include <asio.hpp>

namespace Network {
class Serial {
  asio::io_service io;
  asio::serial_port serial;
  
public:
  Serial() : io(), serial(io) {
  }
  
  void set_baudrate(size_t baudrate) {
    serial.set_option(asio::serial_port::baud_rate(baudrate));
  }
  
  void open(const std::string& dev) {serial.open(dev);}
  
  void close() {serial.close();}
  
  void write(const std::string& data) {
    asio::error_code ec;
    asio::write(serial, asio::const_buffer(data.data(), data.size()), ec);
    if (ec)
      std::cerr << ec << '\n';
  }
  
  std::string readline() {
    std::string data;
    asio::error_code ec;
    size_t n = asio::read_until(serial, asio::dynamic_buffer(data), '\n', ec);
    if (ec)
      std::cerr << ec << '\n';
    return data.substr(0, n);
  }
};
}

#endif  // asio_interface_h
