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

#include "network/Client.h"
#include "network/events.h"

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

	try {
		m_gameClient.connect(p_host, port);

		// act as connected and wait for connection initialization
		m_connected = true;
		m_welcomed = false;

		while (m_connected) {
			CL_KeepAlive::process();

			if (m_welcomed) {
				cl_log_event("network", "Connection fully initialized");
				break;
			}

			CL_System::sleep(10);
		}
	} catch (CL_Exception e) {
		cl_log_event("exception", "Cannot connect to %1:%2", p_host, p_port);
		m_connected = false;
	}
}

void Client::disconnect()
{
	if (m_connected) {
		m_gameClient.disconnect();
	}
}

Client::~Client() {

	if (isConnected()) {
		m_gameClient.disconnect();
	}
}

void Client::slotConnected()
{

	// I am connected
	// And I should introduce myself

	cl_log_event("network", "Introducing myself as %1", m_player->getName());

	CL_NetGameEventValue nickname(m_player->getName());
	CL_NetGameEvent hiEvent(EVENT_HI, nickname);

	m_gameClient.send_event(hiEvent);
}

void Client::slotDisconnected()
{
	// I am disconnected
	// Is that what I've expected?
	cl_log_event("network", "Disconnected from server");

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
			if (eventName == EVENT_WELCOME) {
				handleWelcomeEvent(p_event);
			} else if (eventName == EVENT_PLAYER_CONNECTED) {
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

void Client::handlePlayerConnectedEvent(const CL_NetGameEvent &p_netGameEvent)
{
	const CL_String playerName = (CL_String) p_netGameEvent.get_argument(0);

	Player* player = new Player(playerName);
	m_remotePlayers.push_back(player);

	cl_log_event("event", "Player '%1' connected", playerName);

	m_signalPlayerConnected.invoke(player);
}

void Client::handlePlayerDisconnectedEvent(const CL_NetGameEvent &p_netGameEvent)
{
	const CL_String playerName = (CL_String) p_netGameEvent.get_argument(0);

	cl_log_event("event", "Player '%1' disconnected", playerName);

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


void Client::send(const CL_NetGameEvent &p_event)
{
	m_gameClient.send_event(p_event);
}

void Client::handleInitRaceEvent(const CL_NetGameEvent &p_event)
{
	m_signalInitRace.invoke(p_event.get_argument(0));
}

void Client::handleWelcomeEvent(const CL_NetGameEvent &p_netGameEvent)
{
	m_welcomed = true;
}
