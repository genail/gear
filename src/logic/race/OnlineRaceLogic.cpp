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

#include "Car.h"
#include "common/Game.h"
#include "network/packets/GameState.h"
#include "network/packets/CarState.h"

namespace Race {

const unsigned RACE_START_DELAY = 3000;

OnlineRaceLogic::OnlineRaceLogic(const CL_String &p_host, int p_port) :
	m_initialized(false),
	m_host(p_host),
	m_port(p_port),
	m_voteRunning(false)
{
	assert(p_port > 0 && p_port <= 0xFFFF);

	m_client.setServerAddr(m_host);
	m_client.setServerPort(m_port);

	// connect signal and slots from player's car
	Game &game = Game::getInstance();
	m_localPlayer = &game.getPlayer();
	Car &car = m_localPlayer->getCar();

	m_slots.connect(car.sig_inputChanged(), this, &OnlineRaceLogic::onInputChange);

	// connect signals and slots from client
	m_slots.connect(m_client.sig_connected(),         this, &OnlineRaceLogic::onConnected);
	m_slots.connect(m_client.sig_disconnected(),      this, &OnlineRaceLogic::onDisconnected);
	m_slots.connect(m_client.sig_goodbyeReceived(),   this, &OnlineRaceLogic::onGoodbye);
	m_slots.connect(m_client.sig_playerJoined(),      this, &OnlineRaceLogic::onPlayerJoined);
	m_slots.connect(m_client.sig_playerLeaved(),      this, &OnlineRaceLogic::onPlayerLeaved);
	m_slots.connect(m_client.sig_gameStateReceived(), this, &OnlineRaceLogic::onGameState);
	m_slots.connect(m_client.sig_carStateReceived(),  this, &OnlineRaceLogic::onCarState);
	m_slots.connect(m_client.sig_raceStartReceived(), this, &OnlineRaceLogic::onRaceStart);
	m_slots.connect(m_client.sig_voteStarted(),       this, &OnlineRaceLogic::onVoteStarted);
	m_slots.connect(m_client.sig_voteEnded(),         this, &OnlineRaceLogic::onVoteEnded);
	m_slots.connect(m_client.sig_voteTickReceived(),  this, &OnlineRaceLogic::onVoteTick);
}

OnlineRaceLogic::~OnlineRaceLogic()
{
	destroy();
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

		// remove players
		TPlayerMapPair pair;
		foreach (pair, m_playerMap) {
			Player *player = pair.second;

			// remove car from level
			m_level.removeCar(&player->getCar());

			// delete player if belongs to me
			if (player != &Game::getInstance().getPlayer()) {
				delete player;
			}
		}

		m_level.destroy();
	}
}

void OnlineRaceLogic::update(unsigned p_timeElapsed)
{
	RaceLogic::update(p_timeElapsed);

	// make sure that car is not locked when race is started
	Race::Car &car = m_localPlayer->getCar();
	if (isRaceStarted() && car.isLocked()) {
		car.setLocked(false);

		display(_("*** START! ***"));
	}

}

void OnlineRaceLogic::onConnected()
{
}

void OnlineRaceLogic::onDisconnected()
{
}

void OnlineRaceLogic::onGoodbye(GoodbyeReason p_reason, const CL_String &p_message)
{
	display(cl_format(_("Disconnected from server. Reason: %1"), p_message));
}

void OnlineRaceLogic::onPlayerJoined(const CL_String &p_name)
{
	// check player existence
	TPlayerMap::iterator itor = m_playerMap.find(p_name);

	if (itor == m_playerMap.end()) {
		// create new player
		Player *player = new Player(p_name);
		m_playerMap[p_name] = player;

		// add his car to level
		m_level.addCar(&player->getCar());

		display(cl_format(_("Player %1 joined"), p_name));
	} else {
		cl_log_event(LOG_ERROR, "Player named '%1' already in list", p_name);
	}

}

void OnlineRaceLogic::onPlayerLeaved(const CL_String &p_name)
{
	// get the player
	TPlayerMap::iterator itor = m_playerMap.find(p_name);

	if (itor != m_playerMap.end()) {
		Player *player = itor->second;

		// remove car from level
		m_level.removeCar(&player->getCar());

		// remove player
		delete player;

		display(cl_format("Player %1 leaved", p_name));
	} else {
		cl_log_event(LOG_ERROR, "No player named '%1' in list", p_name);
	}
}

