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

#include "Game.h"
#include "common.h"
#include "race/Race.h"
#include "race/Block.h"
#include "network/events.h"
#include "network/Client.h"
#include "graphics/TireTrack.h"
#include "graphics/Bound.h"

RaceScene::RaceScene(CL_GUIComponent *p_guiParent) :
	Scene(p_guiParent),
	m_lapsTotal(3),
	m_initialized(false),
	m_inputLock(false),
	m_turnLeft(false),
	m_turnRight(false),
	m_raceUI(),
	m_fps(0),
	m_nextFps(0),
	m_lastFpsRegisterTime(0)
{
	set_class_name("RaceScene");

	// listen for input
	func_input_pressed().set(this, &RaceScene::onInputPressed);
	func_input_released().set(this, &RaceScene::onInputReleased);

	Game &game = Game::getInstance();
	Net::Client &client = game.getNetworkConnection();
	Player &player = game.getPlayer();

	oldSpeed = 0.0f;

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

void RaceScene::initialize()
{
	Game &game = Game::getInstance();

	Player &player = game.getPlayer();
	m_viewport.attachTo(&(player.getCar().getPosition()));
	m_players.push_back(&player);

	Race::Level &level = game.getLevel();
	level.addCar(&player.getCar());
}

void RaceScene::destroy()
{
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Race::Level &level = game.getLevel();

	level.removeCar(&player.getCar());
}

void RaceScene::init(const CL_String &p_levelName)
{
//	Game::getInstance().getNetworkRaceConnection().initRace(p_levelName);
}

void RaceScene::draw(CL_GraphicContext &p_gc)
{
	// initialize player's viewport
	m_viewport.prepareGC(p_gc);

	// draw pure level
	drawLevel(p_gc);

	// draw tire tracks
	drawTireTracks(p_gc);
	drawSmokes(p_gc);
	drawCars(p_gc);

	// revert player's viewport
	m_viewport.finalizeGC(p_gc);

	// draw the user interface
	drawUI(p_gc);

	countFps();

#ifndef NDEBUG
	Gfx::Stage::getDebugLayer()->putMessage("fps", CL_StringHelp::int_to_local8(m_fps));

	Gfx::Stage::getDebugLayer()->draw(p_gc);
#endif // NDEBUG
}

void RaceScene::drawSmokes(CL_GraphicContext &p_gc)
{
	foreach(CL_SharedPtr<Gfx::Smoke> &smoke, m_smokes) {
		if (!smoke->isLoaded()) {
			smoke->load(p_gc);
		}

		smoke->draw(p_gc);
	}
}

void RaceScene::drawUI(CL_GraphicContext &p_gc)
{
	Gfx::SpeedMeter &speedMeter = m_raceUI.getSpeedMeter();
	speedMeter.setSpeed(Game::getInstance().getPlayer().getCar().getSpeedKMS());

	m_raceUI.draw(p_gc);
}

void RaceScene::drawTireTracks(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();
	const Race::TyreStripes &tireStripes = level.getTyreStripes();

	Gfx::TireTrack track;

	foreach (const Race::TyreStripes::Stripe &stripe, tireStripes.getStripeList()) {
		track.setFromPoint(stripe.getFromPoint());
		track.setToPoint(stripe.getToPoint());

		track.draw(p_gc);
	}

}

void RaceScene::drawLevel(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();

	// draw blocks

	const size_t w = level.getWidth();
	const size_t h = level.getHeight();

	for (size_t iw = 0; iw < w; ++iw) {
		for (size_t ih = 0; ih < h; ++ih) {
			drawGroundBlock(p_gc, level.getBlock(iw, ih), iw * 200, ih * 200); // FIXME: Magic numers
		}
	}

	// draw grass on it
	foreach(CL_SharedPtr<Gfx::DecorationSprite> &decoration, m_decorations) {
		decoration->draw(p_gc);
	}

	// draw bounds
	const size_t boundCount = level.getBoundCount();
	Gfx::Bound gfxBound;

	for (size_t i = 0; i < boundCount; ++i) {
		const Race::Bound &bound = level.getBound(i);
		gfxBound.setSegment(bound.getSegment());

		gfxBound.draw(p_gc);
	}
}

void RaceScene::drawGroundBlock(CL_GraphicContext &p_gc, const Race::Block& p_block, size_t x, size_t y)
{
	assert(m_blockMapping.find(p_block.getType()) != m_blockMapping.end() && "not loaded block type");

	// always draw grass block
	CL_SharedPtr<Gfx::GroundBlock> gfxGrassBlock = m_blockMapping[Common::BT_GRASS];
	gfxGrassBlock->setPosition(CL_Pointf(x, y));

	gfxGrassBlock->draw(p_gc);

	// then draw selected block
	if (p_block.getType() != Common::BT_GRASS) {
		CL_SharedPtr<Gfx::GroundBlock> gfxBlock = m_blockMapping[p_block.getType()];
		gfxBlock->setPosition(CL_Pointf(x, y));

		gfxBlock->draw(p_gc);
	}

}

void RaceScene::drawCars(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();
	size_t carCount = level.getCarCount();

	for (size_t i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);
		drawCar(p_gc, car);
	}
}

