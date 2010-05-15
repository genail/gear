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

#include "ServerInfoResponse.h"

#include "common/gassert.h"
#include "network/events.h"

namespace Net
{

class ServerInfoResponseImpl
{
	public:

		ServerInfoResponse *m_parent;

		CL_String m_serverName;
		CL_String m_mapName;
		int m_playerCount;
		int m_playerLimit;


		ServerInfoResponseImpl(ServerInfoResponse *p_parent);
		~ServerInfoResponseImpl();

		CL_NetGameEvent buildEvent() const;
		void parseEvent(const CL_NetGameEvent &p_event);
};

ServerInfoResponse::ServerInfoResponse() :
		m_impl(new ServerInfoResponseImpl(this))
{
	// empty
}

ServerInfoResponseImpl::ServerInfoResponseImpl(ServerInfoResponse *p_parent) :
		m_parent(p_parent)
{
	// empty
}

ServerInfoResponse::~ServerInfoResponse()
{
	// empty
}

ServerInfoResponseImpl::~ServerInfoResponseImpl()
{
	// empty
}

CL_NetGameEvent ServerInfoResponse::buildEvent() const
{
	return m_impl->buildEvent();
}

CL_NetGameEvent ServerInfoResponseImpl::buildEvent() const
{
	CL_NetGameEvent event(EVENT_INFO_RESPONSE);

	event.add_argument(m_serverName);
	event.add_argument(m_mapName);
	event.add_argument(m_playerCount);
	event.add_argument(m_playerLimit);

	return event;
}

void ServerInfoResponse::parseEvent(const CL_NetGameEvent &p_event)
{
	m_impl->parseEvent(p_event);
}

void ServerInfoResponseImpl::parseEvent(const CL_NetGameEvent &p_event)
{
	G_ASSERT(p_event.get_name() == EVENT_INFO_RESPONSE);

	m_serverName = p_event.get_argument(0);
	m_mapName = p_event.get_argument(1);
	m_playerCount = p_event.get_argument(2);
	m_playerLimit = p_event.get_argument(3);
}

void ServerInfoResponse::setServerName(const CL_String &p_name)
{
	m_impl->m_serverName = p_name;
}

const CL_String &ServerInfoResponse::getServerName() const
{
	return m_impl->m_serverName;
}

void ServerInfoResponse::setMapName(const CL_String &p_mapName)
{
	m_impl->m_mapName = p_mapName;
}

const CL_String &ServerInfoResponse::getMapName() const
{
	return m_impl->m_mapName;
}

void ServerInfoResponse::setPlayerCount(int p_playerCount)
{
	m_impl->m_playerCount = p_playerCount;
}

int ServerInfoResponse::getPlayerCount() const
{
	return m_impl->m_playerCount;
}

void ServerInfoResponse::setPlayerLimit(int p_playerLimit)
{
	m_impl->m_playerLimit = p_playerLimit;
}

int ServerInfoResponse::getPlayerLimit() const
{
	return m_impl->m_playerLimit;
}

}
