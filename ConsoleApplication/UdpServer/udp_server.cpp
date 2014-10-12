#include "stdafx.h"
#include "udp_server.h"


using boost::asio::ip::udp;

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;


enum severity_level
{
    normal,
    notification,
    warning,
    error,
    critical
};

// Declare attribute keywords
BOOST_LOG_ATTRIBUTE_KEYWORD(_severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(_timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(_uptime, "Uptime", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(_scope, "Scope", attrs::named_scope::value_type)

void udp_server::log(char * buf)
{
	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	//BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
	// Now the first two lines will not pass the filter
	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	//BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

	//BOOST_LOG_TRIVIAL(info) << buf;

	
	BOOST_LOG(lg) << buf;

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

udp_server::udp_server(boost::asio::io_service& io_service, unsigned short port )
: socket_(io_service, udp::endpoint(udp::v4(), port))
{
	// Filtering can also be applied
	//namespace log = boost::log;

	logging::add_console_log(std::clog, keywords::format = "%TimeStamp%: %_%");

	logging::add_file_log
		(
		keywords::file_name = "sample_%N.log", 
		keywords::rotation_size = 10 * 1024 * 1024, 
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = expr::stream
            << expr::format_date_time(_timestamp, "%Y-%m-%d, %H:%M:%S.%f")
            << " [" << expr::format_date_time(_uptime, "%O:%M:%S")
            << "] [" << expr::format_named_scope(_scope, keywords::format = "%n (%f:%l)")
            << "] <" << _severity
            << "> " << expr::message
		);
	
	    logging::add_common_attributes();
    logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());

    BOOST_LOG_FUNCTION();

    // Now our logs will be written both to the console and to the file.
    // Let's do a quick test and output something. We have to create a logger for this.
    src::logger lg;

    // And output...
    BOOST_LOG(lg) << "Hello, World!";

    // Now, let's try logging with severity
    src::severity_logger< severity_level > slg;

    // Let's pretend we also want to profile our code, so add a special timer attribute.
    slg.add_attribute("Uptime", attrs::timer());

    BOOST_LOG_SEV(slg, normal) << "A normal severity message, will not pass to the file";
    BOOST_LOG_SEV(slg, warning) << "A warning severity message, will pass to the file";
    BOOST_LOG_SEV(slg, error) << "An error severity message, will pass to the file";

	BOOST_LOG_TRIVIAL(info) << "Test trivial";
/*	logging::core::get()->set_filter
		(
		logging::trivial::severity >= logging::trivial::info
		);*/

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
		BOOST_LOG_FUNCTION();
		//printf(buf1);
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
