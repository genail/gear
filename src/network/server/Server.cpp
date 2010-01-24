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

#include "Server.h"

#include "common.h"
#include "ServerConfiguration.h"
#include "logic/race/level/Level.h"
#include "network/events.h"
#include "network/version.h"
#include "network/packets/CarState.h"
#include "network/packets/ClientInfo.h"
#include "network/packets/Goodbye.h"
#include "network/packets/GameState.h"
#include "network/packets/PlayerJoined.h"
#include "network/packets/PlayerLeaved.h"
#include "network/packets/RaceStart.h"
#include "network/packets/VoteStart.h"
#include "network/packets/VoteEnd.h"
#include "network/packets/VoteTick.h"
#include "network/server/VoteSystem.h"

namespace Net {

const int VOTE_TIME_LIMIT_SEC = 30;


class ServerImpl
{
	public:

		IMPL_SIGNAL_1(playerJoined, const CL_String&);

		IMPL_SIGNAL_1(playerLeaved, const CL_String&);

		struct Player {

			CL_String m_name;

			bool m_gameStateSent;

			CarState m_lastCarState;

			Player() :
				m_gameStateSent(false)
			{}
		};

		/** Server configuration */
		const ServerConfiguration m_conf;

		/** Running state */
		bool m_running;

		/** List of active connections */
		typedef std::map<CL_NetGameConnection*, Player> TConnectionPlayerMap;
		typedef std::pair<CL_NetGameConnection*, Player> TConnectionPlayerPair;

		TConnectionPlayerMap m_connections;

		/** The level */
		Race::Level m_level;

		/** Voting system */
		VoteSystem m_voteSystem;

		/** ClanLib game server */
		CL_NetGameServer m_gameServer;

		/** Slots container */
		CL_SlotContainer m_slots;


		ServerImpl(const ServerConfiguration &p_conf) :
			m_conf(p_conf), // copy this object
			m_running(false)
		{ /* empty */ }


		// helpers

		void send(CL_NetGameConnection *p_con, const CL_NetGameEvent &p_event);

		void sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL, bool p_ignoreNotFullyConnected = true);

		GameState prepareGameState();

		void startRace();


		// network events

		void onClientConnected(CL_NetGameConnection *p_connection);

		void onClientDisconnected(CL_NetGameConnection *p_connection);

