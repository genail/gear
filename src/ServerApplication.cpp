/*
 * ServerApplication.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "ServerApplication.h"

#include "network/Server.h"

CL_ClanApplication app(&ServerApplication::main);

int ServerApplication::main(const std::vector<CL_String> &args)
{
	Server server(1234);

	while (true) {
		server.update(5);
		CL_System::sleep(10);
	}
}
