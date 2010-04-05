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

#include "OfflineRaceLogic.h"

#include "common.h"
#include "common/Game.h"
#include "logic/race/Progress.h"
#include "logic/race/level/Level.h"

namespace Race {

class OfflineRaceLogicImpl
{
	public:

		OfflineRaceLogic *m_parent;

		bool m_initialized;


		OfflineRaceLogicImpl(OfflineRaceLogic *p_parent);


		void initialize();

		void addLocalPlayerToGame();

		void setLocalPlayerCarAtStartPosition();


		void destroy();

		void removeLocalPlayerFromGame();

};

OfflineRaceLogic::OfflineRaceLogic(Race::Level *p_level) :
		RaceLogic(p_level),
		m_impl(new OfflineRaceLogicImpl(this))
{
	// empty
}

OfflineRaceLogicImpl::OfflineRaceLogicImpl(OfflineRaceLogic *p_parent) :
		m_parent(p_parent),
		m_initialized(false)
{
	// empty
}

OfflineRaceLogic::~OfflineRaceLogic()
{
	destroy();
}

void OfflineRaceLogic::initialize()
{
	RaceLogic::initialize();
	m_impl->initialize();
}

void OfflineRaceLogicImpl::initialize()
{
	addLocalPlayerToGame();
	setLocalPlayerCarAtStartPosition();
}

void OfflineRaceLogicImpl::addLocalPlayerToGame()
{
	Level &level = m_parent->getLevel();
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();

	m_parent->addPlayer(&player);
	level.addCar(&player.getCar());
}

void OfflineRaceLogicImpl::setLocalPlayerCarAtStartPosition()
{
	Level &level = m_parent->getLevel();
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();

	CL_Pointf carPos;
	CL_Angle carRot;
	level.getStartPosAndRot(1, &carPos, &carRot);

	player.getCar().setPosition(carPos);
	player.getCar().setAngle(carRot);
}

void OfflineRaceLogic::destroy()
{
	m_impl->destroy();
	RaceLogic::destroy();
}

void OfflineRaceLogicImpl::destroy()
{
	removeLocalPlayerFromGame();
}

void OfflineRaceLogicImpl::removeLocalPlayerFromGame()
{
	Level &level = m_parent->getLevel();
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();

	m_parent->removePlayer(player);
	level.removeCar(&player.getCar());
}

}
