#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
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

  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 4096> recv_buffer_;
};

