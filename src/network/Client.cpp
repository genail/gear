/*
 * Client.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Client.h"

#include "Debug.h"

Client::Client() :
	m_connected(false)
{
	m_slots.connect(m_gameClient.sig_connected(), this, &Client::slotConnected);
	m_slots.connect(m_gameClient.sig_disconnected(), this, &Client::slotDisconnected);
	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::slotEventReceived);
}

void Client::connect(const CL_String &p_host, int p_port, Player *p_player) {
	m_player = p_player;
	m_gameClient.connect(p_host, CL_StringHelp::int_to_local8(p_port));
}

Client::~Client() {

	if (isConnected()) {
		m_gameClient.disconnect();
	}
}

void Client::slotConnected() {
	// I am connected
	// And I should introduce myself

	CL_NetGameEventValue nickname(m_player->getName());
	CL_NetGameEvent hiEvent("general:hi", nickname);

	m_gameClient.send_event(hiEvent);
}

void Client::slotDisconnected() {
	// I am disconnected
	// Is that what I've expected?
	Debug::out << "Disconnected from server" << std::endl;
}

void Client::slotEventReceived(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String eventName = p_netGameEvent.get_name();

	CL_Console::write_line(CL_String8("got event: ") + eventName);

	if (eventName == "general:hi") {
		eventHi(p_netGameEvent);
	}

}

void Client::eventHi(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String nickname = (CL_String) p_netGameEvent.get_argument(0);

	Player* player = new Player(nickname);
	m_remotePlayers.push_back(player);

	Debug::out << "Player connected: " << nickname.c_str() << std::endl;

	m_signalPlayerConnected.invoke(player);

}

void Client::eventPrepareRace(const CL_NetGameEvent &p_netGameEvent) {
	const int position = (int) p_netGameEvent.get_argument(0);


}
