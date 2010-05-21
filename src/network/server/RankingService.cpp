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

#include "RankingService.h"

#include <string>

#include "common/loglevels.h"
#include "network/events.h"
#include "network/packets/RankingAdvance.h"
#include "network/packets/RankingEntries.h"
#include "network/packets/RankingFind.h"
#include "network/packets/RankingRequest.h"
#include "ranking/LocalRanking.h"

namespace Net
{

class RankingServiceImpl
{
	public:

		LocalRanking m_localRanking;

		void parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);
		void parseRankingFindEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);
		void parseRankingAdvanceEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);
		void parseRankingRequestEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		RankingEntries prepareRankingEntriesPacket(int p_placeFrom, int p_placeTo);

		void sendPacket(CL_NetGameConnection *p_conn, const Packet &p_packet);

};

RankingService::RankingService() :
		m_impl(new RankingServiceImpl())
{
	// empty
}

RankingService::~RankingService()
{
	// empty
}

void RankingService::parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	m_impl->parseEvent(p_conn, p_event);
}

void RankingServiceImpl::parseEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_RANKING_FIND) {
		parseRankingFindEvent(p_conn, p_event);
	} else if (eventName == EVENT_RANKING_ADVANCE) {
		parseRankingAdvanceEvent(p_conn, p_event);
	} else if (eventName == EVENT_RANKING_REQUEST) {
		parseRankingRequestEvent(p_conn, p_event);
	}
}

void RankingServiceImpl::parseRankingFindEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event
) {
	RankingFind rankingFindPacket;
	rankingFindPacket.parseEvent(p_event);

	const int index = m_localRanking.findEntryIndex(rankingFindPacket.getPlayerId());

	RankingEntries rankingEntriesPacket;
	rankingEntriesPacket.setToken(rankingFindPacket.getToken());

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

void RankingServiceImpl::parseRankingAdvanceEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event
)
{
	RankingAdvance rankingAdvancePacket;
	rankingAdvancePacket.parseEvent(p_event);

	const RankingEntry &rankingEntry = rankingAdvancePacket.getRankingEntry();
	m_localRanking.advanceEntry(rankingEntry);
}

void RankingServiceImpl::parseRankingRequestEvent(
		CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event
)
{
	static const int ENTRY_COUNT_LIMIT = 20;

	RankingRequest rankingRequestPacket;
	rankingRequestPacket.parseEvent(p_event);

	int placeFrom = rankingRequestPacket.getPlaceFrom();
	int placeTo = rankingRequestPacket.getPlaceTo();

	if (placeFrom > placeTo) {
		std::swap(placeFrom, placeTo);
	}

	const int entryRequestCount = placeTo - placeFrom;
	if (entryRequestCount <= ENTRY_COUNT_LIMIT) {

		RankingEntries rankingEntriesPacket = prepareRankingEntriesPacket(placeFrom, placeTo);
		rankingEntriesPacket.setToken(rankingRequestPacket.getToken());
		sendPacket(p_conn, rankingEntriesPacket);
	} else {
		cl_log_event(LOG_WARN, "entry request count exceeds the limit (%1)", entryRequestCount);
	}
}

RankingEntries RankingServiceImpl::prepareRankingEntriesPacket(int p_placeFrom, int p_placeTo)
{
	RankingEntries rankingEntriesPacket;
	PlacedRankingEntry rankingEntry;
	try {
		for (int i = p_placeFrom; i <= p_placeTo; ++i) {
			rankingEntry = m_localRanking.getEntryAtPosition(i);
			rankingEntriesPacket.addEntry(rankingEntry);
		}
	} catch (CL_Exception &e) {
		// that's fine, just stop collecting the data
	}

	return rankingEntriesPacket;
}

void RankingServiceImpl::sendPacket(CL_NetGameConnection *p_conn, const Packet &p_packet)
{
	const CL_NetGameEvent netEvent = p_packet.buildEvent();
	p_conn->send_event(netEvent);
}

}
