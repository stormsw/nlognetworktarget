#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
//#include 

// logging set
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace src = boost::log::sources;

using boost::asio::ip::udp;
#define SIZE 26
#define PORT 9999

class udp_server
{
public:
    udp_server(boost::asio::io_service& io_service, unsigned short port=PORT);
  ~udp_server(void);
private:
  void start_receive();
  void handle_receive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/);
  void handle_send(boost::shared_ptr<std::string> /*message*/,
      const boost::system::error_code& /*error*/,
      std::size_t /*bytes_transferred*/);
  void log(char * buf);
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 4096> recv_buffer_;
  src::logger lg;
};