void RaceScene::drawCar(CL_GraphicContext &p_gc, const Race::Car &p_car)
{
	carMapping_t::iterator itor = m_carMapping.find(&p_car);

	CL_SharedPtr<Gfx::Car> gfxCar;

	if (itor == m_carMapping.end()) {
		gfxCar = CL_SharedPtr<Gfx::Car>(cl_new Gfx::Car());
		gfxCar->load(p_gc);

		m_carMapping[&p_car] = gfxCar;
	} else {
		gfxCar = itor->second;
	}

	gfxCar->setPosition(p_car.getPosition());
	gfxCar->setRotation(CL_Angle(p_car.getRotationRad(), cl_radians));

	gfxCar->draw(p_gc);

//		gfxCar = (*itor->second);
////		(&itor->second)->get()->draw(p_gc);
//		cl_log_event("debug", "a");
//	} else {
//
////		m_carsMapping[&p_car]->draw(p_gc);
//		cl_log_event("debug", "b");
//	}
}

void RaceScene::countFps()
{
	const unsigned now = CL_System::get_time();

	if (now - m_lastFpsRegisterTime >= 1000) {
		m_fps = m_nextFps;
		m_nextFps = 0;
		m_lastFpsRegisterTime = now;
	}

	++m_nextFps;
}

void RaceScene::load(CL_GraphicContext &p_gc)
{
	cl_log_event("debug", "RaceScene::load()");

	Scene::load(p_gc);

	m_raceUI.load(p_gc);

	loadGroundBlocks(p_gc);

	// load decorations
	Race::Level &level = Game::getInstance().getLevel();
	const int w = level.getWidth();
	const int h = level.getHeight();

	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < h; ++y) {

			for (int i = 0; i < 3; ++i) {
				CL_SharedPtr<Gfx::DecorationSprite> decoration(new Gfx::DecorationSprite("race/decorations/grass"));

				const CL_Pointf point(x * Race::Block::WIDTH + rand() % Race::Block::WIDTH, y * Race::Block::WIDTH + rand() % Race::Block::WIDTH);
				decoration->setPosition(point);

				decoration->load(p_gc);

				m_decorations.push_back(decoration);
			}

		}
	}
}

void RaceScene::loadGroundBlocks(CL_GraphicContext &p_gc)
{
	const int first = Common::BT_GRASS;
	const int last = Common::BT_START_LINE;

	for (int i = first; i <= last; ++i) {
		CL_SharedPtr<Gfx::GroundBlock> gfxBlock(new Gfx::GroundBlock((Common::GroundBlockType) i));
		gfxBlock->load(p_gc);

		m_blockMapping[(Common::GroundBlockType) i] = gfxBlock;
	}
}

void RaceScene::updateScale() {
	static const float ZOOM_SPEED = 0.005f;
	static const float MAX_SPEED = 500.0f; // FIXME
	
	float speed = fabs( ceil(Game::getInstance().getPlayer().getCar().getSpeed() * 10.0f ) / 10.0f);
	
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
	Gfx::Stage::getDebugLayer()->putMessage(CL_String8("scale"),  CL_StringHelp::float_to_local8(scale));
#endif
}

void RaceScene::update(unsigned p_timeElapsed)
{
	updateScale();

	updateCars(p_timeElapsed);

	Game &game = Game::getInstance();
	Race::Level &level = game.getLevel();

	level.update(p_timeElapsed);

	Player &player = game.getPlayer();
	const Race::Car &car = player.getCar();

	if (car.getLap() > m_lapsTotal) {
		m_viewport.detach();
	}

	updateSmokes(p_timeElapsed);

}

