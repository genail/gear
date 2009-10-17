/*
 * ServerApplication.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "ServerApplication.h"

#include "ClanLib/network.h"

#include "network/Server.h"
#include "ServerConfiguration.h"

CL_ClanApplication app(&ServerApplication::main);

int ServerApplication::main(const std::vector<CL_String> &args)
{
	try {
		CL_SetupCore setup_core;
		CL_SetupNetwork setup_network;

		CL_ConsoleLogger logger;

		// load the server configuration
		ServerConfiguration config;

		Server server(config.getPort());

		while (true) {
			CL_KeepAlive::process();
			server.update(10);
			CL_System::sleep(10);
		}
	} catch (CL_Exception e) {
		CL_Console::write_line("Exception thrown: %1", e.message);
	}

	return 0;
}
