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

#include "common/Game.h"
#include "network/client/Client.h"
#include "network/packets/RankingAdvance.h"

namespace Net
{

class RankingClientImpl
{
	public:

		Client *m_client;


		RankingClientImpl(Client *p_client) :
			m_client(p_client)
		{ /* empty */ }

};

RankingClient::RankingClient(Client *p_client) :
		m_impl(new RankingClientImpl(p_client))
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

}
