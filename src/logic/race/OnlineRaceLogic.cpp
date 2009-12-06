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

#include "OnlineRaceLogic.h"

#include <assert.h>

#include "common/Game.h"
#include "network/packets/GameState.h"

namespace Race {

OnlineRaceLogic::OnlineRaceLogic(const CL_String &p_host, int p_port) :
	m_initialized(false),
	m_host(p_host),
	m_port(p_port)
{
	assert(p_port > 0 && p_port <= 0xFFFF);

	m_client.setServerAddr(m_host);
	m_client.setServerPort(m_port);

	// connect signals and slots
	m_slots.connect(m_client.sig_connected(), this, &OnlineRaceLogic::onConnected);
	m_slots.connect(m_client.sig_disconnected(), this, &OnlineRaceLogic::onDisconnected);
	m_slots.connect(m_client.sig_gameStateReceived(), this, &OnlineRaceLogic::onGameState);
}

OnlineRaceLogic::~OnlineRaceLogic()
{
}

void OnlineRaceLogic::initialize()
{
	if (!m_initialized) {

		// connect to server
		m_client.connect();

		m_initialized = true;
	}
}

void OnlineRaceLogic::destroy()
{
	if (m_initialized) {
		m_client.disconnect();
		m_level.destroy();
	}
}

void OnlineRaceLogic::onConnected()
{
}

void OnlineRaceLogic::onDisconnected()
{
}

void OnlineRaceLogic::onGameState(const Net::GameState &p_gameState)
{
	// load level
	const CL_String &levelName = p_gameState.getLevel();
	m_level.initialize(levelName);

	// first car is player car
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();

	m_level.addCar(&player.getCar());

	// add rest of players
	const unsigned playerCount = p_gameState.getPlayerCount();

	for (unsigned i = 0; i < playerCount; ++i) {
		const CL_String &playerName = p_gameState.getPlayerName(i);
		m_playerMap[playerName] = new Player(playerName);
	}
}

} // namespace
