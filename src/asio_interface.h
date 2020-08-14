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
  
  void set_baudrate(unsigned int baudrate) {
    serial.set_option(asio::serial_port::baud_rate(baudrate));
  }
  
  void open(const std::string& dev) {
    serial.open(dev);
  }
  
  void close() {serial.close();}
  
  void write(const std::string& data) {
    asio::write(serial, asio::buffer(data.c_str(), data.size()));
  }
  
  std::string readline() {
    char c;
    std::string result;
    while (true) {
      asio::read(serial, asio::buffer(&c, 1));
      switch(c)
      {
        case '\r':
          break;
        case '\n':
          return result;
        default:
          result+=c;
      }
    }
  }
};
}

#endif  // asio_interface_h
