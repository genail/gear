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

#pragma once

#include "clanlib/core/system.h"

#ifndef MS_DEFAULT_HOST
#define MS_DEFAULT_HOST "torvalds.rootnode.net"
#endif

#ifndef MS_DEFAULT_PORT
#define MS_DEFAULT_PORT 37005
#endif

#define MS_PROTOCOL_MAJOR 1
#define MS_PROTOCOL_MINOR 0

namespace Net
{

class MasterServerImpl;

class MasterServer
{
	public:

		struct GameServer {
			CL_String m_addr;
			int m_port;
		};

		MasterServer(
				const CL_String &p_host = MS_DEFAULT_HOST,
				int p_port = MS_DEFAULT_PORT
		);

		virtual ~MasterServer();

		bool registerGameServer(int p_gameServerPort);
		bool keepAliveGameServer(int p_gameServerPort);

		bool requestGameServerList();
		int gameServerListCount() const;

		const GameServer &gameServerAt(int p_index) const;



	private:

		CL_SharedPtr<MasterServerImpl> m_impl;

};

}