void OnlineRaceLogic::onGameState(const Net::GameState &p_gameState)
{
	// load level
	const CL_String &levelName = p_gameState.getLevel();
	m_level.initialize(levelName);

	// add rest of players
	const unsigned playerCount = p_gameState.getPlayerCount();

	Player *player;
	Car *car;

	for (unsigned i = 0; i < playerCount; ++i) {
		const CL_String &playerName = p_gameState.getPlayerName(i);

		if (playerName == m_localPlayer->getName()) {
			// this is local player, so it exists now
			player = m_localPlayer;
		} else {
			// this is remote player
			player = new Player(playerName);
		}

		// put player to player list
		m_playerMap[playerName] = player;

		// prepare car and put it to level
		car = &player->getCar();
		car->applyCarState(p_gameState.getCarState(i));

		m_level.addCar(car);
	}


}

void OnlineRaceLogic::onCarState(const Net::CarState &p_carState)
{
	const CL_String &playerName = p_carState.getName();
	TPlayerMap::iterator itor = m_playerMap.find(playerName);

	if (itor != m_playerMap.end()) {
		itor->second->getCar().applyCarState(p_carState);
	} else {
		cl_log_event(LOG_ERROR, "Player %1 do not exists", playerName);
	}
}

void OnlineRaceLogic::onRaceStart(const CL_Pointf &p_carPosition, const CL_Angle &p_carRotation)
{
	cl_log_event(LOG_RACE, "Race is starting");

	Car &car = Game::getInstance().getPlayer().getCar();

	car.setPosition(p_carPosition);
	car.setRotation(p_carRotation.to_degrees() - 90); // FIXME: Remove -90 when #16 is resolved
	car.setLap(1);

	car.setLocked(true);

	// send current state
	const Net::CarState carState = car.prepareCarState();
	m_client.sendCarState(carState);


	startRace(3, CL_System::get_time() + RACE_START_DELAY);
}

void OnlineRaceLogic::onInputChange(const Car &p_car)
{
	if (!p_car.isLocked()) { // ignore when should be locked
		const Net::CarState carState = p_car.prepareCarState();
		m_client.sendCarState(carState);
	}
}

void OnlineRaceLogic::callAVote(VoteType p_type, const CL_String &p_subject)
{
	m_client.callAVote(p_type, p_subject);
}

const CL_String &OnlineRaceLogic::getVoteMessage() const
{
	return m_voteMessage;
}

bool OnlineRaceLogic::isVoteRunning() const
{
	return m_voteRunning;
}

int OnlineRaceLogic::getVoteNoCount() const
{
	return m_voteNoCount;
}

int OnlineRaceLogic::getVoteYesCount() const
{
	return m_voteYesCount;
}

unsigned OnlineRaceLogic::getVoteTimeout() const
{
	return m_voteTimeout;
}

void OnlineRaceLogic::onVoteStarted(VoteType p_voteType, const CL_String& p_subject, unsigned p_timeLimitSec)
{
	switch (p_voteType) {
		case VOTE_RESTART_RACE:
			m_voteMessage = _("Restart Level?");
			break;

		default:
			assert(0 && "unknown VoteType");
	}

	m_voteRunning = true;
	m_voteYesCount = 0;
	m_voteNoCount = 0;
	m_voteTimeout = CL_System::get_time() + (p_timeLimitSec * 1000);

	display(_("New vote called"));

}

void OnlineRaceLogic::onVoteEnded(VoteResult p_voteResult)
{
	switch (p_voteResult) {
		case VOTE_PASSED:
			display("Vote passed");
			break;
		case VOTE_FAILED:
			display("Vote failed");
			break;
		default:
			assert(0 && "unknown VoteResult");
	}

	m_voteRunning = false;
}

void OnlineRaceLogic::voteNo()
{
	m_client.voteNo();
}

void OnlineRaceLogic::voteYes()
{
	m_client.voteYes();
}

void OnlineRaceLogic::onVoteTick(VoteOption p_voteOption)
{
	switch (p_voteOption) {
		case VOTE_YES:
			++m_voteYesCount;
			break;
		case VOTE_NO:
			++m_voteNoCount;
			break;
		default:
			assert(0 && "unknown VoteOption");
	}
}

} // namespace
