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

#include <ClanLib/core.h>

#include "common/types.h"

class VoteSystem;

namespace Race
{

enum RaceGameState
{
	/** No action taken. */
	GS_STANDBY,
	/** Race is pending for start */
	GS_PENDING,
	/** Race started and not yet finished */
	GS_RUNNING,
	/** Race finished by local player */
	GS_FINISHED_SINGLE,
	/** Race finished by all players */
	GS_FINISHED_ALL
};

class Level;
class MessageBoard;
class Progress;

class GameLogicImpl;
class GameLogic
{
	public:

		GameLogic();
		virtual ~GameLogic();

		virtual void initialize();
		virtual void destroy();

		virtual void update(unsigned p_timeElapsedMs);

		void setLevel(Level *p_level);
		const Level &getLevel() const;

		int getLapCount() const;

		RaceGameState getRaceGameState() const;
		const Progress &getProgressObject() const;

		VoteSystem &getVoteSystem();
		const VoteSystem &getVoteSystem() const;
		const MessageBoard &getMessageBoard() const;
		virtual void callAVote(VoteType p_voteType, const CL_String &p_subject);

	protected:

		Level &getLevel();
		Progress &getProgressObject();

		void setRaceGameState(RaceGameState p_gameState);
		void setLapCount(int p_lapCount);


	private:

		CL_SharedPtr<GameLogicImpl> m_impl;

		friend class GameLogicImpl;
		friend class BasicGameClientImpl;

};

}

