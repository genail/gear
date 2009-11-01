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

#include "network/Server.h"

#include <assert.h>

#include "common.h"
#include "network/events.h"
#include "network/version.h"
#include "network/Goodbye.h"
#include "network/ClientInfo.h"
#include "network/PlayerJoined.h"

namespace Net {

Server::Server() :
	m_bindPort(DEFAULT_PORT),
	m_running(false)
{
	m_slots.connect(m_gameServer.sig_client_connected(), this, &Server::onClientConnected);
	m_slots.connect(m_gameServer.sig_client_disconnected(), this, &Server::onClientDisconnected);
	m_slots.connect(m_gameServer.sig_event_received(), this, &Server::onEventArrived);
}

Server::~Server()
{
	if (m_running) {
		stop();
	}
}

void Server::start()
{
	assert(!m_running);

	try {
		m_gameServer.start(CL_StringHelp::int_to_local8(m_port));
		m_running = true;
	} catch (const CL_Exception &e) {
		cl_log_event("runtime", "Unable to start the server: %1", e.message);
	}
}

void Server::stop()
{
	assert(m_running);

	try {
		m_gameServer.stop();
		m_running = false;
	} catch (const CL_Exception &e) {
		cl_log_event("runtime", "Unable to stop the server: %1", e.message);
	}
}

void Server::onClientConnected(CL_NetGameConnection *p_conn)
{
	cl_log_event("network", "Player %1 is connected", (unsigned) p_conn);

	Player player;
	m_connections[p_conn] = player;

	// no signal invoke yet
}

void Server::onClientDisconnected(CL_NetGameConnection *p_netGameConnection)
{
	cl_log_event("network", "Player %1 disconnects", m_connections[p_netGameConnection].m_name.empty() ? CL_StringHelp::uint_to_local8((unsigned) p_netGameConnection) : m_connections[p_netGameConnection].m_name);

	std::map<CL_NetGameConnection*, Player>::iterator itor = m_connections.find(p_netGameConnection);

	if (itor != m_connections.end()) {

		const Player &player = itor->second;

		// emit the signal if player was in the game
		if (player.m_gameStateSent) {
			INVOKE_1(playerLeaved, player.m_name);
		}

		// cleanup
		m_connections.erase(itor);
	}
}

void Server::onEventArrived(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "Event %1 arrived", p_event.to_string());

	try {
		bool unhandled = false;

		// connection initialize events

		if (eventName == EVENT_CLIENT_INFO) {
			onClientInfo(p_connection, p_event);
		}


		if (unhandled) {
			cl_log_event("event", "Event %1 remains unhandled", p_event.to_string());
		}
	} catch (CL_Exception e) {
		cl_log_event("exception", e.message);
	}

}

void Server::onClientInfo(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	ClientInfo clientInfo;
	clientInfo.parseEvent(p_event);

	// check the version
	if (clientInfo.getProtocolVersion().getMajor() != PROTOCOL_VERSION_MAJOR) {
		cl_log_event("event", "Unsupported protocol version for player '%2'", (unsigned) p_connection);

		// send goodbye
		Goodbye goodbye;
		goodbye.setGoodbyeReason(Goodbye::UNSUPPORTED_PROTOCOL_VERSION);

		send(p_connection, goodbye.buildEvent());
		return;
	}

	// check name availability
	// check name availability
	bool nameAvailable = true;
	std::pair<CL_NetGameConnection*, Server::Player> pair;
	foreach (pair, m_connections) {
		if (pair.second.m_name == clientInfo.getName()) {
			nameAvailable = false;
		}
	}

	if (!nameAvailable) {
		cl_log_event("event", "Name '%1' already in use for player '%2'", clientInfo.getName(), (unsigned) p_connection);

		// send goodbye
		Goodbye goodbye;
		goodbye.setGoodbyeReason(Goodbye::NAME_ALREADY_IN_USE);

		send(p_connection, goodbye.buildEvent());
		return;
	}

	// set the name and inform all
	m_connections[p_connection].m_name = clientInfo.getName();

	PlayerJoined playerInfo;
	playerInfo.setName(clientInfo.getName());

	sendToAll();

}

