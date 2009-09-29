/*
 * Server.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Server.h"

#include "common.h"
#include "network/events.h"

Server::Server(int p_port) :
	m_raceServer(this)
{
	m_slots.connect(m_gameServer.sig_client_connected(), this, &Server::slotClientConnected);
	m_slots.connect(m_gameServer.sig_client_disconnected(), this, &Server::slotClientDisconnected);
	m_slots.connect(m_gameServer.sig_event_received(), this, &Server::slotEventArrived);

	m_gameServer.start(CL_StringHelp::int_to_local8(p_port));

	// TESTING
	m_raceServer.initialize();
}

Server::~Server()
{
	m_gameServer.stop();
}

void Server::update(int p_timeElapsed)
{
//	m_gameServer.process_events();
}

void Server::slotClientConnected(CL_NetGameConnection *p_netGameConnection)
{
	cl_log_event("network", "Player %1 is connected", (unsigned) p_netGameConnection);

	Player *player = new Player();
	m_connections[p_netGameConnection] = player;

	// emit the signal
	m_signalPlayerConnected.invoke(p_netGameConnection, player);
}

void Server::slotClientDisconnected(CL_NetGameConnection *p_netGameConnection)
{
	cl_log_event("network", "Player %1 disconnects", m_connections[p_netGameConnection]->getName().empty() ? CL_StringHelp::uint_to_local8((unsigned) p_netGameConnection) : m_connections[p_netGameConnection]->getName());

	std::map<CL_NetGameConnection*, Player*>::iterator itor = m_connections.find(p_netGameConnection);

	if (itor != m_connections.end()) {

		Player* player = itor->second;

		// emit the signal
		m_signalPlayerDisconnected.invoke(p_netGameConnection, player);

		// cleanup
		delete player;
		m_connections.erase(itor);
	}
}

void Server::slotEventArrived(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{

	cl_log_event("event", "Event %1 arrived", p_event.to_string());

	const CL_String eventName = p_event.get_name();

	const std::vector<CL_TempString> parts = CL_StringHelp::split_text(eventName, ":");

	if (parts[0] == EVENT_PREFIX_GENERAL) {
		if (eventName == EVENT_HI) {
			handleHiEvent(p_connection, p_event);
			return;
		}
	} else if (parts[0] == EVENT_PREFIX_RACE) {
		m_raceServer.handleEvent(p_connection, p_event);
		return;
	}

	cl_log_event("event", "Event %1 remains unhandled", p_event.to_string());

}

void Server::handleHiEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	const CL_String playerName = p_event.get_argument(0);

	if (playerName.empty()) {
		cl_log_event("error", "Protocol error while handling %1 event", p_event.to_string());
		return;
	}

	// check availability
	bool nameAvailable = true;
	for (
		std::map<CL_NetGameConnection*, Player*>::const_iterator itor = m_connections.begin();
		itor != m_connections.end();
		++itor
	) {
		if (itor->second->getName() == playerName) {
			nameAvailable = false;
		}
	}

	if (!nameAvailable) {
		// refuse of nick set
		reply(p_connection, CL_NetGameEvent(EVENT_PLAYER_NICK_IN_USE));

		cl_log_event("event", "Name '%1' already in use for player '%2'", playerName, (unsigned) p_connection);
	} else {
		// resend player's connection event
		m_connections[p_connection]->setName(playerName);
		send(CL_NetGameEvent(EVENT_PLAYER_CONNECTED, playerName), p_connection);

		cl_log_event("event", "Player %1 is now known as '%2'", (unsigned) p_connection, playerName);

		// and send him the list of other players
		std::pair<CL_NetGameConnection*, Player*> pair;
		foreach(pair, m_connections) {
			if (pair.first == p_connection) {
				continue;
			}

			reply(p_connection, CL_NetGameEvent(EVENT_PLAYER_CONNECTED, pair.second->getName()));
		}
	}
}

void Server::reply(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event)
{
	p_connection->send_event(p_event);
}

void Server::send(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore)
{
	std::pair<CL_NetGameConnection*, Player*> pair;
	foreach(pair, m_connections) {
		if (pair.first != p_ignore) {
			pair.first->send_event(p_event);
		}
	}
}

void Server::prepareRace()
{

}
