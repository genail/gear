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

#include "RankingServer.h"

#include "network/events.h"
#include "network/packets/RankingFind.h"
#include "network/packets/RankingEntries.h"
#include "network/packets/RankingAdvance.h"
#include "ranking/LocalRanking.h"

namespace Net
{

class RankingServerImpl
{
	public:

		LocalRanking m_localRanking;


		void parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		void parseRankingFindEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		void parseRankingAdvanceEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

};

RankingServer::RankingServer() :
		m_impl(new RankingServerImpl())
{
	// empty
}

RankingServer::~RankingServer()
{
	// empty
}

void RankingServer::parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	m_impl->parseEvent(p_conn, p_event);
}

void RankingServerImpl::parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_RANKING_FIND) {
		parseRankingFindEvent(p_conn, p_event);
	} else if (eventName == EVENT_RANKING_ADVANCE) {
		parseRankingAdvanceEvent(p_conn, p_event);
	}
}

void RankingServerImpl::parseRankingFindEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event
) {
	RankingFind rankingFindPacket;
	rankingFindPacket.parseEvent(p_event);

	const int index = m_localRanking.findEntryIndex(rankingFindPacket.getPlayerId());

	RankingEntries rankingEntriesPacket;

	if (index != -1) {
		RankingEntry rankingEntry = m_localRanking.getEntryAtIndex(index);
		PlacedRankingEntry placedRankingEntry;

		placedRankingEntry.pid = rankingEntry.pid;
		placedRankingEntry.name = rankingEntry.name;
		placedRankingEntry.timeMs = rankingEntry.timeMs;

		placedRankingEntry.place = m_localRanking.getEntryRankingPosition(index);

		rankingEntriesPacket.addEntry(placedRankingEntry);
	}

	p_conn->send_event(rankingEntriesPacket.buildEvent());
}

void RankingServerImpl::parseRankingAdvanceEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event
)
{
	RankingAdvance rankingAdvancePacket;
	rankingAdvancePacket.parseEvent(p_event);

	const RankingEntry &rankingEntry = rankingAdvancePacket.getRankingEntry();
	m_localRanking.advanceEntry(rankingEntry);
}

}
