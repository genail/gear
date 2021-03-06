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

#include "TimeTrailServer.h"

#include "network/events.h"
#include "network/server/RankingService.h"

namespace Net
{

class TimeTrailServerImpl
{
	public:

		TimeTrailServer *m_parent;
		RankingService m_rankingServer;


		TimeTrailServerImpl(TimeTrailServer *p_parent);

		bool handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);
		void handleRankingEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);
};

TimeTrailServer::TimeTrailServer() :
		m_impl(new TimeTrailServerImpl(this))
{
	// empty
}

TimeTrailServerImpl::TimeTrailServerImpl(TimeTrailServer *p_parent) :
		m_parent(p_parent)
{
	// empty
}

TimeTrailServer::~TimeTrailServer()
{
	// empty
}

void TimeTrailServer::handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	const bool handled = m_impl->handleEvent(p_conn, p_event);

	if (!handled) {
		Server::handleEvent(p_conn, p_event);
	}
}

bool TimeTrailServerImpl::handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	if (isRankingEvent(p_event)) {
		handleRankingEvent(p_conn, p_event);
		return true;
	} else {
		return false;
	}
}

void TimeTrailServerImpl::handleRankingEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	m_rankingServer.parseEvent(p_conn, p_event);
}

TGameMode TimeTrailServer::getGameMode() const
{
	return GM_TIME_TRAIL;
}

}
