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

#include "RankingFind.h"

#include "common/gassert.h"
#include "common/Token.h"
#include "network/events.h"


const unsigned ARGS_COUNT = 2;


namespace Net
{

RankingFind::RankingFind() :
		m_token(Token::next())
{
	// empty
}

RankingFind::~RankingFind()
{
	// empty
}

CL_NetGameEvent RankingFind::buildEvent() const
{
	CL_NetGameEvent event(EVENT_RANKING_FIND);
	event.add_argument(m_token);
	event.add_argument(m_playerId);

	return event;
}

void RankingFind::parseEvent(const CL_NetGameEvent &p_event)
{
	G_ASSERT(p_event.get_name() == EVENT_RANKING_FIND);
	G_ASSERT(p_event.get_argument_count() == ARGS_COUNT);

	m_token = p_event.get_argument(0);
	m_playerId = p_event.get_argument(1);
}

void RankingFind::setPlayerId(const CL_String &p_str)
{
	m_playerId = p_str;
}

const CL_String &RankingFind::getPlayerId() const
{
	return m_playerId;
}

int RankingFind::getToken() const
{
	return m_token;
}

}