void Server::handleHiEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	const CL_String playerName = p_event.get_argument(0);

	if (playerName.empty()) {
		cl_log_event("error", "Protocol error while handling %1 event", p_event.to_string());
		return;
	}

	// send the welcome message
	const CL_NetGameEvent welcomeEvent(EVENT_WELCOME);
	send(p_connection, welcomeEvent);


	// check name availability
	bool nameAvailable = true;
	std::pair<CL_NetGameConnection*, Player*> pair;
	foreach (pair, m_connections) {
		if (pair.second->getName() == playerName) {
			nameAvailable = false;
		}
	}

	if (!nameAvailable) {
		// refuse of nick set
		send(p_connection, CL_NetGameEvent(EVENT_PLAYER_NICK_IN_USE));

		cl_log_event("event", "Name '%1' already in use for player '%2'", playerName, (unsigned) p_connection);
	} else {
		// resend player's connection event

		m_connections[p_connection]->setName(playerName);

		sendToAll(CL_NetGameEvent(EVENT_PLAYER_CONNECTED, playerName), p_connection);

		cl_log_event("event", "Player %1 is now known as '%2'", (unsigned) p_connection, playerName);

		// and send him the list of other players
		std::pair<CL_NetGameConnection*, Player*> pair;
		foreach(pair, m_connections) {
			if (pair.first == p_connection) {
				continue;
			}

			CL_NetGameEvent replyEvent(EVENT_PLAYER_CONNECTED, pair.second->getName());

			send(p_connection, replyEvent);
		}


	}
}


void Server::send(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	p_connection->send_event(p_event);
}

void Server::sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore, bool p_ignoreNotFullyConnected)
{
	CL_MutexSection lockSection(&m_lockMutex);

	std::pair<CL_NetGameConnection*, ServerPlayer*> pair;

	foreach(pair, m_connections) {

		if (pair.first == p_ignore) {
			continue;
		}

		if (p_ignoreNotFullyConnected && !pair.second.m_gameStateSent) {
			continue;
		}

		pair.first->send_event(p_event);
	}
}

CL_NetGameConnection* Server::getConnectionForPlayer(const Player* player)
{
	CL_MutexSection lockSection(&m_lockMutex);

	std::pair<CL_NetGameConnection*, Player*> pair;

	foreach (pair, m_connections) {
		if (pair.second == player) {
			return pair.first;
		}
	}

	return NULL;
}

void Server::handleInitRaceEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	assert(0 && "no longer supported");

	// if race is initialized, then send him the init event
	if (m_raceServer.isInitialized()) {
		CL_NetGameEvent raceInitEvent(EVENT_INIT_RACE, m_raceServer.getLevelName());
		send(p_connection, raceInitEvent);

		return;
	}

	if (isPermitted(p_connection)) {
		const CL_String levelName = (CL_String) p_event.get_argument(0);

		// if race is initialized, then destroy it now
		if (m_raceServer.isInitialized()) {
			m_raceServer.destroy();
		}

		cl_log_event("event", "Initializing the race on level %1", levelName);

		// initialize the level
		m_raceServer.initialize(levelName);

		// send init race event to all players
		sendToAll(p_event);
	} else {
		cl_log_event("error", "Race is not initialized, and player %1 is not allowed to do this", m_connections[p_connection]->getName());
	}

}

void Server::handleGrantEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	const CL_String &playerName = m_connections[p_connection]->getName();

	const CL_String password = p_event.get_argument(0);

	if (password == "123") { // FIXME: store the password as server configuration
		m_permitedConnection = p_connection;

		cl_log_event("perm", "Player %1 is the root", playerName);
	} else {
		cl_log_event("perm", "Wrong root password for player %1", playerName);
	}
}

} // namespace

