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

#include "GameState.h"

#include <assert.h>

#include "network/events.h"

namespace Net {

GameState::GameState()
{
	// nothing
}

GameState::~GameState()
{
	// nothing
}

CL_NetGameEvent GameState::buildEvent() const
{
	CL_NetGameEvent event(EVENT_GAME_STATE);

	event.add_argument(m_level);

	const size_t playerCount = m_names.size();
	event.add_argument(playerCount);

	for (size_t i = 0; i < playerCount; ++i) {
		event.add_argument(m_names[i]);

		// inline car state event arguments
		const CarState &carState = m_carStates[i];
		const CL_NetGameEvent carStateEvent = carState.buildEvent();

		const size_t argumentCount = carStateEvent.get_argument_count();
		event.add_argument(argumentCount);

		for (size_t j = 0; j < argumentCount; ++j) {
			event.add_argument(carStateEvent.get_argument(j));
		}
	}

	return event;
}

void GameState::parseEvent(const CL_NetGameEvent &p_event)
{
	assert(p_event.get_name() == EVENT_GAME_STATE);

	unsigned arg = 0;
	m_level = p_event.get_argument(arg++);

	const size_t playerCount = p_event.get_argument(arg++);

	m_names.clear();
	m_carStates.clear();

	for (size_t i = 0; i < playerCount; ++i) {
		m_names.push_back(p_event.get_argument(arg++));

		// read inline car state event
		const size_t argumentCount = p_event.get_argument(arg++);
		CL_NetGameEvent carStateEvent(EVENT_CAR_STATE);

		for (size_t j = 0; j < argumentCount; ++j) {
			carStateEvent.add_argument(p_event.get_argument(arg++));
		}

		CarState carState;
		carState.parseEvent(carStateEvent);

		m_carStates.push_back(carState);
	}
}

void GameState::addPlayer(const CL_String &p_name, const CarState &p_carState)
{
	m_names.push_back(p_name);
	m_carStates.push_back(p_carState);
}

} // namespace
