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

#include "RankingClient.h"

#include "network/events.h"
#include "common/Game.h"
#include "network/client/Client.h"
#include "network/packets/RankingAdvance.h"
#include "network/packets/RankingEntries.h"
#include "network/packets/RankingFind.h"
#include "network/packets/RankingRequest.h"

namespace Net
{

class RankingClientImpl
{
	public:

		RankingClient *m_parent;
		Client *m_client;

		RankingClientImpl(RankingClient *p_parent, Client *p_client) :
			m_parent(p_parent),
			m_client(p_client)
		{ /* empty */ }

		void parseEvent(const CL_NetGameEvent &p_event);
		void parseEntriesEvent(const CL_NetGameEvent &p_event);

		int findEntry(const CL_String &p_playerId);
};


RankingClient::RankingClient(Client *p_client) :
		m_impl(new RankingClientImpl(this, p_client))
{
	// empty
}

RankingClient::~RankingClient()
{
	// empty
}

void RankingClient::sendTimeAdvance(int p_lapTimeMs)
{
	const Player &localPlayer = Game::getInstance().getPlayer();

	Net::RankingAdvance rankingAdvancePacket;
	RankingEntry rankingEntry;

	rankingEntry.pid = localPlayer.getId();
	rankingEntry.name = localPlayer.getName();
	rankingEntry.timeMs = p_lapTimeMs;

	rankingAdvancePacket.setRankingEntry(rankingEntry);
	const CL_NetGameEvent netEvent = rankingAdvancePacket.buildEvent();

	m_impl->m_client->send(netEvent);
}

int RankingClient::requestEntry(int p_place)
{
	return requestEntries(p_place, p_place);
}

int RankingClient::requestEntries(int p_placeFrom, int p_placeTo)
{
	RankingRequest packet;
	packet.setPlaceFrom(p_placeFrom);
	packet.setPlaceTo(p_placeTo);

	const CL_NetGameEvent event = packet.buildEvent();
	m_impl->m_client->send(event);

	return packet.getToken();
}

int RankingClient::findEntry(const CL_String &p_playerId)
{
	return m_impl->findEntry(p_playerId);
}

int RankingClientImpl::findEntry(const CL_String &p_playerId)
{
	RankingFind packet;
	packet.setPlayerId(p_playerId);

	const CL_NetGameEvent event = packet.buildEvent();
	m_client->send(event);

	return packet.getToken();
}

void RankingClient::parseEvent(const CL_NetGameEvent &p_event)
{
	m_impl->parseEvent(p_event);
}

void RankingClientImpl::parseEvent(const CL_NetGameEvent &p_event)
{
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_RANKING_ENTRIES) {
		parseEntriesEvent(p_event);
	} else {
		cl_log_event(LOG_ERROR, "event remains unhandled: %1", p_event.to_string());
	}
}

void RankingClientImpl::parseEntriesEvent(const CL_NetGameEvent &p_event)
{
	RankingEntries rankingEntriesPacket;
	rankingEntriesPacket.parseEvent(p_event);

	const int entryCount = rankingEntriesPacket.getEntryCount();
	std::vector<PlacedRankingEntry> rankingEntries;
	rankingEntries.reserve(entryCount);

	for (int i = 0; i < entryCount; ++i) {
		const PlacedRankingEntry &rankingEntry = rankingEntriesPacket.getEntry(i);
		rankingEntries.push_back(rankingEntry);
	}

	const int token = rankingEntriesPacket.getToken();
	m_parent->sig_entriesReceived.invoke(token, rankingEntries);
}

}
