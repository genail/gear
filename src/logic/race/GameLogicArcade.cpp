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

#include "GameLogicArcade.h"

#include "common/gassert.h"
#include "common/Game.h"
#include "logic/race/Car.h"
#include "logic/race/Progress.h"
#include "logic/race/level/Level.h"

namespace Race
{

class GameLogicArcadeImpl
{
	public:

		GameLogicArcade *m_parent;
		bool m_initialized;


		GameLogicArcadeImpl(GameLogicArcade *p_parent);
		~GameLogicArcadeImpl() {}
		
		void initialize();
		void insertPlayerCar();
		void putCarAtFirstStartPosition();
		
		void destroy();
		void removePlayerCar();
// 		
};

GameLogicArcade::GameLogicArcade() :
		m_impl(new GameLogicArcadeImpl(this))
{
	// empty
}

GameLogicArcadeImpl::GameLogicArcadeImpl(GameLogicArcade *p_parent) :
		m_parent(p_parent),
		m_initialized(false)
{
	// empty
}

GameLogicArcade::~GameLogicArcade()
{
	// empty
}

void GameLogicArcade::initialize()
{
	GameLogic::initialize();
	m_impl->initialize();
}

void GameLogicArcadeImpl::initialize()
{
	G_ASSERT(!m_initialized);

	insertPlayerCar();

	m_initialized = true;
}

void GameLogicArcadeImpl::insertPlayerCar()
{
	Level &level = m_parent->getLevel();
	Progress &progress = m_parent->getProgressObject();

	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Car &car = player.getCar();

	level.addCar(&car);
	progress.addCar(&car);

	putCarAtFirstStartPosition();
}

void GameLogicArcadeImpl::putCarAtFirstStartPosition()
{
	Level &level = m_parent->getLevel();
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Car &car = player.getCar();

	CL_Pointf startPosition;
	CL_Angle startAngle;

	level.getStartPosAndRot(1, &startPosition, &startAngle);
	car.setPosition(startPosition);
	car.setAngle(startAngle);
}

void GameLogicArcade::destroy()
{
	GameLogic::destroy();
	m_impl->destroy();
}

void GameLogicArcadeImpl::destroy()
{
	G_ASSERT(m_initialized);
	removePlayerCar();
	m_initialized = false;
}

void GameLogicArcadeImpl::removePlayerCar()
{
	Level &level = m_parent->getLevel();
	Progress &progress = m_parent->getProgressObject();

	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Car &car = player.getCar();

	level.removeCar(&car);
	progress.removeCar(&car);
}

}
