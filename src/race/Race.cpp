/*
 * Race.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "Race.h"

#include <stdlib.h>

#include "Debug.h"
#include "graphics/Stage.h"

Race::Race(CL_DisplayWindow *p_window, Player *p_player, Client *p_client) :
	m_localPlayer(p_player),
	m_level(),
	m_raceClient(p_client),
	m_raceScene(&m_localPlayer, &m_level),
	m_displayWindow(p_window)
{

}

Race::~Race() {
}

void Race::exec() {

	m_level.addCar(&m_localPlayer.getCar());

	loadAll();

	unsigned lastTime = CL_System::get_time();

	while (true) { // FIXME: Check when race is finished

		const unsigned delta = CL_System::get_time() - lastTime;
		lastTime += delta;

		grabInput(delta);
		updateWorld(delta);
		drawScene(delta);

	}
}

void Race::loadAll() {
	Debug::out << "Loading race..." << std::endl;
	const unsigned start = CL_System::get_time();

	CL_GraphicContext gc = m_displayWindow->get_gc();
	m_raceScene.load(gc);

	const unsigned duration = CL_System::get_time() - start;
	Debug::out << "Loaded in " << duration << " ms" << std::endl;
}

void Race::grabInput(unsigned delta) {
	CL_InputDevice keyboard = m_displayWindow->get_ic().get_keyboard();
	Car &car = m_localPlayer.getCar();

	if (keyboard.get_keycode(CL_KEY_ESCAPE)) {
		exit(0); // TODO: Find better way to exit the race
	}

	if (keyboard.get_keycode(CL_KEY_LEFT) && !keyboard.get_keycode(CL_KEY_RIGHT)) {
		car.setTurn(-1.0f);
	} else if (keyboard.get_keycode(CL_KEY_RIGHT) && !keyboard.get_keycode(CL_KEY_LEFT)) {
		car.setTurn(1.0f);
	} else {
		car.setTurn(0.0f);
	}

	if (keyboard.get_keycode(CL_KEY_UP)) {
		car.setAcceleration(true);
	} else {
		car.setAcceleration(false);
	}

	if (keyboard.get_keycode(CL_KEY_DOWN)) {
		car.setBrake(true);
	} else {
		car.setBrake(false);
	}
}

void Race::updateWorld(unsigned delta) {
	// update all cars
	m_localPlayer.getCar().update(delta);

	const size_t remotePlayersSize = m_remotePlayers.size();

	for (size_t i = 0; i < remotePlayersSize; ++i) {
		m_remotePlayers[i]->getCar().update(delta);
	}
}

void Race::drawScene(unsigned delta) {
	CL_GraphicContext gc = m_displayWindow->get_gc();

	gc.clear(CL_Colorf::cadetblue);

	m_raceScene.draw(gc);

	Stage::getDebugLayer()->draw(gc);

	// Make the stuff visible:
	m_displayWindow->flip();

	// Read messages from the windowing system message queue,
	// if any are available:
	CL_KeepAlive::process();

	// Avoid using 100% CPU in the loop:
	static const int MS_60 = 1000 / 60;
	const int sleepTime = MS_60 - delta;

	if (sleepTime > 0) {
		CL_System::sleep(sleepTime);
	}
}
