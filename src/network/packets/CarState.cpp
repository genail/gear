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

#include "CarState.h"

#include "common/gassert.h"
#include "network/events.h"

namespace Net {

CarState::CarState() :
	m_iterId(-1),
	m_afterCollision(false),
	m_serialData("")
{
	// empty
}

CarState::~CarState()
{
	// empty
}

CL_NetGameEvent CarState::buildEvent() const
{
	G_ASSERT(m_serialData.get_argument_count() != 0);
	// player's name is allowed to be empty

	CL_NetGameEvent event(EVENT_CAR_STATE);

	event.add_argument(m_iterId);
	event.add_argument(m_name);
	event.add_argument(CL_NetGameEventValue(m_afterCollision));

	const int argCount = static_cast<signed>(m_serialData.get_argument_count());

	for (int i = 0; i < argCount; ++i) {
		event.add_argument(m_serialData.get_argument(i));
	}

	return event;
}

void CarState::parseEvent(const CL_NetGameEvent &p_event)
{
	G_ASSERT(p_event.get_name() == EVENT_CAR_STATE);

	int idx = 0;

	m_iterId = p_event.get_argument(idx++);
	m_name = p_event.get_argument(idx++);
	m_afterCollision = p_event.get_argument(idx++);
	m_serialData = CL_NetGameEvent("");

	const int argCount = static_cast<signed>(p_event.get_argument_count());

	for (; idx < argCount; ++idx) {
		m_serialData.add_argument(p_event.get_argument(idx));
	}
}

int32_t CarState::getIterationId() const
{
	return m_iterId;
}

const CL_String &CarState::getName() const
{
	return m_name;
}

CL_NetGameEvent CarState::getSerializedData() const
{
	return m_serialData;
}

void CarState::setIterationId(int32_t p_iterId)
{
	m_iterId = p_iterId;
}

void CarState::setName(const CL_String &p_name)
{
	m_name = p_name;
}

void CarState::setSerializedData(const CL_NetGameEvent &p_data)
{
	m_serialData = p_data;
}

void CarState::setAfterCollision(bool p_afterCollision)
{
	m_afterCollision = p_afterCollision;
}

bool CarState::isAfterCollision() const
{
	return m_afterCollision;
}

} // namespace
