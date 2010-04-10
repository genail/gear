/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#include "Client.h"

#include "common/Game.h"
#include "common.h"
#include "network/events.h"
#include "network/packets/Goodbye.h"
#include "network/packets/ClientInfo.h"
#include "network/packets/GameState.h"
#include "network/packets/CarState.h"
#include "network/packets/VoteStart.h"
#include "network/packets/VoteEnd.h"
#include "network/packets/VoteTick.h"
#include "network/packets/RaceStart.h"

namespace Net {

Client::Client() :
	m_port(DEFAULT_PORT),
	m_connected(false),
	m_rankingClient(this)
{
	m_slots.connect(m_gameClient.sig_connected(), this, &Client::onConnected);
	m_slots.connect(m_gameClient.sig_disconnected(), this, &Client::onDisconnected);
	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::onEventReceived);
}

Client::~Client() {

	if (m_connected) {
		m_gameClient.disconnect();
	}
}

bool Client::connect() {

	assert(!m_addr.empty() && m_port > 0 && m_port < 0xFFFF);

	const CL_String port = CL_StringHelp::int_to_local8(m_port);

	cl_log_event("network", "Connecting to %1:%2", m_addr, m_port);

	try {
		m_gameClient.connect(m_addr, port);
		m_connected = true;
	} catch (CL_Exception e) {
		cl_log_event("exception", "Cannot connect to %1:%2", m_addr, m_port);
		return false;
	}

	return true;
}

void Client::disconnect()
{
	if (m_connected) {
		m_gameClient.disconnect();
	}
}

bool Client::isConnected() const
{
	return m_connected;
}

void Client::send(const CL_NetGameEvent &p_event)
{
	cl_log_event("network", p_event.to_string());
	m_gameClient.send_event(p_event);
}

void Client::sendCarState(const Net::CarState &p_state)
{
	send(p_state.buildEvent());
}

void Client::onConnected()
{
	INVOKE_0(connected);

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

	INVOKE_0(disconnected);
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
		}

		// player events

		else if (eventName == EVENT_PLAYER_JOINED) {
			onPlayerJoined(p_event);
		} else if (eventName == EVENT_PLAYER_LEFT) {
			onPlayerLeaved(p_event);
		}

		// race events

		else if (eventName == EVENT_CAR_STATE) {
			onCarState(p_event);
		} else if (eventName == EVENT_RACE_START) {
			onRaceStart(p_event);
		} else if (eventName == EVENT_VOTE_START) {
			onVoteStart(p_event);
		} else if (eventName == EVENT_VOTE_END) {
			onVoteEnd(p_event);
		} else if (eventName == EVENT_VOTE_TICK) {
			onVoteTick(p_event);
		}

		// unknown events remain unhandled

		else {
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

	INVOKE_2(goodbyeReceived, goodbye.getGoodbyeReason(), goodbye.getStringMessage());
}

void Client::onGameState(const CL_NetGameEvent &p_gameState)
{
	try {
		GameState gamestate;
		gamestate.parseEvent(p_gameState);

		INVOKE_1(gameStateReceived, gamestate);
	} catch (CL_Exception &e) {
		cl_log_event(LOG_ERROR, "protocol error on GAMESTATE: %1", e.message);
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
	CarState state;
	state.parseEvent(p_event);

	INVOKE_1(carStateReceived, state);
}

void Client::onRaceStart(const CL_NetGameEvent &p_event)
{
	RaceStart raceStart;
	raceStart.parseEvent(p_event);

	INVOKE_2(raceStartReceived, raceStart.getCarPosition(), raceStart.getCarRotation());
}

void Client::onVoteStart(const CL_NetGameEvent &p_event)
{
	VoteStart voteStart;
	voteStart.parseEvent(p_event);

	INVOKE_3(voteStarted, voteStart.getType(), voteStart.getSubject(), voteStart.getTimeLimit());
}

void Client::onVoteEnd(const CL_NetGameEvent &p_event)
{
	VoteEnd voteEnd;
	voteEnd.parseEvent(p_event);

	INVOKE_1(voteEnded, voteEnd.getResult());
}

void Client::onVoteTick(const CL_NetGameEvent &p_event)
{
	VoteTick voteTick;
	voteTick.parseEvent(p_event);

	INVOKE_1(voteTickReceived, voteTick.getOption());
}

void Client::callAVote(VoteType p_type, const CL_String& subject)
{
	VoteStart voteStart;

	voteStart.setType(p_type);
	voteStart.setSubject(subject);

	send(voteStart.buildEvent());
}


void Client::voteNo()
{
	vote(false);
}

void Client::voteYes()
{
	vote(true);
}

void Client::vote(bool p_yes)
{
	VoteTick tick;
	tick.setOption(p_yes ? VOTE_YES : VOTE_NO);

	send(tick.buildEvent());
}

RankingClient &Client::getRankingClient()
{
	return m_rankingClient;
}

} // namespace
