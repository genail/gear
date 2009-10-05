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
	m_permitedConnection(NULL),
	m_raceServer(this)
{
	m_slots.connect(m_gameServer.sig_client_connected(), this, &Server::slotClientConnected);
	m_slots.connect(m_gameServer.sig_client_disconnected(), this, &Server::slotClientDisconnected);
	m_slots.connect(m_gameServer.sig_event_received(), this, &Server::slotEventArrived);

	m_gameServer.start(CL_StringHelp::int_to_local8(p_port));
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
	CL_MutexSection lockSection(&m_lockMutex);

	cl_log_event("network", "Player %1 is connected", (unsigned) p_netGameConnection);

	Player *player = new Player();
	m_connections[p_netGameConnection] = player;

	// if this is first player, then it grants the permission
	if (m_connections.size() == 1) {
		m_permitedConnection = p_netGameConnection;
	}

	// emit the signal
	m_signalPlayerConnected.invoke(p_netGameConnection, player);
}

void Server::slotClientDisconnected(CL_NetGameConnection *p_netGameConnection)
{
	CL_MutexSection lockSection(&m_lockMutex);

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

	try {
		const CL_String eventName = p_event.get_name();
		const std::vector<CL_TempString> parts = CL_StringHelp::split_text(eventName, ":");
		bool unhandled = false;

		if (parts[0] == EVENT_PREFIX_GENERAL) {

			if (eventName == EVENT_HI) {
				handleHiEvent(p_connection, p_event);
			} else if (eventName == EVENT_GRANT_PERMISSIONS) {
				handleGrantEvent(p_connection, p_event);
			} else if (eventName == EVENT_INIT_RACE) {
				handleInitRaceEvent(p_connection, p_event);
			} else {
				unhandled = true;
			}

		} else if (parts[0] == EVENT_PREFIX_RACE) {
			CL_MutexSection lockSection(&m_lockMutex);
			m_raceServer.handleEvent(p_connection, p_event);
		} else {
			unhandled = true;
		}

		if (unhandled) {
			cl_log_event("event", "Event %1 remains unhandled", p_event.to_string());
		}
	} catch (CL_Exception e) {
		cl_log_event("exception", e.message);
	}

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

void Server::sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore)
{
	CL_MutexSection lockSection(&m_lockMutex);

	std::pair<CL_NetGameConnection*, Player*> pair;
	foreach(pair, m_connections) {
		if (pair.first != p_ignore) {
			pair.first->send_event(p_event);
		}
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

