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
	m_lapsTotal(3),
	m_initialized(false),
	m_inputLock(false),
	m_turnLeft(false),
	m_turnRight(false)
{
#if !defined(RACE_SCENE_ONLY)
	set_class_name("RaceScene");

	// listen for input
	func_input_pressed().set(this, &RaceScene::onInputPressed);
	func_input_released().set(this, &RaceScene::onInputReleased);

#endif

	Game &game = Game::getInstance();
	Net::Client &client = game.getNetworkConnection();
	Player &player = game.getPlayer();

	// wait for race init
//	m_slots.connect(client.signalInitRace(), this, &RaceScene::onInitRace);
	// listen for local car status change
	m_slots.connect(player.getCar().sigStatusChange(), this, &RaceScene::onCarStateChangedLocal);
	// and for remote car status change
//	m_slots.connect(raceClient.signalCarStateReceived(), this, &RaceScene::onCarStateChangedRemote);
	m_slots.connect(client.sig_carStateReceived(), this, &RaceScene::onCarStateReceived);
	// race start countdown
//	m_slots.connect(raceClient.signalStartCountdown(), this, &RaceScene::onStartCountdown);
	// countdown ends
	m_raceStartTimer.func_expired().set(this, &RaceScene::onCountdownEnds);
	// car lock
//	m_slots.connect(raceClient.signalLockCar(), this, &RaceScene::onInputLock);
	// race state
//	m_slots.connect(raceClient.signalRaceStateChanged(), this, &RaceScene::onRaceStateChanged);
	// player finished
//	m_slots.connect(raceClient.signalPlayerFinished(), this, &RaceScene::onPlayerFinished);

	// player join
	m_slots.connect(client.sig_playerJoined(), this, &RaceScene::onPlayerJoined);
	// player leave
	m_slots.connect(client.sig_playerLeaved(), this, &RaceScene::onPlayerLeaved);

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
	assert(m_initialized);

	m_graphics->draw(p_gc);
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	assert(m_initialized);

	cl_log_event("debug", "RaceScene::load()");

	m_graphics->load(p_gc);

#if !defined(RACE_SCENE_ONLY)
	Scene::load(p_gc);
#endif // !RACE_SCENE_ONLY

}



void RaceScene::update(unsigned p_timeElapsed)
{
	assert(m_initialized);

	m_logic->update(p_timeElapsed);
	m_graphics->update(p_timeElapsed);
}



bool RaceScene::onInputPressed(const CL_InputEvent &p_event)
{
	assert(m_initialized);

	handleInput(Pressed, p_event);
	return true;
}

bool RaceScene::onInputReleased(const CL_InputEvent &p_event)
{
	assert(m_initialized);

	handleInput(Released, p_event);
	return true;
}

void RaceScene::handleInput(InputState p_state, const CL_InputEvent& p_event)
{
	assert(m_initialized);

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
#ifndef NDEBUG
		case CL_KEY_BACKSPACE:
		{
			if (p_state == Released) {
				startRace();
			}
		}
		break;
#endif // NDEBUG
	}

	// handle quit request
	if (p_state == Pressed && p_event.id == CL_KEY_ESCAPE) {
		Gfx::Stage::popScene();
	}

	updateCarTurn();

#if !defined(NDEBUG)
	// debug key bindings
	Dbg::RaceSceneKeyBindings::handleInput(state, p_event);
#endif // !NDEBUG
}

void RaceScene::updateCarTurn()
{
	assert(m_initialized);

	Race::Car &car = Game::getInstance().getPlayer().getCar();
	car.setTurn((int) -m_turnLeft + (int) m_turnRight);
}

void RaceScene::startRace()
{
	assert(m_initialized);

	m_scoreTable.clear();
//	Game::getInstance().getNetworkRaceConnection().triggerRaceStart(3);
}

void RaceScene::onCarStateReceived(const Net::CarState &p_carState)
{
	assert(m_initialized);

//	const CL_String name = p_carState.getName();
//
//	if (name == Game::getInstance().getPlayer().getName()) {
//		// this is about me!
//		cl_log_event("event", "setting myself to start position");
//		Game::getInstance().getPlayer().getCar().applyCarState(p_carState);
//	} else {
//		// remote player state
//		Player *player = findPlayer(name);
//
//		if (player == NULL) {
//			cl_log_event("error", "remote player '%1' not found", name);
//			return;
//		}
//
//		Race::Car &car = player->getCar();
//		car.applyCarState(p_carState);
//	}
}



