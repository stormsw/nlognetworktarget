// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "udp_server.h"
#include <windows.h> 
#include <stdio.h> 


boost::asio::io_service io_service;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		io_service.stop();
		return(TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		io_service.stop();
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		io_service.stop();
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		io_service.stop();
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		io_service.stop();
		return FALSE;

	default:
		return FALSE;
	}
	/*
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	signals.async_wait(
		boost::bind(&boost::asio::io_service::stop, &io_service));//*/
}

//[ example_tutorial_file_advanced
//void init()
//{
//	logging::add_file_log
//		(
//		keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
//		keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
//		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
//		keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
//		);
//
//	logging::core::get()->set_filter
//		(
//		logging::trivial::severity >= logging::trivial::info
//		);
//}
//]


int _tmain(int argc, _TCHAR* argv[])
{
	unsigned short port=PORT;

	printf("\nStarting listener...\n");
	if(argc==1)
	{
		printf("\nPort is not specified, default %d used\n",PORT);
	} else 
	{
		try{
			std::basic_istringstream<_TCHAR> ss(argv[1]);		
			ss >> port;
			printf("\nPort specified as %d\n",port);
		} catch (...)
		{
			printf("\nPort specified incorrectly, default %d used\n",PORT);
		}
	}

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		printf("\nThe Control Handler is installed.\n");
		printf("\n -- Now try pressing Ctrl+C or Ctrl+Break, or");
		printf("\n    try logging off or closing the console...\n");
		printf("\n(...waiting in a loop for events...)\n\n");

		try
		{
			udp_server server(io_service,port);
			io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		printf("\nERROR: Could not set control handler");
		return 1;

	}

	return 0;
}

