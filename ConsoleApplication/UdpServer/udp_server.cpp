#include "stdafx.h"
#include "udp_server.h"

// logging set
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#define SIZE 26
#define PORT 9999


using boost::asio::ip::udp;

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;


void log(char * buf)
{
	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	//BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

	// Filtering can also be applied
	//using namespace boost::log;

	//logging::add_file_log
	//	(
	//	keywords::file_name = "sample_%N.log", 1
	//	keywords::rotation_size = 10 * 1024 * 1024, 2
	//	keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 3
	//	keywords::format = "[%TimeStamp%]: %Message%"                                 4
	//	);

	logging::core::get()->set_filter
		(
		logging::trivial::severity >= logging::trivial::info
		);

	// Now the first two lines will not pass the filter
	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	//BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

	BOOST_LOG_TRIVIAL(info) << buf;
}

using boost::asio::ip::udp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;

	time_t ltime;
	char buf[SIZE];
	errno_t err;

	time(&ltime);
	err = ctime_s(buf, SIZE, &ltime);
	if (err != 0)
	{
		printf("Invalid Arguments for _wctime_s. Error Code: %d\n", err);
	}
	return buf;
}

udp_server::udp_server(boost::asio::io_service& io_service)
: socket_(io_service, udp::endpoint(udp::v4(), PORT))
{
	start_receive();
}


void udp_server::start_receive()
{
	socket_.async_receive_from(
		boost::asio::buffer(recv_buffer_), remote_endpoint_,
		boost::bind(&udp_server::handle_receive, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_receive(const boost::system::error_code& error,
	std::size_t bytes_transferred)
{
	if (!error || error == boost::asio::error::message_size)
	{
		boost::shared_ptr<std::string> message(
			new std::string(make_daytime_string()));

		/*
		socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
		boost::bind(&udp_server::handle_send, this, message,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
		*/
		char buf1[4096];

		memcpy_s(buf1, 4096, recv_buffer_.data(), bytes_transferred);
		buf1[bytes_transferred] = 0;
		printf(buf1);
		log(buf1);
		start_receive();
	}
}

void udp_server::handle_send(boost::shared_ptr<std::string> /*message*/,
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/)
{
}

udp_server::~udp_server(void)
{
}
