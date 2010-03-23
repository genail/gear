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

#include "RaceLogic.h"

#include "common/Collections.h"
#include "common/Game.h"
#include "common/Player.h"
#include "logic/race/Progress.h"
#include "logic/race/level/Object.h"

namespace Race {

class RaceLogicImpl
{
	public:

		typedef std::vector<Player*> TPlayerList;
		typedef std::vector<const Player*> TConstPlayerList;


		// signals
		SIG_IMPL(RaceLogic, stateChanged);


		/** The level */
		Level m_level;

		/** All players vector (with local player too) */
		TPlayerList m_playerList;

		/**
		 * Players that are registered to ongoing race. During the race
		 * no new players can be registered, but already registered players
		 * can leave.
		 */
		TConstPlayerList m_regPlayers;

		/** Players that finished this race */
		TConstPlayerList m_playersFinished;

		/** Progress object */
		Progress m_progress;

		/** Race beginning time. If 0 then race not started or pending yet. */
		unsigned m_raceStartTimeMs;

		/** Race end time. If 0 then race not finished yet */
		unsigned m_raceFinishTimeMs;

		/** Laps total */
		int m_lapCount;

		/** Race state */
		RaceState m_state;

		/** Message board to display game messages */
		MessageBoard m_messageBoard;

		bool m_collisionFlag;



		RaceLogicImpl(const Race::Level &p_level) :
			m_level(p_level),
			m_progress(m_level),
			m_raceStartTimeMs(0),
			m_raceFinishTimeMs(0),
			m_lapCount(0),
			m_state(S_STANDBY),
			m_collisionFlag(false)
		{ /* empty */ }


		bool hasPlayerFinished(const Player &p_player) const;

		bool isLocalPlayerCar(const Race::Car &car);


		void updateState();

		void updateCarCollisions(Race::Car &p_car);

		void updateCollisionWithObject(
				Race::Car &p_car,
				const CL_CollisionOutline &p_carOutline,
				const Race::Object &p_object
		);

		void updateCollisions();

		void updateCarPhysics(unsigned p_timeElapsed);

		void updatePlayersProgress();

