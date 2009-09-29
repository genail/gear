/*
 * Race.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "Race.h"

#include <stdlib.h>

#include "Message.h"
#include "graphics/Stage.h"
#include "network/events.h"
#include "network/Client.h"

Race::Race(CL_DisplayWindow *p_window, Player *p_player, Client *p_client) :
	m_localPlayer(p_player),
	m_level(),
	m_raceClient(&p_client->getRaceClient()),
	m_raceScene(this),
	m_displayWindow(p_window)
{
	// listen for local car status change
	m_slots.connect(m_localPlayer.getCar().sigStatusChange(), this, &Race::slotCarStateChangedLocal);
	// and for remote car status change
	m_slots.connect(m_raceClient->signalCarStateReceived(), this, &Race::slotCarStateChangedRemote);

	// player join
	m_slots.connect(p_client->signalPlayerConnected(), this, &Race::slotPlayerReady);
	m_slots.connect(p_client->signalPlayerDisconnected(), this, &Race::slotPlayerLeaving);
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

		m_iterationMutex.lock();

		grabInput(delta);
		updateWorld(delta);
		drawScene(delta);

		m_iterationMutex.unlock();

		// Avoid using 100% CPU in the loop:
		static const int MS_60 = 1000 / 60;
		const int sleepTime = MS_60 - delta;

		if (sleepTime > 0) {
			CL_System::sleep(sleepTime);
		}

	}
}

void Race::loadAll() {
	Debug::out() << "Loading race..." << std::endl;
	const unsigned start = CL_System::get_time();

	CL_GraphicContext gc = m_displayWindow->get_gc();
	m_raceScene.load(gc);

	const unsigned duration = CL_System::get_time() - start;
	Debug::out() << "Loaded in " << duration << " ms" << std::endl;
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

#ifndef NDEBUG
	// viewport change
	if (keyboard.get_keycode(CL_KEY_ADD)) {
		const float scale = m_raceScene.getViewport().getScale();
		m_raceScene.getViewport().setScale(scale + scale * 0.01f);
	}

	if (keyboard.get_keycode(CL_KEY_SUBTRACT)) {
		const float scale = m_raceScene.getViewport().getScale();
		m_raceScene.getViewport().setScale(scale - scale * 0.01f);
	}

	// trigger race start
	if (keyboard.get_keycode(CL_KEY_BACKSPACE)) {
		m_raceClient->triggerRaceStart();
	}

#endif
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
}

void Race::slotCarStateChangedRemote(const CL_NetGameEvent& p_event) {

	Debug::out() << "handling " << p_event.get_name().c_str() << std::endl;

	// first argument will be name of player
	const CL_String name = (CL_String) p_event.get_argument(0);

	RacePlayer *racePlayer = findPlayer(name);

	if (racePlayer == NULL) {
		Debug::err() << "remote player '" << name.c_str() << "' not found" << std::endl;
		return;
	}

	Car &car = racePlayer->getCar();
	car.applyStatusEvent(p_event, 1);
}

RacePlayer *Race::findPlayer(const CL_String& p_name) {
	const size_t remotePlayersSize = m_remotePlayers.size();

	for (size_t i = 0; i < remotePlayersSize; ++i) {
		if (m_remotePlayers[i]->getPlayer().getName() == p_name) {
			return m_remotePlayers[i];
		}
	}

	return NULL;
}

void Race::slotCarStateChangedLocal(Car &p_car) {
	CL_NetGameEvent event(EVENT_CAR_STATE_CHANGE, m_localPlayer.getPlayer().getName());
	p_car.prepareStatusEvent(event);

	m_raceClient->sendCarStateEvent(event);
}

void Race::slotPlayerReady(Player* p_player) {

	Debug::out() << "Player '" << p_player->getName().c_str() << "' has arrived" << std::endl;

	RacePlayer *racePlayer = new RacePlayer(p_player);

	m_iterationMutex.lock();

	m_remotePlayers.push_back(racePlayer);
	m_level.addCar(&racePlayer->getCar());

	m_iterationMutex.unlock();
}

void Race::slotPlayerLeaving(Player* p_player) {

	Debug::out() << "Player '" << p_player->getName().c_str() << "' is leaving" << std::endl;

	m_iterationMutex.lock();

	for (
		std::vector<RacePlayer*>::iterator itor = m_remotePlayers.begin();
		itor != m_remotePlayers.end();
		++itor
	) {
		if (&(*itor)->getPlayer() == p_player) {

			// remove car
			m_level.removeCar(&(*itor)->getCar());

			delete *itor;
			m_remotePlayers.erase(itor);
		}
	}

	m_iterationMutex.unlock();
}
