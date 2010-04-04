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

#include "MasterServerRegistrant.h"

#include "common.h"
#include "common/Properties.h"
#include "network/masterserver/MasterServer.h"

namespace Net
{

const int REGISTER_PERDIOD_MS = 50000;

MasterServerRegistrant::MasterServerRegistrant()
{
	// empty
}

MasterServerRegistrant::~MasterServerRegistrant()
{
	// empty
}

void MasterServerRegistrant::run()
{
	MasterServer masterServer;
	bool registered = false;
	const int serverPort = Properties::getInt(SRV_PORT, DEFAULT_PORT);

	do {
		try {
			if (!registered) {
				cl_log_event(LOG_DEBUG, "registering game server");
				const bool registerSucceed = masterServer.registerGameServer(serverPort);

				if (registerSucceed) {
					cl_log_event(LOG_INFO, "game server registered to MS");
					registered = true;
				} else {
					cl_log_event(LOG_WARN, "game server registration failed");
				}
			} else {
				cl_log_event(LOG_DEBUG, "keeping alive game server");
				const bool keepAliveSucceed = masterServer.keepAliveGameServer(serverPort);

				if (keepAliveSucceed) {
					cl_log_event(LOG_INFO, "game server kept alive (MS)");
				} else {
					cl_log_event(LOG_INFO, "game server keep alive failed (MS)");
					registered = false;
				}
			}
		} catch (CL_Exception &e) {
			cl_log_event(LOG_WARN, "fatal while registering/keeping alive: " + e.message);
		}
	} while (CL_Event::wait(m_eventInterrupted, REGISTER_PERDIOD_MS) != 0);
}

void MasterServerRegistrant::interrupt()
{
	m_eventInterrupted.set();
}

}
