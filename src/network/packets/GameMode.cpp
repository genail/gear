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

#include "GameMode.h"

#include "common/gassert.h"
#include "network/events.h"

namespace Net
{

class GameModeImpl
{
	public:

		GameMode *m_parent;

		TGameMode m_gameModeType;


		GameModeImpl(GameMode *p_parent);
		~GameModeImpl() {}

		bool isValid() const;
};

GameMode::GameMode() :
		m_impl(new GameModeImpl(this))
{
	// empty
}

GameModeImpl::GameModeImpl(GameMode *p_parent) :
		m_parent(p_parent)
{
	// empty
}

GameMode::~GameMode()
{
	// empty
}

CL_NetGameEvent GameMode::buildEvent() const
{
	CL_NetGameEvent event(EVENT_GAME_MODE);
	event.add_argument(m_impl->m_gameModeType);

	return event;
}

void GameMode::parseEvent(const CL_NetGameEvent &p_event)
{
	static const unsigned ARG_COUNT = 1;

	G_ASSERT(p_event.get_name() == EVENT_GAME_MODE);
	G_ASSERT(p_event.get_argument_count() == ARG_COUNT);

	m_impl->m_gameModeType = p_event.get_argument(0);

	if (!m_impl->isValid()) {
		throw CL_Exception("invalid game mode option");
	}
}

void GameMode::setGameModeType(TGameMode p_gameModeType)
{
	m_impl->m_gameModeType = p_gameModeType;
	G_ASSERT(m_impl->isValid() && "invalid argument");
}

TGameMode GameMode::getGameModeType() const
{
	return m_impl->m_gameModeType;
}

bool GameModeImpl::isValid() const
{
	return  m_gameModeType == GM_ARCADE || m_gameModeType == GM_TIME_TRAIL;
}

}
