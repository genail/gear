/*
 * Server.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Server.h"
#include "Message.h"
#include "network/Events.h"

Server::Server(int p_port) :
	m_nextCarId(1)
{
	m_slots.connect(m_gameServer.sig_client_connected(), this, &Server::slotClientConnected);
	m_slots.connect(m_gameServer.sig_client_disconnected(), this, &Server::slotClientDisconnected);
	m_slots.connect(m_gameServer.sig_event_received(), this, &Server::slotEventArrived);

	m_gameServer.start(CL_StringHelp::int_to_local8(p_port));
}

Server::~Server() {
	m_gameServer.stop();
}

void Server::update(int p_timeElapsed) {
	m_gameServer.process_events();
}

void Server::slotClientConnected(CL_NetGameConnection *p_netGameConnection) {
	Message::out() << "Player " << (unsigned) p_netGameConnection << " is connected" << std::endl;

	Player *player = new Player();
	m_connections[p_netGameConnection] = player;
}

void Server::slotClientDisconnected(CL_NetGameConnection *p_netGameConnection) {
	Message::out() << "Player "
				 << (m_connections[p_netGameConnection]->getName().empty() ? CL_StringHelp::uint_to_local8((unsigned) p_netGameConnection) : m_connections[p_netGameConnection]->getName()).c_str()
				 << " has disconnected"
				 << std::endl;

	std::map<CL_NetGameConnection*, Player*>::iterator itor = m_connections.find(p_netGameConnection);
	if (itor != m_connections.end()) {
		m_connections.erase(itor);
	}
}

void Server::slotEventArrived(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event) {
	const CL_String eventName = p_event.get_name();

	Debug::out() << "Event " << eventName.c_str() << " received" << std::endl;

	if (eventName == EVENT_HI) {
		handleHiEvent(p_connection, p_event);
	}

//	send(p_event, p_connection);
}

void Server::handleHiEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event) {
	const CL_String playerName = p_event.get_argument(0);

	if (playerName.empty()) {
		Message::err() << "Protocol error when handling 'hi' event" << std::endl;
		return;
	}

	// check availability
	bool nameAvailable = true;
	for (std::map<CL_NetGameConnection*, Player*>::const_iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if (itor->second->getName() == playerName) {
			nameAvailable = false;
		}
	}

	if (!nameAvailable) {
		// refuse of nick set
		reply(p_connection, CL_NetGameEvent(EVENT_PLAYER_NICK_IN_USE));

		Message::out() << "Name '" << playerName.c_str() << "' already in use for " << (unsigned) p_connection << std::endl;
	} else {
		// resend player's connection event
		m_connections[p_connection]->setName(playerName);
		send(CL_NetGameEvent(EVENT_PLAYER_CONNECTED, playerName), p_connection);

		Message::out() << "Player " << (unsigned) p_connection << " is now known as '" << playerName.c_str() << "'" << std::endl;

		// and send others player information to the new one
		for (std::map<CL_NetGameConnection*, Player*>::const_iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
			if (itor->first == p_connection) {
				continue;
			}

			reply(p_connection, CL_NetGameEvent(EVENT_PLAYER_CONNECTED, itor->second->getName()));
		}
	}
}

void Server::reply(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event) {
	p_connection->send_event(p_event);
}

void Server::send(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore) {
	for (std::map<CL_NetGameConnection*, Player*>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if (itor->first != p_ignore) {
			itor->first->send_event(p_event);
		}
	}
}

void Server::prepareRace() {

//	int position = 1;
//
//	for (std::map<CL_NetGameConnection*, Player>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
//
//		CL_NetGameEvent prepareEvent("prepare_race");
//		const CL_NetGameEventValue positionValue(position++);
//		prepareEvent.add_argument(positionValue);
//
//		itor->first->send_event(prepareEvent);
//	}
}
