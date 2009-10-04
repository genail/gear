/*
 * Client.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "network/Client.h"
#include "network/events.h"

#include "Debug.h"

Client::Client() :
	m_connected(false),
	m_raceClient(this)
{
	m_slots.connect(m_gameClient.sig_connected(), this, &Client::slotConnected);
	m_slots.connect(m_gameClient.sig_disconnected(), this, &Client::slotDisconnected);
	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::slotEventReceived);
}

void Client::connect(const CL_String &p_host, int p_port, Player *p_player) {
	m_player = p_player;

	const CL_String port = CL_StringHelp::int_to_local8(p_port);

	cl_log_event("network", "Connecting to %1:%2", p_host, port);

	m_gameClient.connect(p_host, port);
}

Client::~Client() {

	if (isConnected()) {
		m_gameClient.disconnect();
	}
}

void Client::slotConnected() {

	m_connected = true;

	// I am connected
	// And I should introduce myself

	cl_log_event("network", "Connected, introducing myself");

	CL_NetGameEventValue nickname(m_player->getName());
	CL_NetGameEvent hiEvent(EVENT_HI, nickname);

	m_gameClient.send_event(hiEvent);
}

void Client::slotDisconnected() {
	// I am disconnected
	// Is that what I've expected?
	Debug::out() << "Disconnected from server" << std::endl;

	m_connected = false;
}

void Client::slotEventReceived(const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "Event %1 arrived", p_event.to_string());

	try {
		const CL_String eventName = p_event.get_name();
		const std::vector<CL_TempString> parts = CL_StringHelp::split_text(eventName, ":");

		bool unhandled = false;

		if (parts[0] == EVENT_PREFIX_GENERAL) {
			if (eventName == EVENT_PLAYER_CONNECTED) {
				handlePlayerConnectedEvent(p_event);
				return;
			} else if (eventName == EVENT_PLAYER_DISCONNECTED) {
				handlePlayerDisconnectedEvent(p_event);
				return;
			} else if (eventName == EVENT_INIT_RACE) {
				handleInitRaceEvent(p_event);
			} else {
				unhandled = true;
			}
		} else if (parts[0] == EVENT_PREFIX_RACE) {
			m_raceClient.handleEvent(p_event);
		} else {
			unhandled = true;
		}

		if (unhandled) {
			cl_log_event("error", "Event %1 remains unhandled", p_event.to_string());
		}

	} catch (CL_Exception e) {
		cl_log_event("exception", e.message);
	}

}

void Client::handlePlayerConnectedEvent(const CL_NetGameEvent &p_netGameEvent) {
	Debug::out() << "handling " << p_netGameEvent.get_name().c_str() << std::endl;

	const CL_String playerName = (CL_String) p_netGameEvent.get_argument(0);

	Player* player = new Player(playerName);
	m_remotePlayers.push_back(player);

	Debug::out() << "Player connected: " << playerName.c_str() << std::endl;

	m_signalPlayerConnected.invoke(player);
}

void Client::handlePlayerDisconnectedEvent(const CL_NetGameEvent &p_netGameEvent) {
	Debug::out() << "handling " << p_netGameEvent.get_name().c_str() << std::endl;

	const CL_String playerName = (CL_String) p_netGameEvent.get_argument(0);

	Debug::out() << "Player disconnected: " << playerName.c_str() << std::endl;

	for (
		std::vector<Player*>::iterator itor = m_remotePlayers.begin();
		itor != m_remotePlayers.end();
		++itor
	) {
		if ((*itor)->getName() == playerName) {

			m_signalPlayerDisconnected.invoke(*itor);

			delete *itor;
			m_remotePlayers.erase(itor);
			break;
		}
	}
}

void Client::eventPrepareRace(const CL_NetGameEvent &p_netGameEvent) {
	const int position = (int) p_netGameEvent.get_argument(0);


}

void Client::send(const CL_NetGameEvent &p_event) {
	m_gameClient.send_event(p_event);
}

void Client::handleInitRaceEvent(const CL_NetGameEvent &p_event)
{
	m_signalInitRace.invoke(p_event.get_argument(0));
}
