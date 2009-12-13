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

#include "VoteSystem.h"

#include <assert.h>
#include <ClanLib/core.h>

#include "common.h"

/** Yes to No ratio that will result in succeeded voting */
const float PASS_RATIO = 1.0f * (2.0f / 3.0f);
/** No to Yes ratio that will result in failed voting */
const float FAIL_RATIO = 1.0f - PASS_RATIO;

namespace Net {

VoteSystem::VoteSystem()
{
}

VoteSystem::~VoteSystem()
{
}

VoteResult VoteSystem::getResult() const
{
	assert(isFinished());
	return m_result != -1 ? (VoteResult) m_result : VOTE_FAILED;
}

bool VoteSystem::isFinished() const
{
	// finished when result is known
	if (m_result != -1) {
		return true;
	}

	// finished when time has passed
	const unsigned now = CL_System::get_time();

	if (m_endTime <= now) {
		return true;
	}

	// finished when result calculation will succeed
	m_result = calculateResult();

	if (m_result != -1) {
		return true;
	}

	return false;
}


void VoteSystem::addVote(VoteOption p_option, int p_voterId)
{
	assert(hasVoter(p_voterId));

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

	m_voters.push_back(p_voterId);
}

void VoteSystem::start(VoteType p_type, unsigned p_voterCount, unsigned p_timeLimit)
{
	m_voterCount = p_voterCount;
	m_endTime = CL_System::get_time() + p_timeLimit;

	m_yesCount = m_noCount = 0;
	m_result = -1;

	m_voters.clear();
}

int VoteSystem::calculateResult() const
{
	// check if passed
	const float yesToAllRatio = m_yesCount / (float) m_voterCount;

	if (yesToAllRatio >= PASS_RATIO) {
		return (int) VOTE_PASSED;
	}

	// check if failed
	const float noToAllRatio = m_noCount / (float) m_voterCount;

	if (noToAllRatio > FAIL_RATIO) {
		return (int) VOTE_FAILED;
	}

	// not decided yet
	return -1;

}

bool VoteSystem::hasVoter(int p_id) const
{
	foreach (int id, m_voters) {
		if (id == p_id) {
			return true;
		}
	}

	return false;
}

} // namespace