void RaceScene::onCarStateChangedLocal(Race::Car &p_car)
{
	assert(m_initialized);

	Net::CarState carState = p_car.prepareCarState();
	carState.setName(Game::getInstance().getPlayer().getName());

	Game::getInstance().getNetworkConnection().sendCarState(carState);
}

void RaceScene::onPlayerJoined(const CL_String &p_name)
{
	assert(m_initialized);

//	Player *player = new Player(p_name);
//
//	m_players.push_back(player);
//	Game::getInstance().getLevel().addCar(&player->getCar());
}

void RaceScene::onPlayerLeaved(const CL_String &p_name)
{
	assert(m_initialized);
//	for (
//		std::list<Player*>::iterator itor = m_players.begin();
//		itor != m_players.end();
//		++itor
//	) {
//		if ((*itor)->getName() == p_name) {
//
//			// remove car
//			Game::getInstance().getLevel().removeCar(&(*itor)->getCar());
//
//			delete *itor;
//			m_players.erase(itor);
//		}
//	}
}

void RaceScene::onStartCountdown()
{
	assert(m_initialized);

	cl_log_event("race", "starting countdown...");

	static const unsigned RACE_START_COUNTDOWN_TIME = 3000;

	m_raceStartTimer.start(RACE_START_COUNTDOWN_TIME, false);
//	m_raceUI.displayCountdown();

	// mark all players state as not finished
//	Game::getInstance().getPlayer().setFinished(false);

//	foreach (Player *player, m_players) {
////		player->setFinished(false);
//	}
}

void RaceScene::onCountdownEnds()
{
	assert(m_initialized);

	m_raceStartTime = CL_System::get_time();
	m_inputLock = false;
}

void RaceScene::onInputLock()
{
	assert(m_initialized);

	m_inputLock = true;
}

void RaceScene::onRaceStateChanged(int p_lapsNum)
{
	assert(m_initialized);

	m_lapsTotal = p_lapsNum;
}

void RaceScene::onPlayerFinished(const CL_NetGameEvent &p_event)
{
	assert(m_initialized);

	const CL_String playerName = p_event.get_argument(0);
	const unsigned time = p_event.get_argument(1);

	// set this player finished state
	markPlayerFinished(playerName, time);

}

void RaceScene::markPlayerFinished(const CL_String &p_name, unsigned p_time)
{
	assert(m_initialized);
//	RacePlayer *scorePlayer;
//
//	if (Game::getInstance().getRacePlayer().getPlayer().getName() == p_name) {
//
//		Game::getInstance().getRacePlayer().setFinished(true);
//		scorePlayer = &Game::getInstance().getRacePlayer();
//
//		// send to the server that race is finished
//		Game::getInstance().getNetworkRaceConnection().markFinished(p_time);
//	} else {
//		RacePlayer *player = findPlayer(p_name);
//
//		if (player == NULL) {
//			cl_log_event("error", "Cannot find player called %1", p_name);
//			return;
//		}
//
//		player->setFinished(true);
//		scorePlayer = player;
//	}
//
//	// put to score table
//	m_scoreTable.add(scorePlayer, p_time);
//
//	// check if this race is finished
//	if (isRaceFinished()) {
//		endRace();
//	}
}

void RaceScene::endRace()
{
	assert(m_initialized);
//	// display the score table and quit the game
//	const int scoreEntries = m_scoreTable.getEntriesCount();
//
//	CL_Console::write_line("-----------Score Table------------");
//
//	for (int i = 0; i < scoreEntries; ++i) {
//
//		const CL_String &playerName = m_scoreTable.getEntryPlayer(i)->getPlayer().getName();
//		const unsigned time = m_scoreTable.getEntryTime(i);
//
//		CL_Console::write_line("%1) %2 (%3 ms)", i + 1, playerName, time);
//	}
//
//	CL_Console::write_line("----------------------------------");
//
//	CL_Console::write_line("");
}

bool RaceScene::isRaceFinished()
{
	assert(m_initialized);
//	foreach (const RacePlayer *player, m_players) {
//		if (!player->isFinished()) {
//			return false;
//		}
//	}
//
//	return true;
	return false;
}
