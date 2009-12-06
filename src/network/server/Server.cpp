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

#include "Server.h"

#include <assert.h>

#include "common.h"
#include "network/events.h"
#include "network/version.h"
#include "../packets/Goodbye.h"
#include "../packets/GameState.h"
#include "../packets/ClientInfo.h"
#include "../packets/PlayerJoined.h"

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
		m_gameServer.start(CL_StringHelp::int_to_local8(m_bindPort));
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
		const CL_String eventName = p_event.get_name();

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

void Server::onClientInfo(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	ClientInfo clientInfo;
	clientInfo.parseEvent(p_event);

	// check the version
	if (clientInfo.getProtocolVersion().getMajor() != PROTOCOL_VERSION_MAJOR) {
		cl_log_event("event", "Unsupported protocol version for player '%2'", (unsigned) p_conn);

		// send goodbye
		Net::Goodbye goodbye;
		goodbye.setGoodbyeReason(Goodbye::UNSUPPORTED_PROTOCOL_VERSION);

		send(p_conn, goodbye.buildEvent());
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
		cl_log_event("event", "Name '%1' already in use for player '%2'", clientInfo.getName(), (unsigned) p_conn);

		// send goodbye
		Goodbye goodbye;
		goodbye.setGoodbyeReason(Goodbye::NAME_ALREADY_IN_USE);

		send(p_conn, goodbye.buildEvent());
		return;
	}

	// set the name and inform all
	m_connections[p_conn].m_name = clientInfo.getName();

	cl_log_event("event", "'%1' is now known as '%2', sending gamestate...", (unsigned) p_conn, clientInfo.getName());

	PlayerJoined playerJoined;
	playerJoined.setName(clientInfo.getName());

	sendToAll(playerJoined.buildEvent(), p_conn);

	// send the gamestate
	const GameState gamestate = prepareGameState();
	send(p_conn, gamestate.buildEvent());

	m_connections[p_conn].m_gameStateSent = true;
}

GameState Server::prepareGameState()
{
	GameState gamestate;

	std::pair<CL_NetGameConnection*, Server::Player> pair;

	foreach (pair, m_connections) {
		const Server::Player &player = pair.second;

		if (player.m_gameStateSent) {
			gamestate.addPlayer(player.m_name, player.m_lastCarState);
		}
	}

	gamestate.setLevel("resources/level.xml"); // FIXME: How to choose level?

	return gamestate;
}


void Server::send(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	p_connection->send_event(p_event);
}

void Server::sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore, bool p_ignoreNotFullyConnected)
{
	std::pair<CL_NetGameConnection*, Server::Player> pair;

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

//CL_NetGameConnection* Server::getConnectionForPlayer(const Player* player)
//{
//	CL_MutexSection lockSection(&m_lockMutex);
//
//	std::pair<CL_NetGameConnection*, Player*> pair;
//
//	foreach (pair, m_connections) {
//		if (pair.second == player) {
//			return pair.first;
//		}
//	}
//
//	return NULL;
//}

} // namespace

