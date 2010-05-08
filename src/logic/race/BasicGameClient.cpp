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

#include "common/loglevels.h"
#include "common/Game.h"
#include "common/RemotePlayer.h"
#include "logic/VoteSystem.h"
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
		Net::Client &m_netClient;

		TNamePlayerMap m_namePlayerMap;
		CarInputState m_lastLocalCarInputState;

		CL_SlotContainer m_slots;


		BasicGameClientImpl(BasicGameClient *p_parent, GameLogic *p_gameLogic);
		~BasicGameClientImpl();

		void update();
		void updatePlayerCarRemoteState();
		void sendCarState(const Race::Car &p_car);

		void onPlayerJoined(const CL_String &p_name);
		bool playerExists(const CL_String &p_name);
		RemotePlayer& createNewPlayer(const CL_String &p_name);
		void addPlayerCarToLevel(RemotePlayer &p_remotePlayer);

		void onPlayerLeaved(const CL_String &p_name);
		void removePlayer(const CL_String &p_name);
		void removePlayerCarFromLevel(RemotePlayer &p_remotePlayer);
		
		void onCarStateReceived(const Net::CarState &p_carState);

		void applyGameState(const Net::GameState &p_gameState);
		void positionLocalPlayerCar(const Net::CarState &p_carState);
		void applyCarState(Race::Car &p_car, const Net::CarState &p_carState);
		
		void onRaceStartReceived(const Net::RaceStart &p_raceStart);

		void onVoteStated(
				VoteType p_voteType,
				const CL_String& p_subject,
				unsigned p_timeLimit);
		void onVoteTickReceived(VoteOption p_option);
		void onVoteEnded(VoteResult p_voteResult);
};

BasicGameClient::BasicGameClient(GameLogic *p_gameLogic) :
		m_impl(new BasicGameClientImpl(this, p_gameLogic))
{
	// empty
}

BasicGameClientImpl::BasicGameClientImpl(BasicGameClient *p_parent, GameLogic *p_gameLogic) :
		m_parent(p_parent),
		m_gameLogic(p_gameLogic),
		m_netClient(Game::getInstance().getNetworkConnection())
{
	Game &game = Game::getInstance();

	m_slots.connect(
			m_netClient.sig_playerJoined(),
			this, &BasicGameClientImpl::onPlayerJoined);

	m_slots.connect(
			m_netClient.sig_playerLeaved(),
			this, &BasicGameClientImpl::onPlayerLeaved);
	
	m_slots.connect(
			m_netClient.sig_carStateReceived(),
			this, &BasicGameClientImpl::onCarStateReceived);
			
	m_slots.connect(
			m_netClient.sig_voteStarted(),
			this, &BasicGameClientImpl::onVoteStated);
			
	m_slots.connect(
			m_netClient.sig_voteTickReceived(),
			this, &BasicGameClientImpl::onVoteTickReceived);
			
	m_slots.connect(
			m_netClient.sig_voteEnded(),
			this, &BasicGameClientImpl::onVoteEnded);
}

BasicGameClient::~BasicGameClient()
{
	// empty
}

BasicGameClientImpl::~BasicGameClientImpl()
{
	// empty
}

void BasicGameClient::update()
{
	m_impl->update();
}

void BasicGameClientImpl::update()
{
	updatePlayerCarRemoteState();
}

void BasicGameClientImpl::updatePlayerCarRemoteState()
{
	Game &game = Game::getInstance();
	Player &localPlayer = game.getPlayer();
	Race::Car &localCar = localPlayer.getCar();

	const CarInputState &currentCarState = localCar.getInputState();
	if (m_lastLocalCarInputState != currentCarState) {
		sendCarState(localCar);
		m_lastLocalCarInputState = currentCarState;
	}
}