void RaceScene::updateSmokes(unsigned p_timeElapsed)
{
	// remove finished smokes and update the ongoing
	smokeList_t::iterator itor = m_smokes.begin();

	while (itor != m_smokes.end()) {
		if ((*itor)->isFinished()) {
			m_smokes.erase(itor++);
		} else {
			(*itor)->update(p_timeElapsed);
			++itor;
		}
	}

	// if car is drifting then add new smokes
	Player &player = Game::getInstance().getPlayer();
	const Race::Car &car = player.getCar();

	if (car.isDrifting()) {
		CL_SharedPtr<Gfx::Smoke> smoke(new Gfx::Smoke(car.getPosition()));
		smoke->start();

		m_smokes.push_back(smoke);
	}

}

void RaceScene::updateCars(unsigned p_timeElapsed)
{
	foreach(Player *player, m_players) {
		player->getCar().update(p_timeElapsed);
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
}

void RaceScene::updateCarTurn()
{
	Race::Car &car = Game::getInstance().getPlayer().getCar();
	car.setTurn((int) -m_turnLeft + (int) m_turnRight);
}

void RaceScene::startRace()
{
	m_scoreTable.clear();
//	Game::getInstance().getNetworkRaceConnection().triggerRaceStart(3);
}

void RaceScene::onCarStateReceived(const Net::CarState &p_carState)
{
	const CL_String name = p_carState.getName();

	if (name == Game::getInstance().getPlayer().getName()) {
		// this is about me!
		cl_log_event("event", "setting myself to start position");
		Game::getInstance().getPlayer().getCar().applyCarState(p_carState);
	} else {
		// remote player state
		Player *player = findPlayer(name);

		if (player == NULL) {
			cl_log_event("error", "remote player '%1' not found", name);
			return;
		}

		Race::Car &car = player->getCar();
		car.applyCarState(p_carState);
	}
}

Player *RaceScene::findPlayer(const CL_String& p_name)
{
	foreach(Player *player, m_players) {
		if (player->getName() == p_name) {
			return player;
		}
	}

	return NULL;
}

void RaceScene::onCarStateChangedLocal(Race::Car &p_car)
{
	Net::CarState carState = p_car.prepareCarState();
	carState.setName(Game::getInstance().getPlayer().getName());

	Game::getInstance().getNetworkConnection().sendCarState(carState);
}

void RaceScene::onPlayerJoined(const CL_String &p_name)
{

	Player *player = new Player(p_name);

	m_players.push_back(player);
	Game::getInstance().getLevel().addCar(&player->getCar());
}

void RaceScene::onPlayerLeaved(const CL_String &p_name)
{

	for (
		std::list<Player*>::iterator itor = m_players.begin();
		itor != m_players.end();
		++itor
	) {
		if ((*itor)->getName() == p_name) {

			// remove car
			Game::getInstance().getLevel().removeCar(&(*itor)->getCar());

			delete *itor;
			m_players.erase(itor);
		}
	}
}

void RaceScene::onStartCountdown()
{
	cl_log_event("race", "starting countdown...");

	static const unsigned RACE_START_COUNTDOWN_TIME = 3000;

	m_raceStartTimer.start(RACE_START_COUNTDOWN_TIME, false);
//	m_raceUI.displayCountdown();

	// mark all players state as not finished
//	Game::getInstance().getPlayer().setFinished(false);

	foreach (Player *player, m_players) {
//		player->setFinished(false);
	}
}

void RaceScene::onCountdownEnds()
{
	m_raceStartTime = CL_System::get_time();
	m_inputLock = false;
}

void RaceScene::onInputLock()
{
	m_inputLock = true;
}

void RaceScene::onRaceStateChanged(int p_lapsNum)
{
	m_lapsTotal = p_lapsNum;
//	Game::getInstance().getRacePlayer().getCar().setLap(1);
}

void RaceScene::onInitRace(const CL_String& p_levelName)
{
	Game::getInstance().getLevel().addCar(&Game::getInstance().getPlayer().getCar());

	m_initialized = true;

	cl_log_event("race", "Initialized race with level %1", p_levelName);
}

void RaceScene::onPlayerFinished(const CL_NetGameEvent &p_event)
{
	const CL_String playerName = p_event.get_argument(0);
	const unsigned time = p_event.get_argument(1);

	// set this player finished state
	markPlayerFinished(playerName, time);

}

void RaceScene::markPlayerFinished(const CL_String &p_name, unsigned p_time)
{
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
//	foreach (const RacePlayer *player, m_players) {
//		if (!player->isFinished()) {
//			return false;
//		}
//	}
//
//	return true;
	return false;
}
