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

#include "Game.h"
#include "common.h"
#include "network/events.h"
#include "network/Goodbye.h"
#include "network/PlayerInfo.h"

namespace Net {

Client::Client() :
	m_port(DEFAULT_PORT),
	m_connected(false)
//	m_raceClient(this)
{
	m_slots.connect(m_gameClient.sig_connected(), this, &Client::onConnected);
	m_slots.connect(m_gameClient.sig_disconnected(), this, &Client::onDisconnected);
	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::onEventReceived);
}

void Client::connect() {

	assert(!m_addr.empty() && m_port > 0 && m_port < 0xFFFF);

	const CL_String port = CL_StringHelp::int_to_local8(m_port);

	cl_log_event("network", "Connecting to %1:%2", m_addr, m_port);

	try {
		m_gameClient.connect(m_addr, port);
	} catch (CL_Exception e) {
		cl_log_event("exception", "Cannot connect to %1:%2", m_addr, m_port);
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

void Client::onConnected()
{
	m_connected = true;
	m_signalConnected.invoke();

	// I am connected
	// Sending player info
	ClientInfo playerInfo;
	playerInfo.setName(Game::getInstance().getPlayer().getName());

	cl_log_event("network", "Introducing myself as %1", playerInfo.getName());

	send(playerInfo.buildEvent());
}

void Client::onDisconnected()
{
	// I am disconnected
	// Is that what I've expected?
	cl_log_event("network", "Disconnected from server");

	m_connected = false;

	m_signalDisconnected.invoke();
}

void Client::onEventReceived(const CL_NetGameEvent &p_event)
{
	cl_log_event("event", "Event %1 arrived", p_event.to_string());

	try {
		const CL_String eventName = p_event.get_name();
		bool unhandled = false;

		// connect / disconnect procedure

		if (eventName == EVENT_GOODBYE) {
			onGoodbye(p_event);
		} else if (eventName == EVENT_GAME_STATE) {
			onGameState(p_event);
		} else

		// player events

		if (eventName == EVENT_PLAYER_JOINED) {
			onPlayerJoined(p_event);
		} else if (eventName == EVENT_PLAYER_LEAVED) {
			onPlayerLeaved(p_event);
		} else

		// unknown events remain unhandled

		{
			unhandled = true;
		}

		if (unhandled) {
			cl_log_event("error", "Event %1 remains unhandled", p_event.to_string());
		}

	} catch (CL_Exception e) {
		cl_log_event("exception", e.message);
	}

}

void Client::onGoodbye(const CL_NetGameEvent &p_event)
{
	Goodbye goodbye;
	goodbye.parseEvent(p_event);

	cl_log_event("event", "Server says goodbye: %1", goodbye.getStringMessage());
	disconnect();
}

void Client::onGameState(const CL_NetGameEvent &p_gameState)
{
	try {
		GameState gamestate;
		gamestate.parseGameStateEvent(p_gameState);

		INVOKE_1(gameStateReceived, gamestate);
	} catch (CL_Exception &e) {
		cl_log_event("protocol error on GAMESTATE: %1", e.message);
	}

}

void Client::onPlayerJoined(const CL_NetGameEvent &p_event)
{
	const CL_String name = p_event.get_argument(0);
	cl_log_event("event", "Player '%1' joined the game", name);
	INVOKE_1(playerJoined, name);
}

void Client::onPlayerLeaved(const CL_NetGameEvent &p_event)
{
	const CL_String name = p_event.get_argument(0);
	cl_log_event("event", "Player '%1' leaved the game", name);
	INVOKE_1(playerLeaved, name);
}

void Client::onCarState(const CL_NetGameEvent &p_event)
{

}

void Client::send(const CL_NetGameEvent &p_event)
{
	m_gameClient.send_event(p_event);
}

} // namespace
