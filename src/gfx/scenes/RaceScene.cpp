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
#include "controllers/GameMenuController.h"
#include "logic/race/Block.h"
#include "logic/race/OfflineRaceLogic.h"
#include "logic/race/OnlineRaceLogic.h"
#include "logic/race/RaceLogic.h"
#include "logic/race/ScoreTable.h"
#include "logic/race/TimeTrailRaceLogic.h"
#include "network/events.h"
#include "network/packets/CarState.h"
#include "network/client/Client.h"
#include "gfx/race/RaceGraphics.h"
#include "gfx/race/level/Bound.h"
#include "gfx/race/ui/GameMenu.h"
#include "gfx/race/ui/ScoreTable.h"
#include "debug/RaceSceneKeyBindings.h"

class RaceSceneImpl
{
	public:

		enum InputState {
			Pressed,
			Released
		};


		RaceScene *m_parent;

		/** Logic subsystem */
		Race::RaceLogic *m_logic;

		/** Graphics subsystem */
		Gfx::RaceGraphics *m_graphics;



		/** The score table */
		ScoreTable m_scoreTable;

		/** Race start time */
		unsigned m_raceStartTime;

		/** If this race is initialized */
		bool m_initialized;

		// input

		/** Set to true if user interaction should be locked */
		bool m_inputLock;

		/** Keys down */
		bool m_turnLeft, m_turnRight;

		// display

		/** Last drift car position. If null, then no drift was doing last time. */
		CL_Pointf m_lastDriftPoint;

		// The game menu

		/** Game menu */
		Gfx::GameMenu m_gameMenu;

		/** Game menu controller */
		/** The controller */
		GameMenuController m_gameMenuController;


		// key bindings
		int m_keySteerLeft, m_keySteerRight;
		int m_keyAccel, m_keyBrake;


		// other

		/** The slots container */
		CL_SlotContainer m_slots;


		RaceSceneImpl(RaceScene *p_parent);
		~RaceSceneImpl() {}

		void initializeOffline(Race::Level *p_level);
		void initializeOnline(TGameMode p_gameMode);
		void destroy();

		void load(CL_GraphicContext &p_gc);
		void draw(CL_GraphicContext &p_gc);
		void update(unsigned p_timeElapsed);

		void inputPressed(const CL_InputEvent &p_event);
		void inputReleased(const CL_InputEvent &p_event);

		// input

		void grabInput();

		void handleInput(InputState p_state, const CL_InputEvent& p_event);

		void updateCarTurn();


		// event handlers

		void onInputLock();

		void onRaceStateChanged(int p_lapsNum);


		// other
		void initCommon();
};

RaceScene::RaceScene(CL_GUIComponent &p_parent) :
		DirectScene(p_parent),
		m_impl(new RaceSceneImpl(this))
{
	// empty
}

RaceSceneImpl::RaceSceneImpl(RaceScene *p_parent) :
		m_parent(p_parent),
		m_logic(NULL),
		m_graphics(NULL),
		m_initialized(false),
		m_inputLock(false),
		m_turnLeft(false),
		m_turnRight(false),
		m_gameMenu(*p_parent),
		m_gameMenuController(&m_logic, &m_gameMenu)
{
	// empty
}

RaceScene::~RaceScene()
{
	// empty
}

void RaceScene::initializeOffline(Race::Level *p_level)
{
	m_impl->initializeOffline(p_level);
}

void RaceSceneImpl::initializeOffline(Race::Level *p_level)
{
	G_ASSERT(!m_initialized);

	m_logic = new Race::OfflineRaceLogic(p_level);
	initCommon();
}

void RaceScene::initializeOnline(TGameMode p_gameMode)
{
	m_impl->initializeOnline(p_gameMode);
}

void RaceSceneImpl::initializeOnline(TGameMode p_gameMode)
{
	G_ASSERT(!m_initialized);

	switch (p_gameMode) {
		case GM_ARCADE:
			cl_log_event(LOG_DEBUG, "building ARCADE logic");
			m_logic = new Race::OnlineRaceLogic();
			break;
		case GM_TIME_TRAIL:
			cl_log_event(LOG_DEBUG, "building TIME TRAIL logic");
			m_logic = new Race::TimeTrailRaceLogic();
			break;

		default:
			G_ASSERT(0 && "unknown option");
	}

	initCommon();
}

void RaceSceneImpl::initCommon()
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
	m_impl->destroy();
}

void RaceSceneImpl::destroy()
{
	G_ASSERT(m_initialized);

	delete m_logic;
	m_logic = NULL;

	delete m_graphics;
	m_graphics = NULL;

	m_parent->setLoaded(false);
	m_initialized = false;
}

void RaceScene::poped()
{
	m_impl->destroy();
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	DirectScene::load(p_gc);
	m_impl->load(p_gc);
}

void RaceSceneImpl::load(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);
	m_graphics->load(p_gc);
}

void RaceScene::draw(CL_GraphicContext &p_gc)
{
	m_impl->draw(p_gc);
}

void RaceSceneImpl::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);
	m_graphics->draw(p_gc);
}

void RaceScene::update(unsigned p_timeElapsed)
{
	m_impl->update(p_timeElapsed);
}

void RaceSceneImpl::update(unsigned p_timeElapsed)
{
	G_ASSERT(m_initialized);

	if (m_logic) {
		m_logic->update(p_timeElapsed);
	}

	m_graphics->update(p_timeElapsed);
}

void RaceScene::inputPressed(const CL_InputEvent &p_event)
{
	m_impl->inputPressed(p_event);
}

void RaceSceneImpl::inputPressed(const CL_InputEvent &p_event)
{
	if (m_initialized) {
		handleInput(Pressed, p_event);
	}
}

void RaceScene::inputReleased(const CL_InputEvent &p_event)
{
	m_impl->inputReleased(p_event);
}

void RaceSceneImpl::inputReleased(const CL_InputEvent &p_event)
{
	if (m_initialized) {
		handleInput(Released, p_event);
	}
}

void RaceSceneImpl::handleInput(InputState p_state, const CL_InputEvent& p_event)
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

void RaceSceneImpl::updateCarTurn()
{
	G_ASSERT(m_initialized);

	Race::Car &car = Game::getInstance().getPlayer().getCar();
	car.setTurn((int) -m_turnLeft + (int) m_turnRight);
}

void RaceSceneImpl::onInputLock()
{
	G_ASSERT(m_initialized);
	m_inputLock = true;
}

const Race::RaceLogic *RaceScene::getLogic() const
{
	return m_impl->m_logic;
}

