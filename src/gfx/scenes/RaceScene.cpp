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

#include "RaceScene.h"

#include <assert.h>
#include <stdlib.h>

#include "common/Game.h"
#include "common.h"
#include "logic/race/Block.h"
#include "logic/race/OfflineRaceLogic.h"
#include "logic/race/OnlineRaceLogic.h"
#include "network/events.h"
#include "network/packets/CarState.h"
#include "network/client/Client.h"
#include "gfx/race/RaceGraphics.h"
#include "gfx/race/level/TireTrack.h"
#include "gfx/race/level/Bound.h"
#include "debug/RaceSceneKeyBindings.h"

#if defined(RACE_SCENE_ONLY)

RaceScene::RaceScene(CL_GUIComponent *p_guiParent) :

#else // RACE_SCENE_ONLY

RaceScene::RaceScene(CL_GUIComponent *p_guiParent) :
	Scene(p_guiParent),

#endif // !RACE_SCENE_ONLY
	m_logic(NULL),
	m_graphics(NULL),
	m_initialized(false),
	m_inputLock(false),
	m_turnLeft(false),
	m_turnRight(false),
	m_gameMenu(this),
	m_gameMenuController(&m_logic, &m_gameMenu)
{
#if !defined(RACE_SCENE_ONLY)
	set_class_name("RaceScene");

	// listen for input
	func_input_pressed().set(this, &RaceScene::onInputPressed);
	func_input_released().set(this, &RaceScene::onInputReleased);

#endif

}

RaceScene::~RaceScene() {
}

void RaceScene::initialize(const CL_String &p_hostname, int p_port)
{
	if (!m_initialized) {
		if (p_hostname == "") {
			m_logic = new Race::OfflineRaceLogic("resources/level.xml");
		} else {
			m_logic = new Race::OnlineRaceLogic(p_hostname, p_port);
		}

		m_logic->initialize();
		m_graphics = new Gfx::RaceGraphics(m_logic);

		m_initialized = true;
	}

}

void RaceScene::destroy()
{
	if (m_initialized) {
		delete m_logic;
		m_logic = NULL;

		delete m_graphics;
		m_graphics = NULL;

		m_initialized = false;
	}
}

void RaceScene::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);

	m_graphics->draw(p_gc);
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);

	cl_log_event("debug", "RaceScene::load()");

	m_graphics->load(p_gc);

#if !defined(RACE_SCENE_ONLY)
	Scene::load(p_gc);
#endif // !RACE_SCENE_ONLY

}



void RaceScene::update(unsigned p_timeElapsed)
{
	G_ASSERT(m_initialized);

	m_logic->update(p_timeElapsed);
	m_graphics->update(p_timeElapsed);
}



bool RaceScene::onInputPressed(const CL_InputEvent &p_event)
{
	G_ASSERT(m_initialized);

	handleInput(Pressed, p_event);
	return true;
}

bool RaceScene::onInputReleased(const CL_InputEvent &p_event)
{
	G_ASSERT(m_initialized);

	handleInput(Released, p_event);
	return true;
}

void RaceScene::handleInput(InputState p_state, const CL_InputEvent& p_event)
{
	G_ASSERT(m_initialized);

	Race::Car &car = Game::getInstance().getPlayer().getCar();

	bool state;

	switch (p_state) {
		case Pressed:
			state = true;
			break;
		case Released:
			state = false;
			break;
		default:
			assert(0 && "unknown input state");
	}

	switch (p_event.id) {
		case CL_KEY_LEFT:
			m_turnLeft = state;
			break;
		case CL_KEY_RIGHT:
			m_turnRight = state;
			break;
		case CL_KEY_UP:
			car.setAcceleration(state);
			break;
		case CL_KEY_DOWN:
			car.setBrake(state);
			break;
		case CL_KEY_SPACE:
			car.setHandbrake(state);
			break;
		case CL_KEY_F1:
			if (m_logic->isVoteRunning()) {
				m_logic->voteYes();
			}
			break;
		case CL_KEY_F2:
			if (m_logic->isVoteRunning()) {
				m_logic->voteNo();
			}
			break;
	}

	// handle quit request
	if (p_state == Pressed && p_event.id == CL_KEY_ESCAPE) {
		m_gameMenu.set_visible(true);
		m_gameMenu.set_focus(true);
	}

	updateCarTurn();

#if !defined(NDEBUG)
	// debug key bindings
	Dbg::RaceSceneKeyBindings::handleInput(state, p_event);
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

