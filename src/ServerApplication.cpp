/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ServerApplication.h"

#include "ClanLib/network.h"

#include "common/loglevels.h"
#include "common/Properties.h"
#include "network/server/Server.h"
#include "network/server/TimeTrailServer.h"

CL_ClanApplication app(&ServerApplication::main);

Net::Server *createTimeTrailServer()
{
	cl_log_event(LOG_INFO, "starting TIME TRAIL server");
	return new Net::TimeTrailServer();
}

Net::Server *createArcadeServer()
{
	cl_log_event(LOG_INFO, "starting ARCADE server");
	return new Net::Server();
}

Net::Server *createServer()
{
	const CL_String gameModeStr = Properties::getString(SRV_GAME_MODE, SRV_GAME_MODE_ARCADE);

	if (gameModeStr == SRV_GAME_MODE_ARCADE) {
		return createArcadeServer();
	} else if (gameModeStr == SRV_GAME_MODE_TIMETRAIL) {
		return createTimeTrailServer();
	} else {
		throw CL_Exception("unknown game mode: " + gameModeStr);
	}
}

void destroyServer(Net::Server *&p_server)
{
	if (p_server != NULL) {
		delete p_server;
		p_server = NULL;
	}
}

int ServerApplication::main(const std::vector<CL_String> &args)
{
	Net::Server *server = NULL;

	try {
		CL_SetupCore setup_core;
		CL_SetupNetwork setup_network;
		CL_ConsoleLogger logger;

		Properties::load(CONFIG_FILE_SERVER);

		server = createServer();
		server->start();

		while (true) {
			CL_KeepAlive::process();
			CL_System::sleep(2);
		}
	} catch (CL_Exception e) {
		CL_Console::write_line("exception thrown: %1", e.message);
	}

	destroyServer(server);
	return 0;
}