		void onEventArrived(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		//
		// event handlers
		//

		void onClientInfo(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void onCarState(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void onVoteStart(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		void onVoteTick(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		//
		// other events
		//

		void onVoteSystemFinished();
};

METH_SIGNAL_1(Server, playerJoined, const CL_String&);
METH_SIGNAL_1(Server, playerLeaved, const CL_String&);

Server::Server(const ServerConfiguration &p_conf) :
	m_impl(new ServerImpl(p_conf))
{
	const CL_String levPath = cl_format("%1/%2", LEVELS_DIR, p_conf.getLevel());

	if (!CL_FileHelp::file_exists(levPath)) {
		cl_log_event(LOG_ERROR, "level %1 doesn't exists", levPath);
		exit(1);
	}

	m_impl->m_slots.connect(
			m_impl->m_gameServer.sig_client_connected(),
			m_impl.get(), &ServerImpl::onClientConnected
	);

	m_impl->m_slots.connect(
			m_impl->m_gameServer.sig_client_disconnected(),
			m_impl.get(), &ServerImpl::onClientDisconnected
	);

	m_impl->m_slots.connect(
			m_impl->m_gameServer.sig_event_received(),
			m_impl.get(), &ServerImpl::onEventArrived
	);

	m_impl->m_voteSystem.func_finished().set(
			m_impl.get(), &ServerImpl::onVoteSystemFinished
	);
}

Server::~Server()
{
	if (m_impl->m_running) {
		stop();
	}
}

void Server::start()
{
	G_ASSERT(!m_impl->m_running);

	try {
		m_impl->m_gameServer.start(
				CL_StringHelp::int_to_local8(m_impl->m_conf.getPort())
		);

		m_impl->m_running = true;

		cl_log_event(LOG_INFO, "server is up and running");

	} catch (const CL_Exception &e) {
		cl_log_event(LOG_ERROR, "unable to start the server: %1", e.message);
	}
}

void Server::stop()
{
	G_ASSERT(m_impl->m_running);

	try {
		m_impl->m_gameServer.stop();
		m_impl->m_running = false;
	} catch (const CL_Exception &e) {
		cl_log_event(LOG_ERROR, "unable to stop the server: %1", e.message);
	}
}

void ServerImpl::onClientConnected(CL_NetGameConnection *p_conn)
{
	cl_log_event(LOG_EVENT, "player %1 is connected", (unsigned) p_conn);

	Player player;

	player.m_lastCarState.setPosition(CL_Pointf(250, 250));

	m_connections[p_conn] = player;

	// no signal invoke yet
}

void ServerImpl::onClientDisconnected(CL_NetGameConnection *p_netGameConnection)
{
	cl_log_event(
			LOG_EVENT,
			"player %1 disconnects",
			m_connections[p_netGameConnection].m_name.empty()
				? CL_StringHelp::uint_to_local8((unsigned) p_netGameConnection)
				: m_connections[p_netGameConnection].m_name
	);

	std::map<CL_NetGameConnection*, Player>::iterator itor =
			m_connections.find(p_netGameConnection);

	if (itor != m_connections.end()) {

		const Player &player = itor->second;

		// emit the signal if player was in the game
		if (player.m_gameStateSent) {
			INVOKE_1(playerLeaved, player.m_name);
		}

		// send event to rest of players
		PlayerLeaved playerLeaved;
		playerLeaved.setName(player.m_name);;

		sendToAll(playerLeaved.buildEvent(), itor->first);

		// cleanup
		m_connections.erase(itor);
	}
}

void ServerImpl::onEventArrived(
		CL_NetGameConnection *p_conn,
		const CL_NetGameEvent &p_event
)
{
	cl_log_event(LOG_EVENT, "event %1 arrived", p_event.to_string());

	try {
		bool unhandled = false;
		const CL_String eventName = p_event.get_name();

		// connection initialize events

		if (eventName == EVENT_CLIENT_INFO) {
			onClientInfo(p_conn, p_event);
		}

		// race events

		else if (eventName == EVENT_CAR_STATE) {
			onCarState(p_conn, p_event);
		} else if (eventName == EVENT_VOTE_START) {
			onVoteStart(p_conn, p_event);
		} else if (eventName == EVENT_VOTE_TICK) {
			onVoteTick(p_conn, p_event);
		}

		// unknown events remains unhandled

		else {
			unhandled = true;
		}


		if (unhandled) {
			cl_log_event(
					LOG_EVENT,
					"event %1 remains unhandled",
					p_event.to_string()
			);
		}
	} catch (CL_Exception e) {
		cl_log_event(LOG_ERROR, e.message);
	}

}

void ServerImpl::onVoteStart(
		CL_NetGameConnection *p_conn,
		const CL_NetGameEvent &p_event
)
{
	VoteStart voteStart;
	voteStart.parseEvent(p_event);

	if (!m_voteSystem.isRunning()) {
		cl_log_event(LOG_EVENT, "starting a new vote");

		m_voteSystem.start(
				voteStart.getType(),
				m_connections.size(),
				VOTE_TIME_LIMIT_SEC * 1000
		);

		// set the time limit
		voteStart.setTimeLimit(VOTE_TIME_LIMIT_SEC);

		// send new event
		sendToAll(voteStart.buildEvent());
	}
}

void ServerImpl::onVoteTick(
		CL_NetGameConnection *p_conn,
		const CL_NetGameEvent &p_event
)
{
	VoteTick voteTick;
	voteTick.parseEvent(p_event);

	if (!m_voteSystem.isFinished()) {
		const bool accepted =
				m_voteSystem.addVote(voteTick.getOption(), (int) p_conn);

		if (accepted && !m_voteSystem.isFinished()) {
			// send this vote over network
			sendToAll(p_event);
		}
	}
}

void ServerImpl::onVoteSystemFinished()
{
	// voting finished, send event
	VoteEnd voteEnd;
	voteEnd.setResult(m_voteSystem.getResult());

	sendToAll(voteEnd.buildEvent());

	// check if I should take action
	if (m_voteSystem.getResult() == VOTE_PASSED) {
		switch (m_voteSystem.getType()) {
			case VOTE_RESTART_RACE:
				startRace();
				break;

			default:
				assert(0 && "unknown VoteType");
		}
	}
}

void ServerImpl::onCarState(
		CL_NetGameConnection *p_conn,
		const CL_NetGameEvent &p_event)
{
	// register last car state
	Player &player = m_connections[p_conn];
	player.m_lastCarState.parseEvent(p_event);

	// set players name (client may not set it to his nickname)
	player.m_lastCarState.setName(player.m_name);

	// send it all over
	sendToAll(player.m_lastCarState.buildEvent(), p_conn);
}

void ServerImpl::onClientInfo(
		CL_NetGameConnection *p_conn,
		const CL_NetGameEvent &p_event
)
{
	ClientInfo clientInfo;
	clientInfo.parseEvent(p_event);

	// check the version
	if (clientInfo.getProtocolVersion().getMajor() != PROTOCOL_VERSION_MAJOR) {
		cl_log_event(
				LOG_EVENT,
				"unsupported protocol version for player '%2'",
				reinterpret_cast<unsigned>(p_conn)
		);

		// send goodbye
		Net::Goodbye goodbye;
		goodbye.setGoodbyeReason(GR_UNSUPPORTED_PROTOCOL_VERSION);

		send(p_conn, goodbye.buildEvent());
		return;
	}

	// check name availability
	// check name availability
	bool nameAvailable = true;
	TConnectionPlayerPair pair;
	foreach (pair, m_connections) {
		if (pair.second.m_name == clientInfo.getName()) {
			nameAvailable = false;
		}
	}

	if (!nameAvailable) {
		cl_log_event(
				LOG_EVENT,
				"name '%1' already in use for player '%2'",
				clientInfo.getName(),
				reinterpret_cast<unsigned>(p_conn)
		);

		// send goodbye
		Goodbye goodbye;
		goodbye.setGoodbyeReason(GR_NAME_ALREADY_IN_USE);

		send(p_conn, goodbye.buildEvent());
		return;
	}

	// set the name and inform all
	m_connections[p_conn].m_name = clientInfo.getName();

	cl_log_event(
			LOG_EVENT,
			"'%1' is now known as '%2', sending gamestate...",
			reinterpret_cast<unsigned>(p_conn),
			clientInfo.getName()
	);

	PlayerJoined playerJoined;
	playerJoined.setName(clientInfo.getName());

	sendToAll(playerJoined.buildEvent(), p_conn);

	// send the gamestate
	const GameState gamestate = prepareGameState();
	send(p_conn, gamestate.buildEvent());

	m_connections[p_conn].m_gameStateSent = true;
}

GameState ServerImpl::prepareGameState()
{
	GameState gamestate;

	TConnectionPlayerPair pair;

	foreach (pair, m_connections) {
		const ServerImpl::Player &player = pair.second;
		gamestate.addPlayer(player.m_name, player.m_lastCarState);
	}


	const CL_String levPath = cl_format("%1/%2", LEVELS_DIR, m_conf.getLevel());
	gamestate.setLevel(levPath);

	return gamestate;
}


void ServerImpl::send(
		CL_NetGameConnection *p_con,
		const CL_NetGameEvent &p_event
)
{
	p_con->send_event(p_event);
}

void ServerImpl::sendToAll(
		const CL_NetGameEvent &p_event,
		const CL_NetGameConnection* p_ignore,
		bool p_ignoreNotFullyConnected
)
{
	TConnectionPlayerPair pair;

	foreach(pair, m_connections) {

		if (pair.first == p_ignore) {
			continue;
		}

		if (p_ignoreNotFullyConnected && !pair.second.m_gameStateSent) {
			continue;
		}

		pair.first->send_event(p_event);
	}
}

void ServerImpl::startRace()
{
	RaceStart raceStart;

	TConnectionPlayerPair pair;

	int i = 1;
	CL_Pointf pos;
	CL_Angle rot;

	foreach (pair, m_connections) {
		m_level.getStartPosAndRot(i, &pos, &rot);
		raceStart.setCarPosition(pos);
		raceStart.setCarRotation(rot);

		send(pair.first, raceStart.buildEvent());

		++i;
	}

	sendToAll(raceStart.buildEvent());
}

} // namespace

