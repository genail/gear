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

#include "Server.h"

#include "common.h"
#include "common/Properties.h"
#include "logic/race/Car.h"
#include "logic/race/level/Level.h"
#include "math/Float.h"
#include "network/events.h"
#include "network/version.h"
#include "network/packets/CarState.h"
#include "network/packets/ClientInfo.h"
#include "network/packets/Goodbye.h"
#include "network/packets/GameMode.h"
#include "network/packets/GameState.h"
#include "network/packets/PlayerJoined.h"
#include "network/packets/PlayerLeft.h"
#include "network/packets/RaceStart.h"
#include "network/packets/VoteStart.h"
#include "network/packets/VoteEnd.h"
#include "network/packets/VoteTick.h"
#include "network/server/MasterServerRegistrant.h"
#include "network/server/VoteSystem.h"

namespace Net {

const int VOTE_TIME_LIMIT_SEC = 30;

class ServerImpl
{
	public:

		SIG_IMPL(Server, playerJoined);

		SIG_IMPL(Server, playerLeft);


		struct Player {

			CL_String m_name;

			bool m_gameStateSent;

			CL_SharedPtr<Race::Car> m_car;

			CarState m_lastCarState;

			Player() :
				m_gameStateSent(false),
				m_car(new Race::Car())
			{}
		};


		typedef std::map<CL_NetGameConnection*, Player> TConnectionPlayerMap;
		typedef std::pair<CL_NetGameConnection*, Player> TConnectionPlayerPair;


		Server *m_parent;

		bool m_running;

		CL_NetGameServer m_gameServer;

		TConnectionPlayerMap m_connections;


		Race::Level m_level;

		VoteSystem m_voteSystem;


		CL_Thread m_masterServerThread;

		MasterServerRegistrant m_serverRegistrant;


		CL_SlotContainer m_slots;


		ServerImpl(Server *p_parent);

		~ServerImpl();


		void startServerRegistrant();

		void stopServerRegistrant();


		// helpers

		void send(CL_NetGameConnection *p_con, const CL_NetGameEvent &p_event);

		void sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL, bool p_ignoreNotFullyConnected = true);

		GameState prepareGameState();

		void startRace();

		void kick(CL_NetGameConnection *p_conn, GoodbyeReason p_reason);


		// network events

		void handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event);

		void onClientConnected(CL_NetGameConnection *p_connection);

		void sendGameMode(CL_NetGameConnection *p_conn);

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

SIG_CPP(Server, playerJoined);
SIG_CPP(Server, playerLeft);

Server::Server() :
	m_impl(new ServerImpl(this))
{
	// empty
}

Server::~Server()
{
	if (m_impl->m_running) {
		stop();
	}
}

ServerImpl::ServerImpl(Server *p_parent) :
		m_parent(p_parent),
		m_running(false)
{
	const CL_String levPath =
			cl_format(
					"%1/%2",
					LEVELS_DIR,
					Properties::getString(SRV_LEVEL)
			);

	if (!CL_FileHelp::file_exists(levPath)) {
		cl_log_event(LOG_ERROR, "level %1 doesn't exists, exiting", levPath);
		exit(1);
	}

	m_level.load(levPath);
	if (!m_level.isUsable()) {
		cl_log_event(LOG_ERROR, "level %1 is not usable, exiting", levPath);
		exit(1);
	}

	m_slots.connect(
			m_gameServer.sig_client_connected(),
			this, &ServerImpl::onClientConnected
	);

	m_slots.connect(
			m_gameServer.sig_client_disconnected(),
			this, &ServerImpl::onClientDisconnected
	);

	m_slots.connect(
			m_gameServer.sig_event_received(),
			this, &ServerImpl::onEventArrived
	);

	m_voteSystem.func_finished().set(
			this, &ServerImpl::onVoteSystemFinished
	);
}

ServerImpl::~ServerImpl()
{
	// empty
}

void Server::start()
{
	G_ASSERT(!m_impl->m_running);

	const int port = Properties::getInt(SRV_PORT, DEFAULT_PORT);

	try {
		m_impl->m_gameServer.start(CL_StringHelp::int_to_local8(port));

		m_impl->m_running = true;
		cl_log_event(LOG_INFO, "server is up and running");

		m_impl->startServerRegistrant();
	} catch (const CL_Exception &e) {
		cl_log_event(LOG_ERROR, "unable to start the server: %1", e.message);
	}
}

void ServerImpl::startServerRegistrant()
{
	cl_log_event(LOG_DEBUG, "launching game server register thread");
	m_masterServerThread.start(&m_serverRegistrant);
}

void Server::stop()
{
	G_ASSERT(m_impl->m_running);

	try {
		m_impl->m_gameServer.stop();
		m_impl->m_running = false;
		m_impl->stopServerRegistrant();
	} catch (const CL_Exception &e) {
		cl_log_event(LOG_ERROR, "unable to stop the server: %1", e.message);
	}
}

