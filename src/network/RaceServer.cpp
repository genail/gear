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

#include "network/RaceServer.h"

#include <assert.h>
#include <limits.h>

#include "common.h"
#include "network/events.h"
#include "network/Server.h"
#include "network/GameState.h"

RaceServer::RaceServer(Server* p_server) :
	m_initialized(false),
	m_lapsNum(INT_MAX),
	m_server(p_server),
	m_slotsConnected(false)
{
}

RaceServer::~RaceServer()
{
}

void RaceServer::initialize(const CL_String& p_levelName)
{
	if (m_initialized) {
		assert(0 && "already initialized");
	}

	m_levelName = p_levelName;

	m_level = new Level();
	m_level->initialize(p_levelName);

	RacePlayer* player;

	std::pair<CL_NetGameConnection*, Player*> pair;

	foreach (pair, m_server->m_connections) {
		player = new RacePlayer(pair.second);
		m_racePlayers[pair.first] = player;

		m_level->addCar(&player->getCar());
	}

	m_initialized = true;

	// connect the slots if not connected yet
	if (!m_slotsConnected) {
		m_slots.connect(m_server->signalPlayerConnected(), this, &RaceServer::slotPlayerConnected);
		m_slots.connect(m_server->signalPlayerDisconnected(), this, &RaceServer::slotPlayerDisconnected);

		m_slotsConnected = true;
	}
}

void RaceServer::destroy()
{
	if (!m_initialized) {
		assert(0 && "not initialized");
	}

	delete m_level;

	std::pair<CL_NetGameConnection*, RacePlayer*> pair;

	foreach (pair, m_racePlayers) {
		delete pair.second;
	}

	m_racePlayers.clear();
}

void RaceServer::slotPlayerConnected(CL_NetGameConnection *p_connection, Player *p_player)
{
	if (!m_initialized) {
		return;
	}

	cl_log_event("event", "Adding race player");

	RacePlayer *racePlayer = new RacePlayer(p_player);

	m_racePlayers[p_connection] = racePlayer;
	m_level->addCar(&racePlayer->getCar());

	// send the gamestate
	cl_log_event("event", "Sending gamestate");

	GameState gamestate;
	gamestate.setLevel(m_levelName);
	m_server->send(p_connection, gamestate.genGameStateEvent());
}

void RaceServer::slotPlayerDisconnected(CL_NetGameConnection *p_connection, Player *p_player)
{
	if (!m_initialized) {
		return;
	}

	cl_log_event("event", "Removing race player");

	std::map<CL_NetGameConnection*, RacePlayer*>::iterator itor = m_racePlayers.find(p_connection);

	assert(itor != m_racePlayers.end());

	m_level->removeCar(&itor->second->getCar());

	delete itor->second;
	m_racePlayers.erase(itor);
}

void RaceServer::handleEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{

	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_CAR_STATE_CHANGE) {
		handleCarStateChangeEvent(p_connection, p_event);
	} else if (eventName == EVENT_TRIGGER_RACE_START) {
		handleTriggerRaceStartEvent(p_connection, p_event);
	} else if (eventName == EVENT_PLAYER_FINISHED) {
		handlePlayerFinishedEvent(p_connection, p_event);
	} else {
		cl_log_event("error", "unhandled event: %1", eventName);
	}

}

void RaceServer::handleCarStateChangeEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "handling %1", p_event.to_string());

	assert(m_racePlayers.find(p_connection) != m_racePlayers.end() && "Player not in RaceServer player list");

	// apply state to local race player
	RacePlayer* player = m_racePlayers[p_connection];
	Car& car = player->getCar();

	car.applyStatusEvent(p_event, 1);

	// send the info to others
	m_server->sendToAll(p_event, p_connection);

	// check finished state
	if (!player->isFinished()) {
		if (car.getLap() > m_lapsNum) {
			player->setFinished(true);

			// send finished event
			const CL_NetGameEvent finishedEvent(EVENT_PLAYER_FINISHED, player->getPlayer().getName());
			m_server->sendToAll(finishedEvent);
		}
	}
}

void RaceServer::handleTriggerRaceStartEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "handling %1", p_event.to_string());

	const int lapsNum = (int) p_event.get_argument(0);

	// lock cars movement
	const CL_NetGameEvent lockEvent(EVENT_LOCK_CAR);
	m_server->sendToAll(lockEvent);

	// set their position
	std::pair<CL_NetGameConnection*, RacePlayer*> pair;

	int startPositionNum = 1;
	foreach (pair, m_racePlayers) {

		pair.second->setFinished(false);

		Car &car = pair.second->getCar();
		car.setStartPosition(startPositionNum);

		Player &player = pair.second->getPlayer();

		CL_NetGameEvent startPositionEvent(EVENT_CAR_STATE_CHANGE);
		startPositionEvent.add_argument(player.getName()); // self player

		car.prepareStatusEvent(startPositionEvent);

		CL_NetGameConnection* connection = m_server->getConnectionForPlayer(&player);

		if (connection != NULL) {
			m_server->sendToAll(startPositionEvent);
		} else {
			cl_log_event("error", "available RacePlayer not found in Server object");
		}


		++startPositionNum;
	}

	// set the laps number
	m_lapsNum = lapsNum;

	// send the information about this race
	const CL_NetGameEvent raceState(EVENT_RACE_STATE, lapsNum);
	m_server->sendToAll(raceState);

	// start countdown to unlock on all clients
	CL_NetGameEvent countdownEvent(EVENT_START_COUNTDOWN);
	m_server->sendToAll(countdownEvent);
}

void RaceServer::handlePlayerFinishedEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	unsigned time = p_event.get_argument(0);

	// set the player finished state
	RacePlayer *racePlayer = m_racePlayers[p_connection];
	racePlayer->setFinished(true);

	// send similar event to other players
	CL_NetGameEvent event(EVENT_PLAYER_FINISHED, racePlayer->getPlayer().getName(), time);
	m_server->sendToAll(event, p_connection);
}
