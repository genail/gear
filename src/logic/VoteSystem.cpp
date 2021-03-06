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

#include "VoteSystem.h"

#include <ClanLib/core.h>

#include "common.h"
#include "common/gassert.h"

class VoteSystemImpl
{
	public:

		enum State {
			S_HOLD,
			S_RUNNING,
			S_FINISHED
		};


		VoteSystem *m_parent;

		VoteType m_type;
		CL_String m_subject;

		int m_voterCount;
		int m_yesCount, m_noCount;
		int m_passCount;

		/** Voting state */
		State m_state;

		/** Set to -1 if result is now known yet */
		int m_result;

		/** Voters that already gave thier vote */
		std::vector<int> m_voters;

		CL_Timer m_timer;
		unsigned m_finishTime;

		SIG_IMPL(VoteSystem, expired);


		VoteSystemImpl(VoteSystem *p_parent);
		~VoteSystemImpl() { /* empty */ }

		void start(VoteType p_type, int p_voterCount, int p_timeLimitMs);
		bool addVote(VoteOption p_option, int p_voterId = -1);

		int calculateResult() const;
		bool hasVoter(int p_id) const;
		void onTimerExpired();
};

SIG_CPP(VoteSystem, expired);

VoteSystem::VoteSystem() :
		m_impl(new VoteSystemImpl(this))
{
	// empty
}

VoteSystemImpl::VoteSystemImpl(VoteSystem *p_parent) :
		m_parent(p_parent),
		m_state(S_HOLD),
		m_voterCount(0),
		m_yesCount(0),
		m_noCount(0),
		m_finishTime(0)
{
	m_timer.func_expired().set(this, &VoteSystemImpl::onTimerExpired);
}

VoteSystem::~VoteSystem()
{
	// empty
}

void VoteSystem::start(VoteType p_type, int p_voterCount, int p_timeLimitMs)
{
	m_impl->start(p_type, p_voterCount, p_timeLimitMs);
}

void VoteSystemImpl::start(VoteType p_type, int p_voterCount, int p_timeLimitMs)
{
	m_type = p_type;
	m_voterCount = p_voterCount;

	m_yesCount = m_noCount = 0;
	m_result = -1;
	m_state = S_RUNNING;

	m_voters.clear();
	m_timer.start(p_timeLimitMs, false);

	m_finishTime = CL_System::get_time() + p_timeLimitMs;

	m_passCount = static_cast<int>(floor(p_voterCount / 2.0f) + 1);
}

bool VoteSystem::addVote(VoteOption p_option, int p_voterId)
{
	return m_impl->addVote(p_option, p_voterId);
}

bool VoteSystemImpl::addVote(VoteOption p_option, int p_voterId)
{
	if(!hasVoter(p_voterId)) {

		switch (p_option) {
			case VOTE_YES:
				++m_yesCount;
				break;
			case VOTE_NO:
				++m_noCount;
				break;
			default:
				assert(0 && "unknown vote option");
		}

		if (p_voterId != -1) {
			m_voters.push_back(p_voterId);
		}

		// calculate result
		m_result = calculateResult();

		if (m_result != -1) {
			// finished, set state and call signal
			m_state = S_FINISHED;
			m_timer.stop();
		}

		return true;

	} else {
		cl_log_event(LOG_WARN, "multiple vote attempt from %1", p_voterId);
		return false;
	}
}

VoteResult VoteSystem::getResult() const
{
	G_ASSERT(isFinished());
	return m_impl->m_result != -1 ? (VoteResult) m_impl->m_result : VOTE_FAILED;
}

bool VoteSystem::isFinished() const
{
	return m_impl->m_state == VoteSystemImpl::S_FINISHED;
}

bool VoteSystem::isRunning() const
{
	return m_impl->m_state == VoteSystemImpl::S_RUNNING;
}

int VoteSystem::getYesCount() const
{
	return m_impl->m_yesCount;
}

int VoteSystem::getNoCount() const
{
	return m_impl->m_noCount;
}

int VoteSystemImpl::calculateResult() const
{
	if (m_yesCount >= m_passCount) {
		return static_cast<int>(VOTE_PASSED);
	} else if (m_noCount > m_voterCount - m_passCount) {
		return static_cast<int>(VOTE_FAILED);
	} else {
		return -1;
	}
}

bool VoteSystemImpl::hasVoter(int p_id) const
{
	if (p_id == -1) {
		return false;
	}

	foreach (int id, m_voters) {
		if (id == p_id) {
			return true;
		}
	}

	return false;
}

void VoteSystemImpl::onTimerExpired()
{
	if (m_state == S_RUNNING) {
		m_state = S_FINISHED;
		INVOKE_0(expired);
	}
}

unsigned VoteSystem::getFinishTime() const
{
	return m_impl->m_finishTime;
}

VoteType VoteSystem::getType() const
{
	return m_impl->m_type;
}

void VoteSystem::setVoteSubject(const CL_String &p_subject)
{
	m_impl->m_subject = p_subject;
}

const CL_String &VoteSystem::getVoteSubject() const
{
	return m_impl->m_subject;
}
