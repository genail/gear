/*
 * RaceServer.cpp
 *
 *  Created on: 2009-09-27
 *      Author: chudy
 */

#include "network/RaceServer.h"

#include <assert.h>
#include <limits.h>

#include "common.h"
#include "network/events.h"
#include "network/Server.h"

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

void RaceServer::initialize()
{
	if (m_initialized) {
		assert(0 && "already initialized");
	}

	if (!m_slotsConnected) {
		m_slots.connect(m_server->signalPlayerConnected(), this, &RaceServer::slotPlayerConnected);
		m_slots.connect(m_server->signalPlayerDisconnected(), this, &RaceServer::slotPlayerDisconnected);

		m_slotsConnected = true;
	}

	RacePlayer* player;

	std::pair<CL_NetGameConnection*, Player*> pair;

	foreach (pair, m_server->m_connections) {
		player = new RacePlayer(pair.second);
		m_racePlayers[pair.first] = player;
	}

	m_initialized = true;
}

void RaceServer::destroy()
{
	if (!m_initialized) {
		assert(0 && "not initialized");
	}

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

	m_racePlayers[p_connection] = new RacePlayer(p_player);
}

void RaceServer::slotPlayerDisconnected(CL_NetGameConnection *p_connection, Player *p_player)
{
	if (!m_initialized) {
		return;
	}

	std::map<CL_NetGameConnection*, RacePlayer*>::iterator itor = m_racePlayers.find(p_connection);

	assert(itor != m_racePlayers.end());

	delete itor->second;
	m_racePlayers.erase(itor);
}

void RaceServer::handleEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{

	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_CAR_STATE_CHANGE) {
		handleCarStateChangeEvent(p_connection, p_event);
	}  else if (eventName == EVENT_TRIGGER_RACE_START) {
		handleTriggerRaceStartEvent(p_connection, p_event);
	} else {
		cl_log_event("error", "unhandled event: %1", eventName);
	}

}

void RaceServer::handleCarStateChangeEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "handling %1", p_event.to_string());

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
		Car &car = pair.second->getCar();
		car.setStartPosition(startPositionNum);

		Player &player = pair.second->getPlayer();

		CL_NetGameEvent startPositionEvent(EVENT_CAR_STATE_CHANGE);
		startPositionEvent.add_argument(""); // self player

		car.prepareStatusEvent(startPositionEvent);

		CL_NetGameConnection* connection = m_server->getConnectionForPlayer(&player);

		if (connection != NULL) {
			m_server->send(connection, startPositionEvent);
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
