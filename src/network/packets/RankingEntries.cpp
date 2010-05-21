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

#include "RankingEntries.h"

#include "common.h"
#include "common/gassert.h"
#include "network/events.h"

namespace Net
{

RankingEntries::RankingEntries() :
		m_token(-1)
{
	// empty
}

RankingEntries::~RankingEntries()
{
	// empty
}

CL_NetGameEvent RankingEntries::buildEvent() const
{
	CL_NetGameEvent event(EVENT_RANKING_ENTRIES);
	event.add_argument(m_token);
	event.add_argument(getEntryCount());

	foreach(const PlacedRankingEntry &entry, m_rankingEntries) {
		event.add_argument(entry.pid);
		event.add_argument(entry.name);
		event.add_argument(entry.timeMs);
		event.add_argument(entry.place);
	}

	return event;
}

void RankingEntries::parseEvent(const CL_NetGameEvent &p_event)
{
	G_ASSERT(p_event.get_name() == EVENT_RANKING_ENTRIES);
	G_ASSERT(getEntryCount() == 0);

	m_token = p_event.get_argument(0);
	const int entryCount = p_event.get_argument(1);

	int index = 2;
	PlacedRankingEntry entry;

	for (int i = 0; i < entryCount; ++i) {
		entry.pid = p_event.get_argument(index++);
		entry.name = p_event.get_argument(index++);
		entry.timeMs = p_event.get_argument(index++);
		entry.place = p_event.get_argument(index++);

		addEntry(entry);
	}
}

void RankingEntries::addEntry(const PlacedRankingEntry &p_entry)
{
	m_rankingEntries.push_back(p_entry);
}

int RankingEntries::getEntryCount() const
{
	return m_rankingEntries.size();
}

const PlacedRankingEntry &RankingEntries::getEntry(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < (signed) m_rankingEntries.size());
	return m_rankingEntries[p_index];
}

void RankingEntries::setToken(int p_token)
{
	m_token = p_token;
}

int RankingEntries::getToken() const
{
	return m_token;
}

}
