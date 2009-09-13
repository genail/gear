/*
 * Server.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Server.h"


Server::Server(int p_port)
//	m_listenPort(CL_StringHelp::int_to_local8(p_port)),
//	m_listen(m_listenPort)
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
//	bool someoneConnected = m_listen.get_accept_event().wait(0);
//
//	if (someoneConnected) {
//		CL_TCPConnection connection = m_listen.accept();
//		CL_SocketName remoteName = connection.get_remote_name();
//		CL_Console::write_line("%1 connected", remoteName.get_address());
//		CL_String8 message = "Hello There!";
//		connection.write(message.data(), message.length());
//		connection.disconnect_graceful();
//	}
	m_gameServer.process_events();
}

void Server::slotClientConnected(CL_NetGameConnection *p_netGameConnection) {
	CL_Console::write_line("New client connected");
	m_connections.push_back(p_netGameConnection);
}

void Server::slotClientDisconnected(CL_NetGameConnection *p_netGameConnection) {
	CL_Console::write_line("Client disconnected");

	for (std::vector<CL_NetGameConnection*>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if ((*itor) == p_netGameConnection) {
			m_connections.erase(itor);
			break;
		}
	}
}

void Server::slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent) {
	CL_Console::write_line("Event received");
//	CL_Console::write_line(CL_StringHelp::int_to_local8((int) p_netGameEvent.get_argument(0)));
	for (std::vector<CL_NetGameConnection*>::iterator itor = m_connections.begin(); itor != m_connections.end(); ++itor) {
		if ((*itor) != p_netGameConnection) {
			p_netGameConnection->send_event(p_netGameEvent);
		}
	}

}
