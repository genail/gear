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
#include "logic/race/Progress.h"

namespace Race {

class RaceLogicImpl
{
	public:

		typedef std::vector<Player> TPlayerList;
		typedef std::vector<Player> TConstPlayerList;


		/** The level */
		Level m_level;

		/** All players vector (with local player too) */
		TPlayerList m_playerList;

		/** Progress object */
		Progress m_progress;

		/** Race beginning time. If 0 then race not started yet */
		unsigned m_raceStartTimeMs;

		/** Race end time. If 0 then race not finished yet */
		unsigned m_raceFinishTimeMs;

		/** Laps total */
		int m_lapCount;

		/** Next place to get on finish */
		int m_nextPlace;

		/** Players that finished this race */
		TConstPlayerList m_playersFinished;

		/** Message board to display game messages */
		MessageBoard m_messageBoard;



		RaceLogicImpl() :
			m_level(),
			m_progress(m_level),
			m_raceStartTimeMs(0),
			m_raceFinishTimeMs(0),
			m_lapCount(0),
			m_nextPlace(1)
		{ /* empty */ }


		bool hasPlayerFinished(const Player &p_player) const;

		// update routines

		void updateCarPhysics(unsigned p_timeElapsed);

		void updateCheckpoints();

		void updatePlayersProgress();

