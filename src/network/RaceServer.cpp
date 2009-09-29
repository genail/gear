/*
 * RaceServer.cpp
 *
 *  Created on: 2009-09-27
 *      Author: chudy
 */

#include "network/RaceServer.h"

#include <assert.h>

#include "Message.h"
#include "network/events.h"
#include "network/Server.h"

RaceServer::RaceServer(Server* p_server) :
	m_initialized(false),
	m_server(p_server),
	m_slotsConnected(false)
{
}

RaceServer::~RaceServer() {
}

void RaceServer::initialize() {
	if (m_initialized) {
		assert(0 && "already initialized");
	}

	if (!m_slotsConnected) {
		m_slots.connect(m_server->signalPlayerConnected(), this, &RaceServer::slotPlayerConnected);
		m_slots.connect(m_server->signalPlayerDisconnected(), this, &RaceServer::slotPlayerDisconnected);

		m_slotsConnected = true;
	}

	RacePlayer* player;

	for (
		std::map<CL_NetGameConnection*, Player*>::iterator itor = m_server->m_connections.begin();
		itor != m_server->m_connections.end();
		++itor
	) {
		player = new RacePlayer(itor->second);
		m_racePlayers[itor->first] = player;
	}

	m_initialized = true;
}

void RaceServer::destroy() {
	if (!m_initialized) {
		assert(0 && "not initialized");
	}

	for (
		std::map<CL_NetGameConnection*, RacePlayer*>::iterator itor = m_racePlayers.begin();
			itor != m_racePlayers.end();
			++itor
	) {
		delete itor->second;
	}

	m_racePlayers.clear();
}

void RaceServer::slotPlayerConnected(CL_NetGameConnection *p_connection, Player *p_player) {

	if (!m_initialized) {
		return;
	}

	m_racePlayers[p_connection] = new RacePlayer(p_player);
}

void RaceServer::slotPlayerDisconnected(CL_NetGameConnection *p_connection, Player *p_player) {

	if (!m_initialized) {
		return;
	}

	std::map<CL_NetGameConnection*, RacePlayer*>::iterator itor = m_racePlayers.find(p_connection);

	assert(itor != m_racePlayers.end());

	delete itor->second;
	m_racePlayers.erase(itor);
}

void RaceServer::handleEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event) {
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_CAR_STATE_CHANGE) {
		handleCarStateChangeEvent(p_connection, p_event);
	} else {
		Debug::err() << "unhandled event: " << eventName.c_str() << std::endl;
	}
}

void RaceServer::handleCarStateChangeEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event) {
	Debug::out() << "handling " << p_event.get_name().c_str() << std::endl;
	m_server->send(p_event, p_connection);
}
