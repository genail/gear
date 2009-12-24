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

#pragma once

#include <ClanLib/core.h>
#include <ClanLib/network.h>

#include "common.h"
#include "../packets/CarState.h"
#include "../packets/GameState.h"
#include "VoteSystem.h"

namespace Net {

class Server {

	SIGNAL_1(playerJoined, const CL_String&);

	SIGNAL_1(playerLeaved, const CL_String&);

		struct Player {

				CL_String m_name;

				bool m_gameStateSent;

				CarState m_lastCarState;

				Player() :
					m_gameStateSent(false)
				{}
		};

	public:

		Server();

		virtual ~Server();

		void setBindPort(unsigned short p_port) { m_bindPort = p_port; }


		void start();

		void stop();


	private:
		/** Bind port number */
		unsigned short m_bindPort;

		/** Running state */
		bool m_running;

		/** List of active connections */
		typedef std::map<CL_NetGameConnection*, Server::Player> TConnectionPlayerMap;
		typedef std::pair<CL_NetGameConnection*, Server::Player> TConnectionPlayerPair;

		TConnectionPlayerMap m_connections;

		/** Voting system */
		VoteSystem m_voteSystem;

		/** ClanLib game server */
		CL_NetGameServer m_gameServer;

		/** Slots container */
		CL_SlotContainer m_slots;


		// helpers

		void send(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL, bool p_ignoreNotFullyConnected = true);

		GameState prepareGameState();

		void startRace();


		// network events

		void onClientConnected(CL_NetGameConnection *p_connection);

		void onClientDisconnected(CL_NetGameConnection *p_connection);

		void onEventArrived(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		//
		// event handlers
		//

		void onClientInfo(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void onCarState(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void onVoteStart(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void onVoteTick(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		//
		// other events
		//

		void onVoteSystemFinished();
};

} // namespace