void ServerImpl::stopServerRegistrant()
{
	cl_log_event(LOG_DEBUG, "stopping game server register thread");
	m_serverRegistrant.interrupt();
	m_masterServerThread.join();
}

void ServerImpl::onClientConnected(CL_NetGameConnection *p_conn)
{
	cl_log_event(LOG_EVENT, "player %1 is connected", (unsigned) p_conn);

	Player player;

	// set default car state
	CL_NetGameEvent data("");

	player.m_car->setPosition(CL_Pointf(-50.0f, -50.0f));
	player.m_car->serialize(&data);
	player.m_lastCarState.setSerializedData(data);

	m_connections[p_conn] = player;

	sendGameMode(p_conn);
}

void ServerImpl::onClientDisconnected(CL_NetGameConnection *p_conn)
{
	cl_log_event(
			LOG_EVENT,
			"player %1 disconnects",
			m_connections[p_conn].m_name.empty()
				? CL_StringHelp::uint_to_local8((unsigned) p_conn)
				: m_connections[p_conn].m_name
	);

	std::map<CL_NetGameConnection*, Player>::iterator itor =
			m_connections.find(p_conn);

	G_ASSERT(itor != m_connections.end());

	const Player &player = itor->second;

	// emit the signal if player was in the game
	if (player.m_gameStateSent) {
		INVOKE_1(playerLeft, player.m_name);
	}

	// send event to rest of players
	PlayerLeft playerLeft;
	playerLeft.setName(player.m_name);;

	sendToAll(playerLeft.buildEvent(), itor->first);

	// cleanup
	m_connections.erase(itor);
}

void ServerImpl::onEventArrived(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	cl_log_event(LOG_EVENT, "event %1 arrived", p_event.to_string());
	handleEvent(p_conn, p_event);
}

void Server::handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
	m_impl->handleEvent(p_conn, p_event);
}

void ServerImpl::handleEvent(CL_NetGameConnection *p_conn, const CL_NetGameEvent &p_event)
{
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
	// state check precision
	static const float PRECISSION = 0.5f;


	// register last car state
	Player &player = m_connections[p_conn];
	player.m_lastCarState.parseEvent(p_event);

	// player name may be not set by client
	player.m_lastCarState.setName(player.m_name);

	sendToAll(player.m_lastCarState.buildEvent(), p_conn);


	// validate client physics calculations
	// and kick player when his state differs from server one
	if (player.m_car->getIterationId() != -1) {

		try {
			CL_NetGameEvent serverState("");

			player.m_car->updateToIteration(
					player.m_lastCarState.getIterationId()
			);

			// compare server and client car position

			// this is position calculated by server
			const CL_Pointf servPos = player.m_car->getPosition();

			// apply client data and retrieve his position
			player.m_car->deserialize(
					player.m_lastCarState.getSerializedData()
			);
			const CL_Pointf &cliPos = player.m_car->getPosition();


			if (!player.m_lastCarState.isAfterCollision()) {
				if (
						!Math::Float::cmp(servPos.x, cliPos.x, PRECISSION)
						|| !Math::Float::cmp(servPos.y, cliPos.y, PRECISSION)
				) {
					cl_log_event(
							LOG_WARN,
							CL_String("diff in client and server states:\n")
							+ "client x: %1  y: %2\nserver x: %3  y: %4",
							cliPos.x, cliPos.y,
							servPos.x, servPos.y
					);

					kick(p_conn, GR_CHEATING);
				}
			}

		} catch (CL_Exception &e) {
			// something went wrong while comparing states
			// this also may be a cheater doing
			cl_log_event(LOG_WARN, "%1", e.message);
			kick(p_conn, GR_CHEATING);
		}

	} else {
		// this is first iteration, read data without checking it
		player.m_car->deserialize(player.m_lastCarState.getSerializedData());
	}

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
		kick(p_conn, GR_UNSUPPORTED_PROTOCOL_VERSION);
		return;
	}

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
		kick(p_conn, GR_NAME_ALREADY_IN_USE);
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

void ServerImpl::sendGameMode(CL_NetGameConnection *p_conn)
{
	GameMode gameModePacket;
	gameModePacket.setGameModeType(m_parent->getGameMode());

	const CL_NetGameEvent event = gameModePacket.buildEvent();
	send(p_conn, event);
}

TGameMode Server::getGameMode() const
{
	return GM_ARCADE;
}

GameState ServerImpl::prepareGameState()
{
	GameState gamestate;

	TConnectionPlayerPair pair;

	foreach (pair, m_connections) {
		const ServerImpl::Player &player = pair.second;
		gamestate.addPlayer(player.m_name, player.m_lastCarState);
	}


	const CL_String levPath =
			cl_format(
					"%1/%2",
					LEVELS_DIR,
					Properties::getString(SRV_LEVEL)
			);

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
}

void ServerImpl::kick(CL_NetGameConnection *p_conn, GoodbyeReason p_reason)
{
	Goodbye goodbye;
	goodbye.setGoodbyeReason(p_reason);

	send(p_conn, goodbye.buildEvent());
	p_conn->disconnect();
}

} // namespace

