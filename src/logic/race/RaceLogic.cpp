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

#include "RaceLogic.h"

#include "common/Game.h"
#include "common/Player.h"

namespace Race {

RaceLogic::RaceLogic() :
	m_raceStartTimeMs(0),
	m_raceFinishTimeMs(0),
	m_lapCount(0)
{
	m_messageBoard.addMessage(_("Game loaded"));
}

RaceLogic::~RaceLogic()
{
}

void RaceLogic::update(unsigned p_timeElapsed)
{
	updateCarPhysics(p_timeElapsed);
	updateLevel(p_timeElapsed);
	updatePlayersProgress();
}

void RaceLogic::updateCarPhysics(unsigned p_timeElapsed)
{
	const unsigned carCount = m_level.getCarCount();
	for (unsigned i = 0; i < carCount; ++i) {
		Race::Car &car = m_level.getCar(i);
		car.update(p_timeElapsed);
	}
}

void RaceLogic::updateLevel(unsigned p_timeElapsed)
{
	m_level.update(p_timeElapsed);
}

void RaceLogic::updatePlayersProgress()
{
	Player *player;

	TPlayerMapPair pair;
	foreach (pair, m_playerMap) {
		player = pair.second;

		if (player->getCar().getLap() > m_lapCount && !hasPlayerFinished(player)) {
			cl_log_event(LOG_RACE, "Player '%1' has finished the race", player->getName());

			m_playersFinished.push_back(player);
		}
	}
}

const Race::Level &RaceLogic::getLevel() const
{
	return m_level;
}

const MessageBoard &RaceLogic::getMessageBoard() const
{
	return m_messageBoard;
}

std::vector<CL_String> RaceLogic::getPlayerNames() const
{
	std::vector<CL_String> names;
	TPlayerMapPair pair;

	names.reserve(m_playerMap.size());

	foreach(pair, m_playerMap) {
		names.push_back(pair.first);
	}

	return names;
}

const Player &RaceLogic::getPlayer(const CL_String& p_name) const
{
	assert(m_playerMap.find(p_name) != m_playerMap.end());
	return *m_playerMap.at(p_name);
}

void RaceLogic::callAVote(VoteType p_type, const CL_String &p_subject)
{
	// nothing
}

const CL_String &RaceLogic::getVoteMessage() const
{
	static CL_String def;
	return def;
}

bool RaceLogic::isVoteRunning() const
{
	return false;
}

int RaceLogic::getVoteNoCount() const
{
	return 0;
}

int RaceLogic::getVoteYesCount() const
{
	return 0;
}

unsigned RaceLogic::getVoteTimeout() const
{
	return 0;
}

void RaceLogic::voteNo()
{
	// empty
}

void RaceLogic::voteYes()
{
	// empty
}

void RaceLogic::startRace(int p_lapCount, unsigned p_startTimeMs)
{
	m_lapCount = p_lapCount;
	m_raceStartTimeMs = p_startTimeMs;

	m_raceFinishTimeMs = 0;
	m_playersFinished.clear();
}

bool RaceLogic::isRaceFinished() const
{
	return m_raceFinishTimeMs != 0;
}

bool RaceLogic::isRacePending() const
{
	return m_raceStartTimeMs > CL_System::get_time();
}

bool RaceLogic::isRaceStarted() const
{
	return m_raceStartTimeMs <= CL_System::get_time();
}

unsigned RaceLogic::getRaceStartTime() const
{
	return m_raceStartTimeMs;
}

unsigned RaceLogic::getRaceFinishTime() const
{
	return m_raceFinishTimeMs;
}

bool RaceLogic::hasPlayerFinished(const Player *p_player) const
{
	foreach (const Player *p, m_playersFinished) {
		if (p_player == p) {
			return true;
		}
	}

	return false;
}

} // namespace
