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

#include <map>
#include <ClanLib/core.h>

#include "common/RemotePlayer.h"
#include "RaceLogic.h"
#include "network/client/Client.h"

namespace Net {
	class CarState;
	class GameState;
}

namespace Race {

class Car;

class OnlineRaceLogic: public Race::RaceLogic {

	public:

		OnlineRaceLogic(const CL_String &p_host, int p_port);

		virtual ~OnlineRaceLogic();


		virtual void initialize();

		virtual void destroy();

		// vote system

		virtual void callAVote(VoteType p_type, const CL_String &p_subject);


		virtual const CL_String &getVoteMessage() const;

		virtual int getVoteNoCount() const;

		virtual int getVoteYesCount() const;

		virtual unsigned getVoteTimeout() const;

		virtual bool isVoteRunning() const;

		virtual void voteNo();

		virtual void voteYes();


		virtual void update(unsigned p_timeElapsed);


	private:

		typedef std::vector<CL_SharedPtr<RemotePlayer> > TPlayerList;


		/** Initialized state */
		bool m_initialized;

		/** Hostname */
		CL_String m_host;

		/** Port */
		int m_port;

		/** Network client */
		Net::Client *m_client;

		/** Local player */
		Player &m_localPlayer;

		/** Network players */
		TPlayerList m_remotePlayers;

		/** Last iteration id when input was sent */
		int32_t m_lastIterInputSent;

		/** Do have input changed on the last iteration? */
		bool m_inputChanged;

		/** Slots container */
		CL_SlotContainer m_slots;


		// vote system

		bool m_voteRunning;

		CL_String m_voteMessage;

		int m_voteYesCount;

		int m_voteNoCount;

		unsigned m_voteTimeout;


		bool needToSendCarState();

		bool iterationsPeriodReached();

		void sendCarState(const Car &p_car);


		// signal handlers

		void onConnected();

		void onDisconnected();

		void onGoodbye(GoodbyeReason p_reason, const CL_String &p_message);

		void onPlayerJoined(const CL_String &p_name);

		void onPlayerLeaved(const CL_String &p_name);

		void onGameState(const Net::GameState &p_gameState);

		void onCarState(const Net::CarState &p_carState);

		void onRaceStart(const CL_Pointf &p_carPosition, const CL_Angle &p_carRotation);

		void onInputChange(const Car &p_car);

		void onVoteStarted(VoteType p_voteType, const CL_String& p_subject, unsigned p_timeLimitSec);

		void onVoteEnded(VoteResult p_voteResult);

		void onVoteTick(VoteOption p_voteOption);

};

}