void BasicGameClientImpl::sendCarState(const Race::Car &p_car)
{
	CL_NetGameEvent gameStateEvent("");
	p_car.serialize(&gameStateEvent);

	const Player &carOwner = p_car.getOwnerPlayer();
	const CL_String &carOwnerName = carOwner.getName();

	Net::CarState carStatePacket;
	carStatePacket.setSerializedData(gameStateEvent);
	carStatePacket.setIterationId(p_car.getIterationId());
	carStatePacket.setName(carOwnerName);

	m_netClient.sendCarState(carStatePacket);
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

RemotePlayer& BasicGameClientImpl::createNewPlayer(const CL_String &p_name)
{
	CL_SharedPtr<RemotePlayer> remotePlayer(new RemotePlayer(p_name));
	m_namePlayerMap[p_name] = remotePlayer;

	addPlayerCarToLevel(*remotePlayer);
	return *remotePlayer;
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

	removePlayerCarFromLevel(*remotePlayer);
}

void BasicGameClientImpl::removePlayerCarFromLevel(RemotePlayer &p_remotePlayer)
{
	Race::Level &level = m_gameLogic->getLevel();
	level.removeCar(&p_remotePlayer.getCar());
}

void BasicGameClientImpl::onCarStateReceived(const Net::CarState &p_carState)
{
	const CL_String &playerName = p_carState.getName();
	if (playerExists(playerName)) {
		RemotePlayer &remotePlayer = *m_namePlayerMap[playerName];
		Race::Car &car = remotePlayer.getCar();
		
		applyCarState(car, p_carState);
	}
}

void BasicGameClient::applyGameState(const Net::GameState &p_gameState)
{
	m_impl->applyGameState(p_gameState);
}

void BasicGameClientImpl::applyGameState(const Net::GameState &p_gameState)
{
	Game &game = Game::getInstance();
	Player &localPlayer = game.getPlayer();
	const CL_String &localPlayerName = localPlayer.getName();
	
	
	const int playerCount = p_gameState.getPlayerCount();
	for (int i = 0; i < playerCount; ++i) {
		const CL_String &playerName = p_gameState.getPlayerName(i);
		
		const Net::CarState &carState = p_gameState.getCarState(i);
		
		if (playerName == localPlayerName) {
			positionLocalPlayerCar(carState);
		} else {
			RemotePlayer &remotePlayer = createNewPlayer(playerName);
			Race::Car &remotePlayerCar = remotePlayer.getCar();
			applyCarState(remotePlayerCar, carState);
		}
	}
}

void BasicGameClientImpl::positionLocalPlayerCar(const Net::CarState &p_carState)
{
	Game &game = Game::getInstance();
	Player &localPlayer = game.getPlayer();
	Race::Car &localPlayerCar = localPlayer.getCar();
	
	applyCarState(localPlayerCar, p_carState);
}

void BasicGameClientImpl::applyCarState(
Race::Car &p_car, const Net::CarState &p_carState)
{
	CL_NetGameEvent serializedCarData = p_carState.getSerializedData();
	p_car.deserialize(serializedCarData);
}

void BasicGameClientImpl::onRaceStartReceived(const Net::RaceStart &p_raceStart)
{

}

void BasicGameClient::callAVote(VoteType p_voteType, const CL_String &p_subject)
{
	m_impl->m_netClient.callAVote(p_voteType, p_subject);
}

void BasicGameClientImpl::onVoteStated(
		VoteType p_voteType,
		const CL_String& p_subject,
		unsigned p_timeLimit)
{
	VoteSystem &voteSystem = m_gameLogic->getVoteSystem();
	const Race::Level &level = m_gameLogic->getLevel();
	const int carCount = level.getCarCount();

	voteSystem.start(p_voteType, carCount, p_timeLimit * 1000);
}

void BasicGameClientImpl::onVoteTickReceived(VoteOption p_option)
{
	VoteSystem &voteSystem = m_gameLogic->getVoteSystem();
	voteSystem.addVote(p_option);
}

void BasicGameClientImpl::onVoteEnded(VoteResult p_voteResult)
{
	VoteSystem &voteSystem = m_gameLogic->getVoteSystem();
	if (!voteSystem.isFinished()) {
		cl_log_event(LOG_WARN, "Local vote system is acting diffrently to remote vote system. Something is wrong!");
	}
}

void BasicGameClient::voteYes()
{
	m_impl->m_netClient.voteYes();
}

void BasicGameClient::voteNo()
{
	m_impl->m_netClient.voteNo();
}

}
