/*
 * Race.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "Race.h"

#include <stdlib.h>

#include "common.h"
#include "graphics/Stage.h"
#include "network/events.h"
#include "network/Client.h"

Race::Race(CL_DisplayWindow *p_window, Player *p_player, Client *p_client) :
	m_displayWindow(p_window),
	m_lapsNum(20),
	m_localPlayer(p_player),
	m_level(),
	m_initialized(false),
	m_inputLock(false),
	m_raceClient(&p_client->getRaceClient()),
	m_raceScene(this)
{
	// wait for race init
	m_slots.connect(p_client->signalInitRace(), this, &Race::slotInitRace);
	// listen for local car status change
	m_slots.connect(m_localPlayer.getCar().sigStatusChange(), this, &Race::slotCarStateChangedLocal);
	// and for remote car status change
	m_slots.connect(m_raceClient->signalCarStateReceived(), this, &Race::slotCarStateChangedRemote);
	// race start countdown
	m_slots.connect(m_raceClient->signalStartCountdown(), this, &Race::slotStartCountdown);
	// countdown ends
	m_raceStartTimer.func_expired().set(this, &Race::slotCountdownEnds);
	// car lock
	m_slots.connect(m_raceClient->signalLockCar(), this, &Race::slotInputLock);
	// race state
	m_slots.connect(m_raceClient->signalRaceStateChanged(), this, &Race::slotRaceStateChanged);
	// player finished
	m_slots.connect(m_raceClient->signalPlayerFinished(), this, &Race::slotPlayerFinished);

	// player join
	m_slots.connect(p_client->signalPlayerConnected(), this, &Race::slotPlayerReady);
	// player leave
	m_slots.connect(p_client->signalPlayerDisconnected(), this, &Race::slotPlayerLeaving);

	// add all players
	const int playersCount = p_client->getPlayersCount();

	for (int i = 0; i < playersCount; ++i) {
		slotPlayerReady(p_client->getPlayer(i));
	}

}

Race::~Race()
{
}

void Race::exec()
{
	if (m_raceClient->getClient().isConnected()) {
		cl_log_event("race", "Waiting for initialize event from server");

		m_raceClient->initRace("resources/level.txt"); // FIXME: allow to define own level

		while (!m_initialized) {
			// process events
			CL_KeepAlive::process();
			CL_System::sleep(5);
		}

		cl_log_event("race", "initialized!");
	} else {
		cl_log_event("network", "Not connected, going offline");
		m_level.loadFromFile("resources/level.txt");
	}


	m_level.addCar(&m_localPlayer.getCar());

	loadAll();

	unsigned lastTime = CL_System::get_time();

	while (true) { // FIXME: Check when race is finished

		// process events
		CL_KeepAlive::process();

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
			CL_KeepAlive::process();
		}

	}
}

void Race::loadAll()
{
	cl_log_event("race", "Loading race...");
	const unsigned start = CL_System::get_time();

	CL_GraphicContext gc = m_displayWindow->get_gc();
	m_raceScene.load(gc);

	const unsigned duration = CL_System::get_time() - start;
	cl_log_event("race", "Loaded in %1 ms", duration);
}

void Race::grabInput(unsigned delta)
{
	CL_InputDevice keyboard = m_displayWindow->get_ic().get_keyboard();
	Car &car = m_localPlayer.getCar();

	if (keyboard.get_keycode(CL_KEY_ESCAPE)) {
		exit(0); // TODO: Find better way to exit the race
	}

	if (!m_inputLock) {
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

		if (keyboard.get_keycode(CL_KEY_SPACE)) {
			car.setHandbrake(true);
		} else {
			car.setHandbrake(false);
		}
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
		startRace();
	}

#endif
}

void Race::updateWorld(unsigned p_delta)
{
	// update all cars
	Car &localCar = m_localPlayer.getCar();
	localCar.update(p_delta);

	// check car finished state
	if (!m_localPlayer.isFinished() && localCar.getLap() > m_lapsNum) {
		const unsigned time = CL_System::get_time() - m_raceStartTime;
		markPlayerFinished(m_localPlayer.getPlayer().getName(), time);
	}

	const size_t remotePlayersSize = m_remotePlayers.size();

	for (size_t i = 0; i < remotePlayersSize; ++i) {
		m_remotePlayers[i]->getCar().update(p_delta);
	}

	// update the level
	m_level.update(p_delta);

	// update race scene
	m_raceScene.update(p_delta);
}

void Race::drawScene(unsigned p_delta)
{
	CL_GraphicContext gc = m_displayWindow->get_gc();

	gc.clear(CL_Colorf::cadetblue);

	m_raceScene.draw(gc);

	Stage::getDebugLayer()->draw(gc);

	// Make the stuff visible:
	m_displayWindow->flip();

}

void Race::slotCarStateChangedRemote(const CL_NetGameEvent& p_event)
{

	cl_log_event("event", "handling %1", p_event.to_string());

	// first argument will be name of player
	const CL_String name = (CL_String) p_event.get_argument(0);

	if (name == m_localPlayer.getPlayer().getName()) {
		// this is about me!
		cl_log_event("event", "setting myself to start position");
		m_localPlayer.getCar().applyStatusEvent(p_event, 1);
	} else {
		// remote player state
		RacePlayer *racePlayer = findPlayer(name);

		if (racePlayer == NULL) {
			cl_log_event("error", "remote player '%1' not found", name);
			return;
		}

		Car &car = racePlayer->getCar();
		car.applyStatusEvent(p_event, 1);
	}
}

RacePlayer *Race::findPlayer(const CL_String& p_name)
{
	const size_t remotePlayersSize = m_remotePlayers.size();

	for (size_t i = 0; i < remotePlayersSize; ++i) {
		if (m_remotePlayers[i]->getPlayer().getName() == p_name) {
			return m_remotePlayers[i];
		}
	}

	return NULL;
}

void Race::slotCarStateChangedLocal(Car &p_car)
{
	CL_NetGameEvent event(EVENT_CAR_STATE_CHANGE, m_localPlayer.getPlayer().getName());
	p_car.prepareStatusEvent(event);

	m_raceClient->sendCarStateEvent(event);
}

void Race::slotPlayerReady(Player* p_player)
{

	cl_log_event("event", "Player '%1' has arrived", p_player->getName());

	RacePlayer *racePlayer = new RacePlayer(p_player);

	m_iterationMutex.lock();

	m_remotePlayers.push_back(racePlayer);
	m_level.addCar(&racePlayer->getCar());

	m_iterationMutex.unlock();
}

void Race::slotPlayerLeaving(Player* p_player)
{

	cl_log_event("event", "Player '%1' is leaving", p_player->getName());

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

void Race::slotStartCountdown()
{
	cl_log_event("race", "starting countdown...");

	static const unsigned RACE_START_COUNTDOWN_TIME = 3000;

	m_raceStartTimer.start(RACE_START_COUNTDOWN_TIME, false);
	m_raceScene.getUI().displayCountdown();

	// mark all players state as not finished
	m_localPlayer.setFinished(false);

	foreach (RacePlayer *player, m_remotePlayers) {
		player->setFinished(false);
	}
}

void Race::slotCountdownEnds()
{
	m_raceStartTime = CL_System::get_time();
	m_inputLock = false;
}

void Race::slotInputLock()
{
	m_inputLock = true;
}

void Race::slotRaceStateChanged(int p_lapsNum)
{
	m_lapsNum = p_lapsNum;
	m_localPlayer.getCar().setLap(1);
}

void Race::slotInitRace(const CL_String& p_levelName)
{
	m_level.loadFromFile(p_levelName);
	m_initialized = true;

	cl_log_event("race", "Initialized race with level %1", p_levelName);
}

void Race::slotPlayerFinished(const CL_NetGameEvent &p_event)
{
	const CL_String playerName = p_event.get_argument(0);
	const unsigned time = p_event.get_argument(1);

	// set this player finished state
	markPlayerFinished(playerName, time);

}

bool Race::isRaceFinished()
{
	if (!m_localPlayer.isFinished()) {
		return false;
	}

	foreach (const RacePlayer *player, m_remotePlayers) {
		if (!player->isFinished()) {
			return false;
		}
	}

	return true;
}

void Race::startRace()
{
	m_scoreTable.clear();
	m_raceClient->triggerRaceStart(3);
}

void Race::endRace()
{
	// display the score table and quit the game
	const int scoreEntries = m_scoreTable.getEntriesCount();

	CL_Console::write_line("-----------Score Table------------");

	for (int i = 0; i < scoreEntries; ++i) {

		const CL_String &playerName = m_scoreTable.getEntryPlayer(i)->getPlayer().getName();
		const unsigned time = m_scoreTable.getEntryTime(i);

		CL_Console::write_line("%1) %2 (%3 ms)", i + 1, playerName, time);
	}

	CL_Console::write_line("----------------------------------");

	CL_Console::write_line("");
	CL_Console::write_line("Thanks for playing :-)");
}

void Race::markPlayerFinished(const CL_String &p_name, unsigned p_time)
{
	RacePlayer *scorePlayer;

	if (m_localPlayer.getPlayer().getName() == p_name) {

		m_localPlayer.setFinished(true);
		scorePlayer = &m_localPlayer;

		// send to the server that race is finished
		m_raceClient->markFinished(p_time);
	} else {
		RacePlayer *player = findPlayer(p_name);

		if (player == NULL) {
			cl_log_event("error", "Cannot find player called %1", p_name);
			return;
		}

		player->setFinished(true);
		scorePlayer = player;
	}

	// put to score table
	m_scoreTable.add(scorePlayer, p_time);

	// check if this race is finished
	if (isRaceFinished()) {
		endRace();
	}
}
