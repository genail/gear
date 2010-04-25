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

#include "BasicGameClient.h"

#include "common/Game.h"
#include "common/RemotePlayer.h"
#include "logic/race/GameLogic.h"
#include "logic/race/level/Level.h"
#include "network/client/Client.h"
#include "network/packets/GameState.h"
#include "network/packets/CarState.h"

namespace Race
{

class BasicGameClientImpl
{
	public:

		typedef std::map<CL_String, CL_SharedPtr<RemotePlayer> > TNamePlayerMap;
		typedef TNamePlayerMap::iterator TNamePlayerMapItor;

		BasicGameClient *m_parent;
		GameLogic *const m_gameLogic;

		CL_SlotContainer m_slots;

		TNamePlayerMap m_namePlayerMap;


		BasicGameClientImpl(BasicGameClient *p_parent, GameLogic *p_gameLogic);
		~BasicGameClientImpl();

		void onPlayerJoined(const CL_String &p_name);
		bool playerExists(const CL_String &p_name);
		void createNewPlayer(const CL_String &p_name);
		void addPlayerCarToLevel(RemotePlayer &p_remotePlayer);

		void onPlayerLeaved(const CL_String &p_name);
		void removePlayer(const CL_String &p_name);
		void removePlayerCarFromLevel(RemotePlayer &p_remotePlayer);

		void applyGameState(const Net::GameState &p_gameState);
		void applyCarState(RemotePlayer &p_remotePlayer, const Net::CarState &p_carState);
};

BasicGameClient::BasicGameClient(GameLogic *p_gameLogic) :
		m_impl(new BasicGameClientImpl(this, p_gameLogic))
{
	// empty
}

BasicGameClientImpl::BasicGameClientImpl(BasicGameClient *p_parent, GameLogic *p_gameLogic) :
		m_parent(p_parent),
		m_gameLogic(p_gameLogic)
{
	Game &game = Game::getInstance();
	Net::Client &client = game.getNetworkConnection();

	m_slots.connect(
			client.sig_playerJoined(),
			this, &BasicGameClientImpl::onPlayerJoined);

	m_slots.connect(
			client.sig_playerLeaved(),
			this, &BasicGameClientImpl::onPlayerLeaved);
}

BasicGameClient::~BasicGameClient()
{
	// empty
}

BasicGameClientImpl::~BasicGameClientImpl()
{
	// empty
}

void BasicGameClientImpl::onPlayerJoined(const CL_String &p_name)
{
	if (!playerExists(p_name)) {
		createNewPlayer(p_name);
	} else {
		cl_log_event(LOG_ERROR, "Player named '%1' already in game", p_name);
	}
}

bool BasicGameClientImpl::playerExists(const CL_String &p_name)
{
	return m_namePlayerMap.find(p_name) != m_namePlayerMap.end();
}

void BasicGameClientImpl::createNewPlayer(const CL_String &p_name)
{
	CL_SharedPtr<RemotePlayer> remotePlayer(new RemotePlayer(p_name));
	m_namePlayerMap[p_name] = remotePlayer;

	addPlayerCarToLevel(*remotePlayer.get());
}

void BasicGameClientImpl::addPlayerCarToLevel(RemotePlayer &p_remotePlayer)
{
	Race::Level &level = m_gameLogic->getLevel();
	level.addCar(&p_remotePlayer.getCar());
}

void BasicGameClientImpl::onPlayerLeaved(const CL_String &p_name)
{
	if (playerExists(p_name)) {
		removePlayer(p_name);
	} else {
		cl_log_event(LOG_ERROR, "Player named '%1' not present in game", p_name);
	}
}

void BasicGameClientImpl::removePlayer(const CL_String &p_name)
{
	CL_SharedPtr<RemotePlayer> remotePlayer = m_namePlayerMap[p_name];
	m_namePlayerMap.erase(p_name);

	removePlayerCarFromLevel(*remotePlayer.get());
}

void BasicGameClientImpl::removePlayerCarFromLevel(RemotePlayer &p_remotePlayer)
{
	Race::Level &level = m_gameLogic->getLevel();
	level.removeCar(&p_remotePlayer.getCar());
}

void BasicGameClient::applyGameState(const Net::GameState &p_gameState)
{
	m_impl->applyGameState(p_gameState);
}

void BasicGameClientImpl::applyGameState(const Net::GameState &p_gameState)
{
	const int playerCount = p_gameState.getPlayerCount();
	for (int i = 0; i < playerCount; ++i) {
		const CL_String &playerName = p_gameState.getPlayerName(i);
		CL_SharedPtr<RemotePlayer> remotePlayer(new RemotePlayer(playerName));
		m_namePlayerMap[playerName] = remotePlayer;

		const Net::CarState &carState = p_gameState.getCarState(i);
		applyCarState(*remotePlayer.get(), carState);
	}
}

void BasicGameClientImpl::applyCarState(
		RemotePlayer &p_remotePlayer, const Net::CarState &p_carState)
{
	Race::Car &car = p_remotePlayer.getCar();
	CL_NetGameEvent serializedCarData = p_carState.getSerializedData();
	car.deserialize(serializedCarData);
}

}
