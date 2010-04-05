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

#include "RaceScene.h"

#include <assert.h>
#include <stdlib.h>

#include "common.h"
#include "common/Game.h"
#include "common/Properties.h"
#include "common/Units.h"
#include "logic/race/Block.h"
#include "logic/race/OfflineRaceLogic.h"
#include "logic/race/OnlineRaceLogic.h"
#include "network/events.h"
#include "network/packets/CarState.h"
#include "network/client/Client.h"
#include "gfx/race/RaceGraphics.h"
#include "gfx/race/level/Bound.h"
#include "gfx/race/ui/ScoreTable.h"
#include "debug/RaceSceneKeyBindings.h"


RaceScene::RaceScene(CL_GUIComponent &p_parent) :
	DirectScene(p_parent),
	m_logic(NULL),
	m_graphics(NULL),
	m_initialized(false),
	m_inputLock(false),
	m_turnLeft(false),
	m_turnRight(false),
	m_gameMenu(*this),
	m_gameMenuController(&m_logic, &m_gameMenu)
{
	// empty
}

RaceScene::~RaceScene() {
}

void RaceScene::initializeOffline(Race::Level *p_level)
{
	if (!m_initialized) {
		m_logic = new Race::OfflineRaceLogic(p_level);
		initCommon();
	}
}

void RaceScene::initializeOnline()
{
	if (!m_initialized) {
		m_logic = new Race::OnlineRaceLogic();
		initCommon();
	}

}

void RaceScene::initCommon()
{
	m_logic->initialize();
	m_graphics = new Gfx::RaceGraphics(m_logic);

	// bind keys
	if (Properties::getBool(CG_USE_WASD, false)) {
		// use WASD instead of arrows
		m_keySteerLeft = CL_KEY_A;
		m_keySteerRight = CL_KEY_D;
		m_keyAccel = CL_KEY_W;
		m_keyBrake = CL_KEY_S;
	} else {
		// use arrows
		m_keySteerLeft = CL_KEY_LEFT;
		m_keySteerRight = CL_KEY_RIGHT;
		m_keyAccel = CL_KEY_UP;
		m_keyBrake = CL_KEY_DOWN;
	}

	m_initialized = true;
}

void RaceScene::destroy()
{
	if (m_initialized) {
		delete m_logic;
		m_logic = NULL;

		delete m_graphics;
		m_graphics = NULL;

		setLoaded(false);
		m_initialized = false;
	}
}

void RaceScene::poped()
{
	destroy();
}

void RaceScene::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);

	m_graphics->draw(p_gc);
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);

	cl_log_event(LOG_DEBUG, "RaceScene::load()");

	m_graphics->load(p_gc);

#if !defined(RACE_SCENE_ONLY)
	DirectScene::load(p_gc);
#endif // !RACE_SCENE_ONLY

}



void RaceScene::update(unsigned p_timeElapsed)
{
	G_ASSERT(m_initialized);

	if (m_logic)
	m_logic->update(p_timeElapsed);
	m_graphics->update(p_timeElapsed);
}



void RaceScene::inputPressed(const CL_InputEvent &p_event)
{
	if (m_initialized) {
		handleInput(Pressed, p_event);
	}
}

void RaceScene::inputReleased(const CL_InputEvent &p_event)
{
	if (m_initialized) {
		handleInput(Released, p_event);
	}
}

void RaceScene::handleInput(InputState p_state, const CL_InputEvent& p_event)
{
	G_ASSERT(m_initialized);

	Race::Car &car = Game::getInstance().getPlayer().getCar();
	bool pressed;

	switch (p_state) {
		case Pressed:
			pressed = true;
			break;
		case Released:
			pressed = false;
			break;
		default:
			assert(0 && "unknown input state");
	}

	if (p_event.id == m_keySteerLeft) {
		m_turnLeft = pressed;
	} else if (p_event.id == m_keySteerRight) {
		m_turnRight = pressed;
	} else if (p_event.id == m_keyAccel) {
		car.setAcceleration(pressed);
	} else if (p_event.id == m_keyBrake) {
		car.setBrake(pressed);
	} else {

		switch (p_event.id) {
			case CL_KEY_F1:
				if (pressed && m_logic->isVoteRunning()) {
					m_logic->voteYes();
				}
				break;
			case CL_KEY_F2:
				if (pressed && m_logic->isVoteRunning()) {
					m_logic->voteNo();
				}
			case CL_KEY_TAB:
				if (pressed) {
					m_graphics->getUi().getScoreTable().restartAnimation();
				}
				break;
#if !defined(NDEBUG)
			case CL_KEY_R:
				if (!pressed) {
					std::cout << "<ref>\n\t<position x=\""
							<< Units::toWorld(car.getPosition().x)
							<< "\" y=\""
							<< Units::toWorld(car.getPosition().y)
							<< "\" />\n</ref>" << std::endl;
				}
#endif // NDEBUG
			default:
				break;
		}
	}

	// handle quit request
	if (pressed && p_event.id == CL_KEY_ESCAPE) {

		if (!m_gameMenu.isVisible()) {
			m_gameMenu.setVisible(true);
		} else {
			m_gameMenu.setVisible(false);
		}
	}

	updateCarTurn();

#if !defined(NDEBUG)
	// debug key bindings
	Dbg::RaceSceneKeyBindings::handleInput(pressed, p_event);
#endif // !NDEBUG
}

void RaceScene::updateCarTurn()
{
	G_ASSERT(m_initialized);

	Race::Car &car = Game::getInstance().getPlayer().getCar();
	car.setTurn((int) -m_turnLeft + (int) m_turnRight);
}

void RaceScene::onInputLock()
{
	G_ASSERT(m_initialized);

	m_inputLock = true;
}

const Race::RaceLogic *RaceScene::getLogic() const
{
	return m_logic;
}

