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

#include "RankingAdvance.h"

#include "common/gassert.h"
#include "network/events.h"

const unsigned ARGS_COUNT = 3;

namespace Net
{

RankingAdvance::RankingAdvance()
{
	// empty
}

RankingAdvance::~RankingAdvance()
{
	// empty
}

CL_NetGameEvent RankingAdvance::buildEvent() const
{
	CL_NetGameEvent event(EVENT_RANKING_ADVANCE);
	event.add_argument(m_rankingEntry.uid);
	event.add_argument(m_rankingEntry.name);
	event.add_argument(m_rankingEntry.timeMs);

	return event;
}

void RankingAdvance::parseEvent(const CL_NetGameEvent &p_event)
{
	G_ASSERT(p_event.get_name() == EVENT_RANKING_ADVANCE);
	G_ASSERT(p_event.get_argument_count() == ARGS_COUNT);

	m_rankingEntry.uid = p_event.get_argument(0);
	m_rankingEntry.name = p_event.get_argument(1);
	m_rankingEntry.timeMs = p_event.get_argument(2);
}

void RankingAdvance::setRankingEntry(const RankingEntry &p_entry)
{
	m_rankingEntry = p_entry;
}

const RankingEntry &RankingAdvance::getRankingEntry() const
{
	return m_rankingEntry;
}

}
