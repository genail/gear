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

#include <map>
#include <list>
#include <vector>

#include <ClanLib/core.h>

#include "common.h"
#include "common/votetypes.h"
#include "logic/race/level/Level.h"
#include "logic/race/MessageBoard.h"
#include "logic/race/TyreStripes.h"

class Player;

namespace Race {

class RaceLogicImpl;

class RaceLogic {

	public:

		typedef std::list<Player*> TPlayerList;

		RaceLogic();

		virtual ~RaceLogic();


		virtual void initialize() = 0;

		virtual void destroy() = 0;



		const Race::Level &getLevel() const;

		const MessageBoard &getMessageBoard() const;

		std::vector<CL_String> getPlayerNames() const;

		unsigned getRaceFinishTime() const;

		int getRaceLapCount() const;

		unsigned getRaceStartTime() const;

		const TyreStripes &getTyreStripes() const;

		/** @return Message that will be displayed on screen when vote is running. */
		virtual const CL_String &getVoteMessage() const;

		virtual int getVoteNoCount() const;

		/** @return Time in milliseconds that is deadline to this vote */
		virtual unsigned getVoteTimeout() const;

		virtual int getVoteYesCount() const;

		virtual bool isVoteRunning() const;



		virtual void callAVote(VoteType p_type, const CL_String &p_subject="");

		bool hasPlayer(const CL_String &p_name) const;

		bool isRaceFinished() const;

		bool isRacePending() const;

		bool isRaceStarted() const;

		const Player &getPlayer(int p_index) const;

		const Player &getPlayer(const CL_String& p_name) const;

		const Player &getPlayer(const Car& p_car) const;

		int getPlayerCount() const;

		/**
		 * Begins the race at <code>p_startTimeMs</code>.
		 *
		 * @p_startTimeMs Race start time in ms from system boot.
		 */
		void startRace(int p_lapCount, unsigned p_startTimeMs);

		virtual void update(unsigned p_timeElapsed);

		virtual void voteNo();

		virtual void voteYes();


	protected:

		Level &getLevel();

		void addPlayer(const Player &p_player);

		void display(const CL_String &p_message);

		Player &getPlayer(int p_index);

		Player &getPlayer(const CL_String& p_name);

		void removePlayer(const Player &p_player);

	private:

		CL_SharedPtr<RaceLogicImpl> m_impl;

};

} // namespace

