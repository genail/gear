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

#include "RaceGraphics.h"

#include "common.h"
#include "common/Game.h"
#include "gfx/race/level/TireTrack.h"
#include "gfx/race/level/Bound.h"
#include "logic/race/Block.h"

namespace Gfx {

RaceGraphics::RaceGraphics(const Race::RaceLogic *p_logic) :
	m_logic(p_logic)
{
	// attach viewport to player's car
	Game &game = Game::getInstance();

	Player &player = game.getPlayer();
	m_viewport.attachTo(&(player.getCar().getPosition()));
}

RaceGraphics::~RaceGraphics()
{
}

void RaceGraphics::draw(CL_GraphicContext &p_gc)
{
	// initialize player's viewport
	m_viewport.prepareGC(p_gc);

	// draw pure level
	drawLevel(p_gc);

	// on level objects
	drawTireTracks(p_gc);
	drawCars(p_gc);
	drawSmokes(p_gc);

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

void RaceGraphics::load(CL_GraphicContext &p_gc)
{
	m_raceUI.load(p_gc);
	loadGroundBlocks(p_gc);
	loadDecorations(p_gc);
	loadSandPits(p_gc);
}

void RaceGraphics::loadGroundBlocks(CL_GraphicContext &p_gc)
{
	const int first = Common::BT_GRASS;
	const int last = Common::BT_START_LINE_UP; // FIXME: this is dangerous

	for (int i = first; i <= last; ++i) {
		CL_SharedPtr<Gfx::GroundBlock> gfxBlock(new Gfx::GroundBlock((Common::GroundBlockType) i));
		gfxBlock->load(p_gc);

		m_blockMapping[(Common::GroundBlockType) i] = gfxBlock;
	}
}

void RaceGraphics::loadDecorations(CL_GraphicContext &p_gc)
{
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

void RaceGraphics::loadSandPits(CL_GraphicContext &p_gc)
{
	const Race::Level *level = m_logic->getLevel();
	const unsigned sandpitCount = level->getSandpitCount();

	for (unsigned i = 0; i < sandpitCount; ++i) {
		const Race::Sandpit &logicSandpit = level->sandpitAt(i);
		CL_SharedPtr<Gfx::Sandpit> gfxSandpit(new Gfx::Sandpit(&logicSandpit));

		// load and add to list

		gfxSandpit->load(p_gc);
		m_sandpits.push_back(gfxSandpit);
	}
}

void RaceGraphics::drawSandpits(CL_GraphicContext &p_gc)
{
	foreach (CL_SharedPtr<Gfx::Sandpit> &sandpit, m_sandpits) {
		sandpit->draw(p_gc);
	}
}

void RaceGraphics::drawSmokes(CL_GraphicContext &p_gc)
{
	foreach(CL_SharedPtr<Gfx::Smoke> &smoke, m_smokes) {
		if (!smoke->isLoaded()) {
			smoke->load(p_gc);
		}

		smoke->draw(p_gc);
	}
}

void RaceGraphics::drawUI(CL_GraphicContext &p_gc)
{
	Gfx::SpeedMeter &speedMeter = m_raceUI.getSpeedMeter();
	speedMeter.setSpeed(Game::getInstance().getPlayer().getCar().getSpeedKMS());

	m_raceUI.draw(p_gc);
}

void RaceGraphics::drawTireTracks(CL_GraphicContext &p_gc)
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

void RaceGraphics::drawLevel(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();

	drawBackBlocks(p_gc);

	drawSandpits(p_gc);

	drawForeBlocks(p_gc);

	// draw bounds
	const size_t boundCount = level.getBoundCount();
	Gfx::Bound gfxBound;

	for (size_t i = 0; i < boundCount; ++i) {
		const Race::Bound &bound = level.getBound(i);
		gfxBound.setSegment(bound.getSegment());

		gfxBound.draw(p_gc);
	}

#if !defined(NDEBUG) && defined(DRAW_CHECKPOINTS)

	// draw car -> checkpoint links
	const Race::RaceLogic::TPlayerList &players = m_logic->getPlayerList();

	foreach (const Player *player, players) {
		const Race::Car &car = player->getCar();

		const Race::Checkpoint *cp = car.getCurrentCheckpoint();

		if (cp != NULL) {
			CL_Draw::line(p_gc, car.getPosition(), cp->getPosition(), CL_Colorf::green);
		}
	}

	const Race::Track &track = level.getTrack();
	const unsigned checkpointCount = track.getCheckpointCount();

	for (unsigned i = 0; i < checkpointCount; ++i) {
		const Race::Checkpoint *checkpoint = track.getCheckpoint(i);
		const CL_Pointf &point = checkpoint->getPosition();

		CL_Draw::circle(p_gc, point.x, point.y, 5, CL_Colorf::red);
	}

#endif // !NDEBUG && DRAW_CHECKPOINTS
}

void RaceGraphics::drawBackBlocks(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();

	const size_t w = level.getWidth();
	const size_t h = level.getHeight();

	// draw grass
	CL_SharedPtr<Gfx::GroundBlock> gfxGrassBlock = m_blockMapping[Common::BT_GRASS];

	for (size_t iw = 0; iw < w; ++iw) {
		for (size_t ih = 0; ih < h; ++ih) {
			gfxGrassBlock->setPosition(real(CL_Pointf(iw, ih)));
			gfxGrassBlock->draw(p_gc);

			// draw grass decoration sprites
			foreach(CL_SharedPtr<Gfx::DecorationSprite> &decoration, m_decorations) {
				const CL_Pointf &position = decoration->getPosition();

				if (position.x >= real(iw) && position.x < real(iw) + Race::Block::WIDTH && position.y >= real(ih) && position.y < real(ih) + Race::Block::WIDTH) {
					decoration->draw(p_gc);
				}
			}
		}
	}
}

void RaceGraphics::drawForeBlocks(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();

	const size_t w = level.getWidth();
	const size_t h = level.getHeight();

	// draw foreground

	for (size_t iw = 0; iw < w; ++iw) {
		for (size_t ih = 0; ih < h; ++ih) {
			drawGroundBlock(p_gc, level.getBlock(iw, ih), real(iw), real(ih));
		}
	}
}

void RaceGraphics::drawGroundBlock(CL_GraphicContext &p_gc, const Race::Block& p_block, size_t x, size_t y)
{
	assert(m_blockMapping.find(p_block.getType()) != m_blockMapping.end() && "not loaded block type");

	// then draw selected block
	if (p_block.getType() != Common::BT_GRASS) {
		CL_SharedPtr<Gfx::GroundBlock> gfxBlock = m_blockMapping[p_block.getType()];
		gfxBlock->setPosition(CL_Pointf(x, y));

		gfxBlock->draw(p_gc);
	}

}

void RaceGraphics::drawCars(CL_GraphicContext &p_gc)
{
	Race::Level &level = Game::getInstance().getLevel();
	size_t carCount = level.getCarCount();

	for (size_t i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);
		drawCar(p_gc, car);
	}
}

void RaceGraphics::drawCar(CL_GraphicContext &p_gc, const Race::Car &p_car)
{
	TCarMapping::iterator itor = m_carMapping.find(&p_car);

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
	
	#if defined(DRAW_CAR_VECTORS) && !defined(NDEBUG)
		const CL_Pointf &pos = p_car.getPosition();
		p_gc.push_translate(pos.x, pos.y);
		
		CL_Draw::line(p_gc, 0, 0, p_car.m_moveVector.x/10, p_car.m_moveVector.y/10, CL_Colorf::red);
		
		p_gc.pop_modelview();
	#endif // DRAW_CAR_VECTORS && !NDEBUG
	
}

void RaceGraphics::countFps()
{
	const unsigned now = CL_System::get_time();

	if (now - m_lastFpsRegisterTime >= 1000) {
		m_fps = m_nextFps;
		m_nextFps = 0;
		m_lastFpsRegisterTime = now;
	}

	++m_nextFps;
}

void RaceGraphics::update(unsigned p_timeElapsed)
{
	updateViewport(p_timeElapsed);
	updateSmokes(p_timeElapsed);
}

void RaceGraphics::updateViewport(unsigned p_timeElapsed)
{
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


#ifndef NDEBUG
	Gfx::Stage::getDebugLayer()->putMessage(CL_String8("scale"),  CL_StringHelp::float_to_local8(scale));
#endif
}

void RaceGraphics::updateSmokes(unsigned p_timeElapsed)
{
	// remove finished smokes and update the ongoing
	TSmokeList::iterator itor = m_smokes.begin();

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

	// but keep this limit on mind
	static const unsigned SMOKE_PERIOD = 25;
	static unsigned timeFromLastSmoke = SMOKE_PERIOD;

	timeFromLastSmoke += p_timeElapsed;

	static const int RAND_LIMIT = 10;

	if (car.isDrifting() && timeFromLastSmoke >= SMOKE_PERIOD) {

		CL_Pointf smokePosition = car.getPosition();
		smokePosition.x += (rand() % (RAND_LIMIT * 2) - RAND_LIMIT);
		smokePosition.y += (rand() % (RAND_LIMIT * 2) - RAND_LIMIT);

		CL_SharedPtr<Gfx::Smoke> smoke(new Gfx::Smoke(smokePosition));
		smoke->start();

		m_smokes.push_back(smoke);

		timeFromLastSmoke = 0;
	}

}

CL_Pointf RaceGraphics::real(const CL_Pointf &p_point) const
{
	return CL_Pointf(real(p_point.x), real(p_point.y));
}

float RaceGraphics::real(float p_coord) const
{
	return p_coord * Race::Block::WIDTH;
}

}
