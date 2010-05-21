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

#include "RankingBrowseQuery.h"

#include "common/gassert.h"
#include "common/Game.h"

namespace Net
{

class RankingBrowseQueryImpl
{
	public:
		RankingBrowseQuery *m_parent;

		int m_token;

		int m_from, m_to;
		std::vector<PlacedRankingEntry> m_entries;

		CL_Slot m_receivedSlot;

		RankingBrowseQueryImpl(RankingBrowseQuery *p_parent);
		~RankingBrowseQueryImpl();

		void submit();

		void onEntriesReceived(int p_token, const std::vector<PlacedRankingEntry> &p_entries);
};

// ----------------------------------------------------------------------------

RankingBrowseQuery::RankingBrowseQuery() :
		m_impl(new RankingBrowseQueryImpl(this))
{
	// empty
}

RankingBrowseQueryImpl::RankingBrowseQueryImpl(RankingBrowseQuery *p_parent) :
		m_parent(p_parent),
		m_token(-1),
		m_from(0),
		m_to(0)
{
	// empty
}

RankingBrowseQuery::~RankingBrowseQuery()
{
	// empty
}

RankingBrowseQueryImpl::~RankingBrowseQueryImpl()
{
	// empty
}

// ----------------------------------------------------------------------------

void RankingBrowseQuery::setRange(int p_from, int p_to)
{
	m_impl->m_from = p_from;
	m_impl->m_to = p_to;
}

void RankingBrowseQuery::submit()
{
	m_impl->submit();
}

void RankingBrowseQueryImpl::submit()
{
	m_parent->start();

	Game &game = Game::getInstance();
	Net::Client &client = game.getNetworkConnection();
	Net::RankingClient &rankingClient = client.getRankingClient();

	m_receivedSlot =
			rankingClient.sig_entriesReceived.connect(
					this, &RankingBrowseQueryImpl::onEntriesReceived);


	m_token = rankingClient.requestEntries(m_from, m_to);

}

void RankingBrowseQueryImpl::onEntriesReceived(
		int p_token, const std::vector<PlacedRankingEntry> &p_entries)
{
	if (p_token == m_token) {
		m_entries = p_entries;
		m_parent->done();
	}
}

int RankingBrowseQuery::getEntriesCount() const
{
	return m_impl->m_entries.size();
}

const PlacedRankingEntry &RankingBrowseQuery::getEntry(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < static_cast<int>(m_impl->m_entries.size()));
	return m_impl->m_entries[p_index];
}

}
