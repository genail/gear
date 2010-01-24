/*
 * Copyright (c) 2009, Piotr Korzuszek
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

#include "RaceStart.h"

#include <assert.h>

#include "network/events.h"

namespace Net {

RaceStart::RaceStart()
{
}

RaceStart::~RaceStart()
{
}

CL_NetGameEvent RaceStart::buildEvent() const
{
	CL_NetGameEvent event(EVENT_RACE_START);

	event.add_argument(m_carPosition.x);
	event.add_argument(m_carPosition.y);

	event.add_argument(m_carRotation.to_radians());

	return event;
}

void RaceStart::parseEvent(const CL_NetGameEvent &p_event)
{
	assert(p_event.get_name() == EVENT_RACE_START);

	int i = 0;

	m_carPosition.x = static_cast<float> (p_event.get_argument(i++));
	m_carPosition.y = static_cast<float> (p_event.get_argument(i++));

	m_carRotation.set_radians(static_cast<float> (p_event.get_argument(i++)));
}

const CL_Pointf &RaceStart::getCarPosition() const
{
	return m_carPosition;
}

const CL_Angle &RaceStart::getCarRotation() const
{
	return m_carRotation;
}

void RaceStart::setCarPosition(const CL_Pointf &p_position)
{
	m_carPosition = p_position;
}

void RaceStart::setCarRotation(const CL_Angle &p_rotation)
{
	m_carRotation = p_rotation;
}

}