		void updateTyreStripes();

};

SIG_CPP(RaceLogic, stateChanged);

RaceLogic::RaceLogic() :
	m_impl(new RaceLogicImpl(Race::Level()))
{
	display(_("Game loaded"));
}

RaceLogic::RaceLogic(const Race::Level &p_level) :
	m_impl(new RaceLogicImpl(p_level))
{
	display(_("Game loaded"));
}

RaceLogic::~RaceLogic()
{
	// empty
}

void RaceLogic::update(unsigned p_timeElapsed)
{
	m_impl->updateState();
	m_impl->updateCarPhysics(p_timeElapsed);
	m_impl->updateCollisions();
	m_impl->updatePlayersProgress();
}

void RaceLogicImpl::updateState()
{
	if (m_state == S_PENDING) {
		if (m_raceStartTimeMs <= CL_System::get_time()) {
			m_state = S_RUNNING;
			INVOKE_2(stateChanged, S_PENDING, S_RUNNING);
		}
	}
}

void RaceLogicImpl::updateCollisions()
{
	const int carCount = m_level.getCarCount();
	m_collisionFlag = false;

	for (int carIdx = 0; carIdx < carCount; ++carIdx) {
		Race::Car &car = m_level.getCar(carIdx);
		updateCarCollisions(car);
	}
}

void RaceLogicImpl::updateCarCollisions(Race::Car &p_car)
{
	const CL_CollisionOutline carOutline = p_car.getCollisionOutline();
	const int objCount = m_level.getObjectCount();

	for (int objIdx = 0; objIdx < objCount; ++objIdx) {
		const Race::Object &obj = m_level.getObject(objIdx);
		updateCollisionWithObject(p_car, carOutline, obj);
	}
}

void RaceLogicImpl::updateCollisionWithObject(
		Race::Car &p_car,
		const CL_CollisionOutline &p_carOutline,
		const Race::Object &p_object
)
{
	const std::vector<CL_CollidingContours> &cont =
			p_object.collide(p_carOutline);

	// if there are collisions then proceed the segments
	const int contCount = cont.size();
	for (int contIdx = 0; contIdx < contCount; ++contIdx) {

		const CL_CollidingContours &cc = cont[contIdx];

		const int ccCount = static_cast<signed>(cc.points.size());
		for (int ccIdx = 0; ccIdx < ccCount; ++ccIdx) {
			const CL_CollisionPoint &pt = cc.points[ccIdx];

			const std::vector<CL_Pointf> &c1pts =
					cc.contour1->get_points();

			CL_LineSegment2f seg(
					c1pts[pt.contour1_line_start],
					c1pts[pt.contour1_line_end]
			);

			p_car.applyCollision(seg);

			if (isLocalPlayerCar(p_car)) {
				m_collisionFlag = true;
			}

		}

	}
}

bool RaceLogicImpl::isLocalPlayerCar(const Race::Car &car)
{
	Race::Car &localPlayerCar = Game::getInstance().getPlayer().getCar();
	return &localPlayerCar == &car;
}

void RaceLogicImpl::updateCarPhysics(unsigned p_timeElapsed)
{
	const int carCount = m_level.getCarCount();
	for (int i = 0; i < carCount; ++i) {
		Race::Car &car = m_level.getCar(i);
		car.update(p_timeElapsed);
	}
}

void RaceLogicImpl::updatePlayersProgress()
{
	if (m_state != S_RUNNING && m_state != S_FINISHED_SINGLE) {
		// do not do anything is race is not running
		return;
	}


	foreach (const Player *player, m_playerList) {

		const int lapNum = m_progress.getLapNumber(player->getCar());
		if (lapNum > m_lapCount && !hasPlayerFinished(*player)) {

			m_playersFinished.push_back(player);

			if (player == &Game::getInstance().getPlayer()) {
				// this is local player

				// change the state
				RaceState oldState = m_state;
				m_state = S_FINISHED_SINGLE;
				INVOKE_2(stateChanged, oldState, m_state);

				cl_log_event(LOG_DEBUG, "Local player finished the race");
			}
		}
	}

	if (m_playersFinished.size() == m_regPlayers.size()) {
		// all players that are in race reached the finish line

		// change the state
		RaceState oldState = m_state;
		m_state = S_FINISHED_ALL;
		INVOKE_2(stateChanged, oldState, m_state);

		cl_log_event(LOG_DEBUG, "All players finished the race");
	}

	// update overall progress
	m_progress.update();
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

	foreach(const Player *p, m_impl->m_playerList) {
		names.push_back(p->getName());
	}

	return names;
}

const Player &RaceLogic::getPlayer(const CL_String& p_name) const
{
	foreach (const Player *p, m_impl->m_playerList) {
		if (p->getName() == p_name) {
			return *p;
		}
	}

	G_ASSERT(0 && "player doesn't exists");
}

Player &RaceLogic::getPlayer(const CL_String& p_name)
{
	foreach (Player *p, m_impl->m_playerList) {
		if (p->getName() == p_name) {
			return *p;
		}
	}

	G_ASSERT(0 && "player doesn't exists");
}

bool RaceLogic::hasPlayer(const CL_String &p_name) const
{
	// FIXME: duplicate
	foreach (const Player *p, m_impl->m_playerList) {
		if (p->getName() == p_name) {
			return true;
		}
	}

	return false;
}

const Player &RaceLogic::getPlayer(const Car& p_car) const
{
	foreach (const Player *p, m_impl->m_playerList) {
		if (&p->getCar() == &p_car) {
			return *p;
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

	m_impl->m_raceFinishTimeMs = 0;
	m_impl->m_playersFinished.clear();

	// register current players to race
	m_impl->m_regPlayers.clear();

	foreach (Player *p, m_impl->m_playerList) {
		m_impl->m_regPlayers.push_back(p);
	}


	RaceState oldState = m_impl->m_state;
	m_impl->m_state = S_PENDING;
	m_impl->INVOKE_2(stateChanged, oldState, m_impl->m_state);

	display(_("Get ready..."));
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
	foreach (const Player *p, m_playersFinished) {
		if (&p_player == p) {
			return true;
		}
	}

	return false;
}

void RaceLogic::display(const CL_String &p_message)
{
	m_impl->m_messageBoard.addMessage(p_message);
}

void RaceLogic::addPlayer(Player *p_player)
{
	m_impl->m_playerList.push_back(p_player);
	m_impl->m_progress.addCar(p_player->getCar());
}

void RaceLogic::removePlayer(const Player &p_player)
{
	// remove from main player list
	const bool found = Collections::remove(m_impl->m_playerList, &p_player);

	if (found) {
		m_impl->m_progress.removeCar(p_player.getCar());
	}

	// remove from registered if presend
	Collections::remove(m_impl->m_regPlayers, &p_player);

	G_ASSERT(found && "player not found");
}

const Player &RaceLogic::getPlayer(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < getPlayerCount());
	return *m_impl->m_playerList[p_index];
}

Player &RaceLogic::getPlayer(int p_index)
{
	G_ASSERT(p_index >= 0 && p_index < getPlayerCount());
	return *m_impl->m_playerList[p_index];
}

int RaceLogic::getPlayerCount() const
{
	return static_cast<signed>(m_impl->m_playerList.size());
}

Progress &RaceLogic::getProgress()
{
	return m_impl->m_progress;
}

const Progress &RaceLogic::getProgress() const
{
	return m_impl->m_progress;
}

RaceState RaceLogic::getRaceState() const
{
	return m_impl->m_state;
}

bool RaceLogic::localCarCollisionInThisIteration() const
{
	return m_impl->m_collisionFlag;
}

} // namespace
