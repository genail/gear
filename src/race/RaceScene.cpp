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

#include "common.h"
#include "race/Race.h"

RaceScene::RaceScene(CL_GUIComponent *p_guiParent, Player *p_player, Client *p_client) :
	Scene(p_guiParent),
	m_racePlayer(p_player),
	m_lapsTotal(3),
	m_inputLock(false),
	m_turnLeft(false),
	m_turnRight(false),
	m_raceUI(this),
	m_networkClient(p_client)
{
	set_type_name("RaceScene");

	// listen for input
	func_input_pressed().set(this, &RaceScene::onInputPressed);
	func_input_released().set(this, &RaceScene::onInputReleased);

	m_viewport.attachTo(&(m_racePlayer.getCar().getPosition()));
	oldSpeed = 0.0f;

	m_level.addCar(&m_racePlayer.getCar());
	m_cars.push_back(&m_racePlayer.getCar());
}

RaceScene::~RaceScene() {
}

void RaceScene::draw(CL_GraphicContext &p_gc)
{
	m_viewport.prepareGC(p_gc);

	m_level.draw(p_gc);

	m_viewport.finalizeGC(p_gc);

	m_raceUI.draw(p_gc);
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	Scene::load(p_gc);

	m_level.load(p_gc);

	m_raceUI.load(p_gc);
}

void RaceScene::updateScale() {
	static const float ZOOM_SPEED = 0.005f;
	static const float MAX_SPEED = 500.0f; // FIXME
	
	float speed = fabs( ceil(m_racePlayer.getCar().getSpeed() * 10.0f ) / 10.0f);
	
	float properScale = -( 1.0f / MAX_SPEED ) * speed + 2.0f;
	properScale = ceil( properScale * 100.0f ) / 100.0f;
	
	float scale =  m_viewport.getScale();
	
	if( properScale > scale ) {
		m_viewport.setScale( scale + ZOOM_SPEED );
	} else if ( properScale < scale ){
		m_viewport.setScale( scale - ZOOM_SPEED );
	}
	
	oldSpeed = speed;
	
		
#ifndef NDEBUG
	Stage::getDebugLayer()->putMessage(CL_String8("scale"),  CL_StringHelp::float_to_local8(scale));
#endif
}

void RaceScene::update(unsigned p_timeElapsed)
{
	cl_log_event("debug", "update: %1", p_timeElapsed);

	updateScale();

	updateCars(p_timeElapsed);

	m_level.update(p_timeElapsed);

	const Car &car = m_racePlayer.getCar();

	if (car.getLap() > m_lapsTotal) {
		m_viewport.detach();
	}

	// update race UI
	m_raceUI.update(p_timeElapsed);
}

void RaceScene::updateCars(unsigned p_timeElapsed)
{
	foreach(Car *car, m_cars) {
		car->update(p_timeElapsed);
	}
}


bool RaceScene::onInputPressed(const CL_InputEvent &p_event)
{
	handleInput(Pressed, p_event);
	return true;
}

bool RaceScene::onInputReleased(const CL_InputEvent &p_event)
{
	handleInput(Released, p_event);
	return true;
}

void RaceScene::handleInput(InputState p_state, const CL_InputEvent& p_event)
{
	Car &car = m_racePlayer.getCar();

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
	}

	// handle quit request
	if (p_state == Pressed && p_event.id == CL_KEY_ESCAPE) {
		Stage::popScene();
	}

	updateCarTurn();
}

void RaceScene::updateCarTurn()
{
	Car &car = m_racePlayer.getCar();
	car.setTurn((int) -m_turnLeft + (int) m_turnRight);
}

void RaceScene::grabInput()
{
//		const CL_InputDevice &keyboard = getInput();
//		Car &car = m_racePlayer.getCar();
//
//		if (keyboard.get_keycode(CL_KEY_ESCAPE)) {
////			m_race->m_close = true; FIXME: fix the quit sequence
//			exit(0);
//		}
//
//		if (!m_inputLock) {
//			if (keyboard.get_keycode(CL_KEY_LEFT) && !keyboard.get_keycode(CL_KEY_RIGHT)) {
//				car.setTurn(-1.0f);
//			} else if (keyboard.get_keycode(CL_KEY_RIGHT) && !keyboard.get_keycode(CL_KEY_LEFT)) {
//				car.setTurn(1.0f);
//			} else {
//				car.setTurn(0.0f);
//			}
//
//			if (keyboard.get_keycode(CL_KEY_UP)) {
//				car.setAcceleration(true);
//			} else {
//				car.setAcceleration(false);
//			}
//
//			if (keyboard.get_keycode(CL_KEY_DOWN)) {
//				car.setBrake(true);
//			} else {
//				car.setBrake(false);
//			}
//
//			if (keyboard.get_keycode(CL_KEY_SPACE)) {
//				car.setHandbrake(true);
//			} else {
//				car.setHandbrake(false);
//			}
//		}
//
//	#ifndef NDEBUG
//		// viewport change
//		if (keyboard.get_keycode(CL_KEY_ADD)) {
//			const float scale = getViewport().getScale();
//			getViewport().setScale(scale + scale * 0.01f);
//		}
//
//		if (keyboard.get_keycode(CL_KEY_SUBTRACT)) {
//			const float scale = getViewport().getScale();
//			getViewport().setScale(scale - scale * 0.01f);
//		}
//
//		// trigger race start
//		if (keyboard.get_keycode(CL_KEY_BACKSPACE)) {
//			startRace();
//		}
//
//	#endif
}

void RaceScene::startRace()
{
	m_scoreTable.clear();
	m_networkClient.triggerRaceStart(3);
}
