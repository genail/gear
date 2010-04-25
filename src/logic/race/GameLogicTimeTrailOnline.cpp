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

#include "GameLogicTimeTrailOnline.h"

#include "logic/race/BasicGameClient.h"
#include "network/packets/GameState.h"

namespace Race
{

class GameLogicTimeTrailOnlineImpl
{
	public:

		GameLogicTimeTrailOnline *m_parent;

		BasicGameClient m_basicClient;

		CL_SlotContainer m_slots;


		GameLogicTimeTrailOnlineImpl(GameLogicTimeTrailOnline *p_parent);
		~GameLogicTimeTrailOnlineImpl();

		void applyGameState(const Net::GameState &p_gameState);
};

GameLogicTimeTrailOnline::GameLogicTimeTrailOnline() :
		m_impl(new GameLogicTimeTrailOnlineImpl(this))
{
	// empty
}

GameLogicTimeTrailOnlineImpl::GameLogicTimeTrailOnlineImpl(GameLogicTimeTrailOnline *p_parent) :
		m_parent(p_parent),
		m_basicClient(p_parent)
{
	// empty
}

GameLogicTimeTrailOnline::~GameLogicTimeTrailOnline()
{
	// empty
}

GameLogicTimeTrailOnlineImpl::~GameLogicTimeTrailOnlineImpl()
{
	// empty
}

void GameLogicTimeTrailOnline::applyGameState(const Net::GameState &p_gameState)
{
	m_impl->applyGameState(p_gameState);
}

void GameLogicTimeTrailOnlineImpl::applyGameState(const Net::GameState &p_gameState)
{
	m_basicClient.applyGameState(p_gameState);
}

}
