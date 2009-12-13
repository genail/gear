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

#include <vector>
#include <ClanLib/core.h>

#include "common.h"
#include "common/votetypes.h"

namespace Net {

class VoteSystem {

	public:

		VoteSystem();

		virtual ~VoteSystem();


		VoteResult getResult() const;

		bool isFinished() const;

		bool isRunning() const;


		bool addVote(VoteOption p_option, int p_voterId);

		void start(VoteType p_type, unsigned p_voterCount, unsigned p_timeLimitMs);


		CALLBACK_0(finished);

	private:

		enum State {
			S_HOLD,
			S_RUNNING,
			S_FINISHED
		};

		unsigned m_voterCount;

		unsigned m_yesCount, m_noCount;

		/** Voting state */
		State m_state;

		/** Set to -1 if result is now known yet */
		int m_result;

		/** Voters that already gave thier vote */
		std::vector<int> m_voters;

		/** The timer */
		CL_Timer m_timer;


		int calculateResult() const;

		bool hasVoter(int p_id) const;

		void onTimerExpired();


};

}

