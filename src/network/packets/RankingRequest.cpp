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

#include "RankingRequest.h"

#include "common/gassert.h"
#include "network/events.h"

namespace Net
{

class RankingRequestImpl
{
	public:

		RankingRequest *m_parent;

		int m_placeFrom;

		int m_placeTo;


		RankingRequestImpl(RankingRequest *p_parent);
		~RankingRequestImpl() {}


		bool isValid() const;
};


RankingRequestImpl::RankingRequestImpl(RankingRequest *p_parent) :
		m_parent(p_parent),
		m_placeFrom(0),
		m_placeTo(0)
{
	// empty
}

RankingRequest::RankingRequest() :
		m_impl(new RankingRequestImpl(this))
{
	// empty
}

RankingRequest::~RankingRequest()
{
	// empty
}

CL_NetGameEvent RankingRequest::buildEvent() const
{
	CL_NetGameEvent event(EVENT_RANKING_REQUEST);
	event.add_argument(m_impl->m_placeFrom);
	event.add_argument(m_impl->m_placeTo);

	return event;
}

void RankingRequest::parseEvent(const CL_NetGameEvent &p_event)
{
	static const unsigned ARG_COUNT = 2;
	G_ASSERT(p_event.get_name() == EVENT_RANKING_REQUEST);
	G_ASSERT(p_event.get_argument_count() == ARG_COUNT);

	m_impl->m_placeFrom = p_event.get_argument(0);
	m_impl->m_placeTo = p_event.get_argument(1);

	if (!m_impl->isValid()) {
		throw CL_Exception("invalid from-to values");
	}
}

int RankingRequest::getPlaceFrom() const
{
	return m_impl->m_placeFrom;
}

int RankingRequest::getPlaceTo() const
{
	return m_impl->m_placeTo;
}

void RankingRequest::setPlaceFrom(int p_placeFrom)
{
	G_ASSERT(p_placeFrom > 0);
	m_impl->m_placeFrom = p_placeFrom;
}

void RankingRequest::setPlaceTo(int p_placeTo)
{
	G_ASSERT(p_placeTo > 0);
	m_impl->m_placeTo = p_placeTo;
}

bool RankingRequestImpl::isValid() const
{
	return m_placeFrom > 0 && m_placeFrom <= m_placeTo;
}

}
