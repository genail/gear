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

class OnlineRaceLogicImpl;

class OnlineRaceLogic: public Race::RaceLogic {

	public:

		OnlineRaceLogic();

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

		CL_SharedPtr<OnlineRaceLogicImpl> m_impl;

		friend class OnlineRaceLogicImpl;
};

}

