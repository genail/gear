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

#pragma once

#include "ClanLib/core.h"
#include "ClanLib/network.h"

#include "common.h"
#include "common/types.h"
#include "common/votetypes.h"
#include "common/Player.h"
#include "logic/race/Car.h"
#include "logic/race/level/Level.h"
#include "network/client/RankingClient.h"

namespace Net {

class CarState;
class GameState;

class Client {

		/** Client is connected */
		SIGNAL_0(connected);

		/** Client is disconnected */
		SIGNAL_0(disconnected);

		/** Server sends goodbye: reason, reason text */
		SIGNAL_2(goodbyeReceived, GoodbyeReason, const CL_String&);

		/** Player info accepted */
		SIGNAL_0(accepted);

		/** Received game state */
		SIGNAL_1(gameStateReceived, const Net::GameState&);

		/** New player joined */
		SIGNAL_1(playerJoined, const CL_String&);

		/** Player leaved */
		SIGNAL_1(playerLeaved, const CL_String&);

		/** Got new car state */
		SIGNAL_1(carStateReceived, const Net::CarState&);

		/** Should start the race */
		SIGNAL_2(raceStartReceived, const CL_Pointf&, const CL_Angle&);

		/** New vote is started. args: type, subject, time limit in seconds */
		SIGNAL_3(voteStarted, VoteType, const CL_String&, unsigned);

		/** Got vote tick */
		SIGNAL_1(voteTickReceived, VoteOption);

		/** Vote has ended. */
		SIGNAL_1(voteEnded, VoteResult);

	public:

		Client();

		virtual ~Client();


		bool isConnected() const;


		const CL_String& getServerAddr() const { return m_addr; }

		int getServerPort() const { return m_port; }

		RankingClient &getRankingClient();


		void setServerAddr(const CL_String& p_addr) { m_addr = p_addr; }

		void setServerPort(int p_port) { m_port = p_port; }


		void callAVote(VoteType p_type, const CL_String& subject="");

		bool connect();

		void disconnect();

		void sendCarState(const Net::CarState &p_state);

		void voteNo();

		void voteYes();


	private:

		CL_String m_addr;

		int m_port;

		volatile bool m_connected;

		CL_NetGameClient m_gameClient;

		RankingClient m_rankingClient;

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

		void onRaceStart(const CL_NetGameEvent &p_event);

		void onVoteStart(const CL_NetGameEvent &p_event);

		void onVoteEnd(const CL_NetGameEvent &p_event);

		void onVoteTick(const CL_NetGameEvent &p_event);


		friend class RankingClient;
		friend class RankingClientImpl;

};

} // namespace