		void updateTyreStripes();
};

RaceLogic::RaceLogic() :
	m_impl(new RaceLogicImpl())
{
	display(_("Game loaded"));
}

RaceLogic::~RaceLogic()
{
}

void RaceLogic::update(unsigned p_timeElapsed)
{
	m_impl->updateCarPhysics(p_timeElapsed);
	m_impl->updateCheckpoints();
	m_impl->updatePlayersProgress();
}

void RaceLogicImpl::updateCarPhysics(unsigned p_timeElapsed)
{
	const unsigned carCount = m_level.getCarCount();
	for (unsigned i = 0; i < carCount; ++i) {
		Race::Car &car = m_level.getCar(i);
		car.update(p_timeElapsed);
	}
}

void RaceLogicImpl::updateCheckpoints()
{
//	const int carCount = m_level.getCarCount();
//
//
//	for (int i = 0; i < carCount; ++i) {
//		Car &car = m_level.getCar(i);
//
//		const Checkpoint *currentCheckpoint = car.getCurrentCheckpoint() ? car.getCurrentCheckpoint() : m_level.getTrack().getFirst();
//
//		// find next checkpoint
//		bool movingForward, newLap;
//		const Checkpoint *nextCheckpoint = m_level.getTrack().check(car.getPosition(), currentCheckpoint, &movingForward, &newLap);
//
//		// apply to car
//		if (nextCheckpoint != currentCheckpoint) {
//			car.updateCurrentCheckpoint(nextCheckpoint);
//
//			if (!movingForward) {
//				display(_("Wrong way"));
//			}
//
//			if (newLap) {
//				display(_("New lap"));
//			}
//		}
//
//	}
}

void RaceLogicImpl::updatePlayersProgress()
{
	static const unsigned MILLISECOND = 1;
	static const unsigned CENTISECOND = MILLISECOND * 10;
	static const unsigned SECOND = CENTISECOND * 100;
	static const unsigned MINUTE = SECOND * 60;

	unsigned now = 0, min = 0, sec = 0, centi = 0;

	foreach (Player &player, m_playerList) {

		if (player.getCar().getLap() > m_lapCount && !hasPlayerFinished(player)) {

			// calculate timing
			if (now == 0) {
				now = CL_System::get_time();
				unsigned diff = now - m_raceStartTimeMs;

				min = diff / MINUTE;
				diff -= min * MINUTE;

				sec = diff / SECOND;
				diff -= sec * SECOND;

				centi = diff / CENTISECOND;
			}

			m_messageBoard.addMessage(
					cl_format(
							"Player '%1' has finished at %2 place (%3:%4:%5)",
							player.getName(),
							m_nextPlace++,
							min, sec, centi
					)
			);

			m_playersFinished.push_back(player);
		}
	}
}

const Race::Level &RaceLogic::getLevel() const
{
	return m_impl->m_level;
}

const MessageBoard &RaceLogic::getMessageBoard() const
{
	return m_impl->m_messageBoard;
}

std::vector<CL_String> RaceLogic::getPlayerNames() const
{
	std::vector<CL_String> names;
	names.reserve(m_impl->m_playerList.size());

	foreach(const Player &p, m_impl->m_playerList) {
		names.push_back(p.getName());
	}

	return names;
}

const Player &RaceLogic::getPlayer(const CL_String& p_name) const
{
	foreach (const Player &p, m_impl->m_playerList) {
		if (p.getName() == p_name) {
			return p;
		}
	}

	G_ASSERT(0 && "player doesn't exists");
}

Player &RaceLogic::getPlayer(const CL_String& p_name)
{
	foreach (Player &p, m_impl->m_playerList) {
		if (p.getName() == p_name) {
			return p;
		}
	}

	G_ASSERT(0 && "player doesn't exists");
}

bool RaceLogic::hasPlayer(const CL_String &p_name) const
{
	// FIXME: duplicate
	foreach (const Player &p, m_impl->m_playerList) {
		if (p.getName() == p_name) {
			return true;
		}
	}

	return false;
}

const Player &RaceLogic::getPlayer(const Car& p_car) const
{
	foreach (const Player &p, m_impl->m_playerList) {
		if (p.getCar() == p_car) {
			return p;
		}
	}

	G_ASSERT(0 && "player doesn't exists");
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
	m_impl->m_lapCount = p_lapCount;
	m_impl->m_raceStartTimeMs = p_startTimeMs;

	m_impl->m_nextPlace = 1;
	m_impl->m_raceFinishTimeMs = 0;
	m_impl->m_playersFinished.clear();

	display(_("Get ready..."));
}

bool RaceLogic::isRaceFinished() const
{
	return m_impl->m_raceFinishTimeMs != 0;
}

bool RaceLogic::isRacePending() const
{
	return m_impl->m_raceStartTimeMs > CL_System::get_time();
}

bool RaceLogic::isRaceStarted() const
{
	return m_impl->m_raceStartTimeMs <= CL_System::get_time();
}

int RaceLogic::getRaceLapCount() const
{
	return m_impl->m_lapCount;
}

unsigned RaceLogic::getRaceStartTime() const
{
	return m_impl->m_raceStartTimeMs;
}

unsigned RaceLogic::getRaceFinishTime() const
{
	return m_impl->m_raceFinishTimeMs;
}

Level &RaceLogic::getLevel()
{
	return m_impl->m_level;
}

bool RaceLogicImpl::hasPlayerFinished(const Player &p_player) const
{
	foreach (const Player &p, m_playersFinished) {
		if (p_player == p) {
			return true;
		}
	}

	return false;
}

void RaceLogic::display(const CL_String &p_message)
{
	m_impl->m_messageBoard.addMessage(p_message);
}

void RaceLogic::addPlayer(const Player &p_player)
{
	m_impl->m_playerList.push_back(p_player);
	m_impl->m_progress.addCar(p_player.getCar());
}

const Player &RaceLogic::getPlayer(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < getPlayerCount());
	return m_impl->m_playerList[p_index];
}

Player &RaceLogic::getPlayer(int p_index)
{
	G_ASSERT(p_index >= 0 && p_index < getPlayerCount());
	return m_impl->m_playerList[p_index];
}

int RaceLogic::getPlayerCount() const
{
	return static_cast<signed>(m_impl->m_playerList.size());
}

void RaceLogic::removePlayer(const Player &p_player)
{
	RaceLogicImpl::TPlayerList::iterator itor;
	for (
			itor = m_impl->m_playerList.begin();
			itor != m_impl->m_playerList.end();
			++itor
		) {
		if (p_player == *itor) {
			m_impl->m_progress.removeCar(itor->getCar());
			m_impl->m_playerList.erase(itor);
			break;
		}
	}
}

Progress &RaceLogic::getProgress()
{
	return m_impl->m_progress;
}

} // namespace
