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

#include "GameLogic.h"

#include "common/gassert.h"
#include "common/Game.h"
#include "logic/VoteSystem.h"
#include "logic/race/Car.h"
#include "logic/race/MessageBoard.h"
#include "logic/race/Progress.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Object.h"

namespace Race
{

class GameLogicImpl
{
	public:

		GameLogic *m_parent;

		bool m_initialized;

		Level *m_level;
		int m_lapCount;

		RaceGameState m_gameState;

		CL_SharedPtr<Progress> m_progress;

		VoteSystem m_voteSystem;
		MessageBoard m_messageBoard;


		GameLogicImpl(GameLogic *p_parent);
		~GameLogicImpl();

		void initialize();
		void destroy();

		void update(unsigned p_timeElapsedMs);
		void updateCarsPhysics(unsigned p_timeElapsedMs);
		void updateCollisions();
		void updateCarCollisions(Race::Car &p_car);
		void updateCollisionWithObject(
				Race::Car &p_car,
				const CL_CollisionOutline &p_carOutline,
				const Race::Object &p_object
		);

		void setLevel(Level *p_level);
		const Level &getLevel() const;

		bool hasLastLapTime() const;
		int getLastLapTime() const;
		bool hasBestLapTime() const;
		int getBestLapTime() const;
		int getCurrentLapTime() const;

};

GameLogic::GameLogic() :
		m_impl(new GameLogicImpl(this))
{
	// empty
}

GameLogicImpl::GameLogicImpl(GameLogic *p_parent) :
		m_parent(p_parent),
		m_initialized(false),
		m_level(NULL),
		m_lapCount(0),
		m_gameState(GS_STANDBY)
{
	// empty
}

GameLogic::~GameLogic()
{
	// empty
}

GameLogicImpl::~GameLogicImpl()
{
	G_ASSERT(!m_initialized && "not destroyed properly");
}

void GameLogic::initialize()
{
	m_impl->initialize();
}

void GameLogicImpl::initialize()
{
	G_ASSERT(!m_initialized);
	G_ASSERT(m_level != NULL && "level not set");

	m_progress->initialize();

	m_initialized = true;
}

void GameLogic::destroy()
{
	m_impl->destroy();
}

void GameLogicImpl::destroy()
{
	G_ASSERT(m_initialized);

	m_progress->destroy();

	m_initialized = false;
}

void GameLogic::update(unsigned p_timeElapsedMs)
{
	m_impl->update(p_timeElapsedMs);
}

void GameLogicImpl::update(unsigned p_timeElapsedMs)
{
	updateCarsPhysics(p_timeElapsedMs);
	updateCollisions();

	m_progress->update();
}

void GameLogicImpl::updateCarsPhysics(unsigned p_timeElapsedMs)
{
	const int carCount = m_level->getCarCount();

	for (int i = 0; i < carCount; ++i) {
		Race::Car &car = m_level->getCar(i);
		car.update(p_timeElapsedMs);
	}
}

void GameLogicImpl::updateCollisions()
{

	const int carCount = m_level->getCarCount();

	for (int i = 0; i < carCount; ++i) {
		Race::Car &car = m_level->getCar(i);
		updateCarCollisions(car);
	}
}

void GameLogicImpl::updateCarCollisions(Race::Car &p_car)
{
	const CL_CollisionOutline carOutline = p_car.getCollisionOutline();
	const int objCount = m_level->getObjectCount();

	for (int i = 0; i < objCount; ++i) {
		const Race::Object &obj = m_level->getObject(i);
		updateCollisionWithObject(p_car, carOutline, obj);
	}
}

void GameLogicImpl::updateCollisionWithObject(
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
		}

	}
}

bool GameLogic::hasLastLapTime() const
{
	return m_impl->hasLastLapTime();
}

bool GameLogicImpl::hasLastLapTime() const
{
	return getLastLapTime() != 0;
}

int GameLogic::getLastLapTime() const
{
	return m_impl->getLastLapTime();
}

int GameLogicImpl::getLastLapTime() const
{
	Game &game = Game::getInstance();
	const Car &car = game.getPlayerCar();
	const int lapN = m_progress->getLapNumber(car);

	if (lapN > 1) {
		return m_progress->getLapTime(car, lapN - 1);
	} else {
		return 0;
	}
}

bool GameLogic::hasBestLapTime() const
{
	return m_impl->hasBestLapTime();
}

bool GameLogicImpl::hasBestLapTime() const
{
	return getBestLapTime() != 0;
}

int GameLogic::getBestLapTime() const
{
	return m_impl->getBestLapTime();
}

int GameLogicImpl::getBestLapTime() const
{
	Game &game = Game::getInstance();
	const Car &car = game.getPlayerCar();
	const int lapN = m_progress->getLapNumber(car);

	unsigned best = 0, nbest = 0;

	for (int i = 1; i < lapN; ++i) {
		if (i == 1) {
			best = m_progress->getLapTime(car, i);
		} else {
			nbest = m_progress->getLapTime(car, i);
			if (nbest < best) {
				best = nbest;
			}
		}
	}

	return best;
}

int GameLogic::getCurrentLapTime() const
{
	return m_impl->getCurrentLapTime();
}

int GameLogicImpl::getCurrentLapTime() const
{
	if (m_gameState == GS_RUNNING) {
		Game &game = Game::getInstance();
		const Car &car = game.getPlayerCar();
		const int lapN = m_progress->getLapNumber(car);

		return m_progress->getLapTime(car, lapN);
	} else {
		return 0;
	}
}

void GameLogic::setLevel(Level *p_level)
{
	m_impl->setLevel(p_level);
}

void GameLogicImpl::setLevel(Level *p_level)
{
	G_ASSERT(!m_initialized);
	m_level = p_level;
	m_progress = CL_SharedPtr<Progress>(new Progress(m_level));
}

const Level &GameLogic::getLevel() const
{
	return m_impl->getLevel();
}

const Level &GameLogicImpl::getLevel() const
{
	G_ASSERT(m_initialized);
	return *m_level;
}

Level &GameLogic::getLevel()
{
	return *m_impl->m_level;
}

void GameLogic::setRaceGameState(RaceGameState p_gameState)
{
	m_impl->m_gameState = p_gameState;
}

RaceGameState GameLogic::getRaceGameState() const
{
	return m_impl->m_gameState;
}

Progress &GameLogic::getProgressObject()
{
	G_ASSERT(m_impl->m_initialized);
	return *m_impl->m_progress.get();
}

const Progress &GameLogic::getProgressObject() const
{
	G_ASSERT(m_impl->m_initialized);
	return *m_impl->m_progress.get();
}

VoteSystem &GameLogic::getVoteSystem()
{
	return m_impl->m_voteSystem;
}

const VoteSystem &GameLogic::getVoteSystem() const
{
	return m_impl->m_voteSystem;
}

const MessageBoard &GameLogic::getMessageBoard() const
{
	return m_impl->m_messageBoard;
}

void GameLogic::setLapCount(int p_lapCount)
{
	m_impl->m_lapCount = p_lapCount;
}

int GameLogic::getLapCount() const
{
	return m_impl->m_lapCount;
}

void GameLogic::callAVote(VoteType p_voteType, const CL_String &p_subject)
{
	G_ASSERT(0 && "not supported by this class");
}

void GameLogic::voteYes()
{
	G_ASSERT(0 && "not supported by this class");
}
void GameLogic::voteNo()
{
	G_ASSERT(0 && "not supported by this class");
}

}
