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

#include "ClanLib/core.h"
#include "ClanLib/network.h"

#include "common.h"
#include "common/votetypes.h"
#include "common/Player.h"
#include "logic/race/Car.h"
#include "logic/race/Level.h"

namespace Net {

class CarState;
class GameState;

class Client {

		/** Client is connected */
		SIGNAL_0(connected);

		/** Client is disconnected */
		SIGNAL_0(disconnected);

		/** Player info accepted */
		SIGNAL_0(accepted);

		/** Received game state */
		SIGNAL_1(const Net::GameState&, gameStateReceived);

		/** New player joined */
		SIGNAL_1(const CL_String&, playerJoined);

		/** Player leaved */
		SIGNAL_1(const CL_String&, playerLeaved);

		/** Got new car state */
		SIGNAL_1(const Net::CarState&, carStateReceived);

		/** New vote is started. args: type, subject, time limit in seconds */
		SIGNAL_3(VoteType, const CL_String&, unsigned, voteStarted);

		/** Vote has ended. */
		SIGNAL_1(VoteResult, voteEnded);

	public:

		Client();

		virtual ~Client();


		const CL_String& getServerAddr() const { return m_addr; }

		int getServerPort() const { return m_port; }


		void setServerAddr(const CL_String& p_addr) { m_addr = p_addr; }

		void setServerPort(int p_port) { m_port = p_port; }


		void callAVote(VoteType p_type, const CL_String& subject="");

		void connect();

		void disconnect();

		void sendCarState(const Net::CarState &p_state);

		void voteNo();

		void voteYes();


	private:

		/** Server addr */
		CL_String m_addr;

		/** Server port */
		int m_port;

		/** Connected state */
		bool m_connected;

		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** The slot container */
		CL_SlotContainer m_slots;


		//
		// helpers
		//

		void send(const CL_NetGameEvent &p_event);

		void vote(bool p_yes);

		//
		// connection events
		//

		/** This client is connected */
		void onConnected();

		/** This client has been disconnected */
		void onDisconnected();

		/** Event received */
		void onEventReceived(const CL_NetGameEvent &p_event);

		//
		// game events receivers
		//

		void onGoodbye(const CL_NetGameEvent &p_event);

		void onGameState(const CL_NetGameEvent &p_gameState);

		void onPlayerJoined(const CL_NetGameEvent &p_event);

		void onPlayerLeaved(const CL_NetGameEvent &p_event);

		void onCarState(const CL_NetGameEvent &p_event);

		void onVoteStart(const CL_NetGameEvent &p_event);

		void onVoteEnd(const CL_NetGameEvent &p_event);

};

} // namespace
