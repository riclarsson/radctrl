#ifndef asio_interface_h
#define asio_interface_h

#include <asio.hpp>
#include <iostream>

#include "timeclass.h"

namespace Network {
class Serial {
  asio::io_service io;
  asio::serial_port serial{io};

 public:
  Serial() {}

  Serial(const std::string& dev, unsigned int baudrate) {
    open(dev);
    set_baudrate(baudrate);
  }

  void set_baudrate(unsigned int baudrate) {
    serial.set_option(asio::serial_port::baud_rate(baudrate));
  }

  void open(const std::string& dev) { serial.open(dev); }

  void close() { serial.close(); }

  void write(const std::string& data) {
    asio::write(serial, asio::buffer(data.c_str(), data.size()));
  }

  std::string readline(const double timeout = 0.0) {
    std::string result;

    if (timeout <= 0) {
      asio::read_until(serial, asio::dynamic_buffer(result),
                       '\n');  // Blocks forever
    } else {
      std::error_code error;
      std::size_t n = 0;
      asio::async_read_until(
          serial, asio::dynamic_buffer(result), '\n',
          [&](const std::error_code& result_error, std::size_t result_n) {
            error = result_error;
            n = result_n;
          });

      // Run or time out
      run(std::chrono::duration_cast<std::chrono::steady_clock::duration>(
          TimeStep(timeout)));

      // throw an error if there is one
      if (error) throw std::system_error(error);

      // remove things not written
      result = result.substr(0, n - 1);
    }

    return result;
  }

 private:
  // Blocking function
  void run(std::chrono::steady_clock::duration timeout) {
    // Restart the io_context, as it may have been left in the "stopped" state
    // by a previous operation.
    io.restart();

    // Block until the asynchronous operation has completed, or timed out. If
    // the pending asynchronous operation is a composed operation, the deadline
    // applies to the entire operation, rather than individual operations on
    // the socket.
    io.run_for(timeout);

    // If the asynchronous operation completed successfully then the io_context
    // would have been stopped due to running out of work. If it was not
    // stopped, then the io_context::run_for call must have timed out.
    if (not io.stopped()) {
      // Close the target since something has gone very wrong
      serial.close();

      // Run the io_context again until the operation completes.
      io.run();
    }
  }
};
}  // namespace Network

#endif  // asio_interface_h
