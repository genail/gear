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

//#define BOOST_TEST_DYN_LINK


#include <unistd.h>
#include <boost/test/unit_test.hpp>

#include "logic/VoteSystem.h"

bool finished;
CL_SlotContainer m_slots;

void onFinished() {
	finished = true;
}

BOOST_AUTO_TEST_SUITE(ServerVoteSystem)

BOOST_AUTO_TEST_CASE(passingVote)
{
	VoteSystem voteSystem;

	finished = false;
	m_slots.connect(voteSystem.sig_finished(), &onFinished);

	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.start(VOTE_RESTART_RACE, 3, 1000);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);


	voteSystem.addVote(VOTE_YES, 1);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_YES, 2);
	BOOST_CHECK_EQUAL(finished, true);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), true);

	BOOST_CHECK_EQUAL(voteSystem.getResult(), VOTE_PASSED);
}

BOOST_AUTO_TEST_CASE(passingVote2)
{
	VoteSystem voteSystem;

	finished = false;
	m_slots.connect(voteSystem.sig_finished(), &onFinished);

	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.start(VOTE_RESTART_RACE, 2, 1000);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);


	voteSystem.addVote(VOTE_YES, 1);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_YES, 2);
	BOOST_CHECK_EQUAL(finished, true);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), true);

	BOOST_CHECK_EQUAL(voteSystem.getResult(), VOTE_PASSED);
}

BOOST_AUTO_TEST_CASE(failingVote)
{
	VoteSystem voteSystem;

	finished = false;
	m_slots.connect(voteSystem.sig_finished(), &onFinished);

	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.start(VOTE_RESTART_RACE, 6, 1000);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_NO, 1);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_NO, 2);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_NO, 3);
	BOOST_CHECK_EQUAL(finished, true);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), true);

	BOOST_CHECK_EQUAL(voteSystem.getResult(), VOTE_FAILED);
}

BOOST_AUTO_TEST_CASE(undecidedVote)
{
	VoteSystem voteSystem;

	finished = false;
	m_slots.connect(voteSystem.sig_finished(), &onFinished);

	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.start(VOTE_RESTART_RACE, 6, 1000);

	voteSystem.addVote(VOTE_YES, 1);
	voteSystem.addVote(VOTE_YES, 2);
	voteSystem.addVote(VOTE_YES, 3);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.addVote(VOTE_NO, 4);
	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);
}


BOOST_AUTO_TEST_CASE(timeLimit)
{
	VoteSystem voteSystem;

	finished = false;
	m_slots.connect(voteSystem.sig_finished(), &onFinished);

	BOOST_CHECK_EQUAL(finished, false);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), false);

	voteSystem.start(VOTE_RESTART_RACE, 6, 50);
	usleep(1000 * 100);
	CL_KeepAlive::process();

	BOOST_CHECK_EQUAL(finished, true);
	BOOST_CHECK_EQUAL(voteSystem.isFinished(), true);
	BOOST_CHECK_EQUAL(voteSystem.getResult(), VOTE_FAILED);
}

BOOST_AUTO_TEST_SUITE_END()
