/*
 * Copyright (c) 2009-2010 The Gear Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Gear nor the
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

#include "RankingFindQuery.h"

#include "common/gassert.h"
#include "common/Game.h"

namespace Net
{

class RankingFindQueryImpl
{
	public:
		RankingFindQuery *m_parent;

		int m_token;

		CL_String m_playerId;

		bool m_found;
		PlacedRankingEntry m_entry;

		CL_Slot m_recvSlot;


		RankingFindQueryImpl(RankingFindQuery *p_parent);
		~RankingFindQueryImpl();

		void submit();

		void onEntriesReceived(int p_token, const std::vector<PlacedRankingEntry> &p_entries);
};

// ----------------------------------------------------------------------------

RankingFindQuery::RankingFindQuery() :
		m_impl(new RankingFindQueryImpl(this))
{
	// empty
}

RankingFindQueryImpl::RankingFindQueryImpl(RankingFindQuery *p_parent) :
		m_parent(p_parent),
		m_token(-1),
		m_found(false)
{
	// empty
}

RankingFindQuery::~RankingFindQuery()
{
	// empty
}

RankingFindQueryImpl::~RankingFindQueryImpl()
{
	// empty
}

// ----------------------------------------------------------------------------

void RankingFindQuery::setPlayerId(const CL_String &p_playerId)
{
	m_impl->m_playerId = p_playerId;
}

void RankingFindQuery::submit()
{
	m_impl->submit();
}

void RankingFindQueryImpl::submit()
{
	G_ASSERT(!m_playerId.empty() && "player id not set");

	m_parent->start();

	Game &game = Game::getInstance();
	Client &client = game.getNetworkConnection();
	RankingClient &rankingClient = client.getRankingClient();

	m_recvSlot =
			rankingClient.sig_entriesReceived.connect(
					this, &RankingFindQueryImpl::onEntriesReceived);

	m_token = rankingClient.findEntry(m_playerId);
}

void RankingFindQueryImpl::onEntriesReceived(
		int p_token, const std::vector<PlacedRankingEntry> &p_entries)
{
	if (p_token == m_token) {

		if (p_entries.size() == 1) {
			m_found = true;
			m_entry = p_entries[0];
		} else {
			m_found = false;
		}

		m_parent->done();
	}
}

bool RankingFindQuery::isFound() const
{
	return m_impl->m_found;
}

const PlacedRankingEntry &RankingFindQuery::getEntry() const
{
	return m_impl->m_entry;
}

} // namespace
