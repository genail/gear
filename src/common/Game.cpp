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

#include "Game.h"

#include "common.h"
#include "gfx/scenes/SceneContainer.h"

Game::Game() :
	m_networkConnection(),
	m_player(),
	m_sceneContainer(NULL)
{

}

Game::~Game()
{
	m_slots.connect(m_networkConnection.sig_gameStateReceived(), this, &Game::onGameState);
	m_slots.connect(m_networkConnection.sig_playerJoined(), this, &Game::onPlayerJoined);
	m_slots.connect(m_networkConnection.sig_playerLeaved(), this, &Game::onPlayerLeaved);
	m_slots.connect(m_networkConnection.sig_disconnected(), this, &Game::onDisconnected);
}

Game &Game::getInstance()
{
	static Game game;
	return game;
}

Race::Level &Game::getLevel()
{
	return m_level;
}

Net::Client &Game::getNetworkConnection()
{
	return m_networkConnection;
}

Player &Game::getPlayer() {
	return m_player;
}

SceneContainer &Game::getSceneContainer() {
	assert(m_sceneContainer != NULL && "scene container must be set by Application");
	return *m_sceneContainer;
}

unsigned Game::getNetworkPlayerCount() const
{
	return m_playersList.size();
}

const Player& Game::getNetworkPlayer(unsigned p_index) const
{
	return *m_playersList[p_index];
}
