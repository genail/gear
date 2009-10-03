/*
 * ServerApplication.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "ServerApplication.h"

#include "ClanLib/network.h"

#include "network/Server.h"

CL_ClanApplication app(&ServerApplication::main);

int ServerApplication::main(const std::vector<CL_String> &args)
{
	CL_SetupCore setup_core;
	CL_SetupNetwork setup_network;

	CL_ConsoleLogger logger;

	Server server(1234);

	while (true) {
		CL_KeepAlive::process();
		server.update(10);
		CL_System::sleep(10);
	}
}
