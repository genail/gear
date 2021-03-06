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

#include "GameLogicArcadeOnline.h"

#include "logic/race/BasicGameClient.h"

namespace Race
{

class GameLogicArcadeOnlineImpl
{
	public:

		GameLogicArcadeOnline *m_parent;
		BasicGameClient m_basicClient;


		GameLogicArcadeOnlineImpl(GameLogicArcadeOnline *p_parent);
		~GameLogicArcadeOnlineImpl();
		
		void update(unsigned p_timeElapsedMs);
};

GameLogicArcadeOnline::GameLogicArcadeOnline() :
		m_impl(new GameLogicArcadeOnlineImpl(this))
{
	// empty
}

GameLogicArcadeOnlineImpl::GameLogicArcadeOnlineImpl(GameLogicArcadeOnline *p_parent) :
		m_parent(p_parent),
		m_basicClient(p_parent)
{
	// empty
}

GameLogicArcadeOnline::~GameLogicArcadeOnline()
{
	// empty
}

GameLogicArcadeOnlineImpl::~GameLogicArcadeOnlineImpl()
{
	// empty
}

void GameLogicArcadeOnline::applyGameState(const Net::GameState &p_gameState)
{
	m_impl->m_basicClient.applyGameState(p_gameState);
}

void GameLogicArcadeOnline::update(unsigned p_timeElapsedMs)
{
	GameLogicArcade::update(p_timeElapsedMs);
	m_impl->update(p_timeElapsedMs);
}

void GameLogicArcadeOnlineImpl::update(unsigned p_timeElapsedMs)
{
	m_basicClient.update();
}

void GameLogicArcadeOnline::callAVote(
		VoteType p_voteType, const CL_String& p_subject)
{
	m_impl->m_basicClient.callAVote(p_voteType, p_subject);
}

void GameLogicArcadeOnline::voteYes()
{
	m_impl->m_basicClient.voteYes();
}

void GameLogicArcadeOnline::voteNo()
{
	m_impl->m_basicClient.voteNo();
}

}
