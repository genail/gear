/*
 * Server.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Server.h"


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
	CL_Console::write_line("New client connected");

	Player player;
	player.m_carId = nextCarId();

	m_connections[p_netGameConnection] = player;
}

void Server::slotClientDisconnected(CL_NetGameConnection *p_netGameConnection) {
	CL_Console::write_line("Client disconnected");

	for (std::map<CL_NetGameConnection*, Player>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if ((*itor).first == p_netGameConnection) {
			m_connections.erase(itor);
			break;
		}
	}
}

void Server::slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent) {
	CL_Console::write_line("Event received");

	// attach car id and send over
//	const CL_NetGameEventValue carId(m_connections[p_netGameConnection].m_carId);
//	p_netGameEvent.add_argument(carId);

	for (std::map<CL_NetGameConnection*, Player>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if ((*itor).first != p_netGameConnection) {
			(*itor).first->send_event(p_netGameEvent);
		}
	}

}
