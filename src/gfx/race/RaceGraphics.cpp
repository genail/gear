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

#include "RaceGraphics.h"

#include <list>

#include "common.h"
#include "common/Game.h"
#include "common/Player.h"
#include "common/Units.h"
#include "gfx/DebugLayer.h"
#include "gfx/Stage.h"
#include "gfx/Viewport.h"
#include "gfx/race/level/Bound.h"
#include "gfx/race/level/Car.h"
#include "gfx/race/level/DecorationSprite.h"
#include "gfx/race/level/Level.h"
#include "gfx/race/level/Sandpit.h"
#include "gfx/race/level/Smoke.h"
#include "gfx/race/level/TyreStripes.h"
#include "gfx/race/ui/RaceUI.h"
#include "gfx/race/ui/SpeedMeter.h"
#include "logic/race/Block.h"
#include "logic/race/level/Bound.h"
#include "logic/race/Progress.h"
#include "logic/race/GameLogic.h"
#include "logic/race/level/Checkpoint.h"

namespace Gfx {

class RaceGraphicsImpl
{
	public:

		RaceGraphics *m_parent;

		bool m_loaded;

		/** How player sees the scene */
		Gfx::Viewport m_viewport;

		/** Logic with data for reading only */
		const Race::GameLogic *m_logic;

		/** Level graphics */
		Gfx::Level m_level;

		/** Race scene interface */
		Gfx::RaceUI m_raceUI;

		/** FPS counter */
		unsigned m_fps, m_nextFps;

		/** Last fps count time */
		unsigned m_lastFpsRegisterTime;

		/** Logic car to gfx car mapping */
		typedef std::map<const Race::Car*, CL_SharedPtr<Gfx::Car> > TCarMapping;
		typedef std::pair<const Race::Car*, CL_SharedPtr<Gfx::Car> > TCarMappingPair;
		TCarMapping m_carGfxMapping;

		/** Car smoke periods */
		typedef std::map<const Race::Car*, unsigned> TCarPeriodMap;
		TCarPeriodMap m_carSmokePeriod;

		/** Tyre stripes */
		TyreStripes m_tyreStripes;

		/** Car smoke clouds */
		typedef std::list< CL_SharedPtr<Gfx::Smoke> > TSmokeList;
		TSmokeList m_smokes;

		/** Decorations */
		typedef std::list< CL_SharedPtr<Gfx::DecorationSprite> > TDecorationList;
		TDecorationList m_decorations;

		/** Sandpits */
		typedef std::list< CL_SharedPtr<Gfx::Sandpit> > TSandpitList;
		TSandpitList m_sandpits;


		RaceGraphicsImpl(RaceGraphics *p_parent, const Race::GameLogic *p_logic);
		~RaceGraphicsImpl();

		void draw(CL_GraphicContext &p_gc);
		void load(CL_GraphicContext &p_gc);

		void update(unsigned p_timeElapsed);

		// initialize routines
		void loadDecorations(CL_GraphicContext &p_gc);
		void loadSandPits(CL_GraphicContext &p_gc);
		void loadTyreStripes(CL_GraphicContext &p_gc);

		// update routines
		void updateViewport(unsigned p_timeElapsed);
		void updateSmokes(unsigned p_timeElapsed);
		void updateCarGfxs(unsigned p_timeElapsed);
		void updateCarGfxMapping();
		void updateTyreStripes();


		// drawing routines
		void drawLevel(CL_GraphicContext &p_gc);
		void drawBackBlocks(CL_GraphicContext &p_gc);
		void drawForeBlocks(CL_GraphicContext &p_gc);
		void drawTyreStripes(CL_GraphicContext &p_gc);
		void drawUI(CL_GraphicContext &p_gc);
		void drawCars(CL_GraphicContext &p_gc);
		void drawCar(CL_GraphicContext &p_gc, const Race::Car &p_car);
		void drawSmokes(CL_GraphicContext &p_gc);
		void drawSandpits(CL_GraphicContext &p_gc);

		void countFps();
};

RaceGraphics::RaceGraphics(const Race::GameLogic *p_logic) :
		m_impl(new RaceGraphicsImpl(this, p_logic))
{
	// empty
}

RaceGraphicsImpl::RaceGraphicsImpl(RaceGraphics *p_parent, const Race::GameLogic *p_logic) :
		m_parent(p_parent),
		m_loaded(false),
		m_viewport(),
		m_logic(p_logic),
		m_level(&p_logic->getLevel(), &m_viewport),
		m_raceUI(p_logic, &m_viewport),
		m_tyreStripes(&p_logic->getLevel())
{
	// attach viewport to player's car
	Game &game = Game::getInstance();

	Player &player = game.getPlayer();
	m_viewport.attachTo(&(player.getCar().getPosition()));
}

RaceGraphics::~RaceGraphics()
{
	// empty
}

RaceGraphicsImpl::~RaceGraphicsImpl()
{
	// empty
}

void RaceGraphics::draw(CL_GraphicContext &p_gc)
{
	m_impl->draw(p_gc);
}

void RaceGraphicsImpl::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_loaded);

	// clear the background
	CL_Draw::fill(
			p_gc, 0.0f, 0.0f,
			Stage::getWidth(), Stage::getHeight(),
			CL_Colorf::green
	);

	if (m_logic->getLevel().isUsable()) {

		// load level graphics when level logic is loaded
		// FIXME: Maybe I should wait for gamestate in separate scene?
		if (!m_level.isLoaded()) {
			m_level.load(p_gc);
		}

		// initialize player's viewport
		m_viewport.prepareGC(p_gc);

		// draw pure level
		drawLevel(p_gc);

		// on level objects
		drawTyreStripes(p_gc);
		drawCars(p_gc);
		drawSmokes(p_gc);

		// revert player's viewport
		m_viewport.finalizeGC(p_gc);
	}

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
	m_impl->load(p_gc);
}

void RaceGraphicsImpl::load(CL_GraphicContext &p_gc)
{
	m_raceUI.load(p_gc);
	loadTyreStripes(p_gc);
	loadDecorations(p_gc);
	loadSandPits(p_gc);

	m_loaded = true;
}

void RaceGraphicsImpl::loadDecorations(CL_GraphicContext &p_gc)
{
//	// load decorations
//	const Race::Level &level = m_logic->getLevel();
//	const int w = level.getWidth();
//	const int h = level.getHeight();
//
//	for (int x = 0; x < w; ++x) {
//		for (int y = 0; y < h; ++y) {
//
//			for (int i = 0; i < 3; ++i) {
//				CL_SharedPtr<Gfx::DecorationSprite> decoration(new Gfx::DecorationSprite("race/decorations/grass"));
//
//				const CL_Pointf point(x * Race::Block::WIDTH + rand() % Race::Block::WIDTH, y * Race::Block::WIDTH + rand() % Race::Block::WIDTH);
//				decoration->setPosition(point);
//
//				decoration->load(p_gc);
//
//				m_decorations.push_back(decoration);
//			}
//
//		}
//	}
}

void RaceGraphicsImpl::loadSandPits(CL_GraphicContext &p_gc)
{
//	const Race::Level &level = m_logic->getLevel();
//	const unsigned sandpitCount = level.getSandpitCount();
//
//	for (unsigned i = 0; i < sandpitCount; ++i) {
//		const Race::Sandpit &logicSandpit = level.sandpitAt(i);
//		CL_SharedPtr<Gfx::Sandpit> gfxSandpit(new Gfx::Sandpit(&logicSandpit));
//
//		// load and add to list
//
//		gfxSandpit->load(p_gc);
//		m_sandpits.push_back(gfxSandpit);
//	}
}

void RaceGraphicsImpl::loadTyreStripes(CL_GraphicContext &p_gc)
{
	m_tyreStripes.load(p_gc);
}

void RaceGraphicsImpl::drawSandpits(CL_GraphicContext &p_gc)
{
	foreach (CL_SharedPtr<Gfx::Sandpit> &sandpit, m_sandpits) {
		sandpit->draw(p_gc);
	}
}

void RaceGraphicsImpl::drawSmokes(CL_GraphicContext &p_gc)
{
#if !defined(NO_SMOKES)
	foreach(CL_SharedPtr<Gfx::Smoke> &smoke, m_smokes) {
		if (!smoke->isLoaded()) {
			smoke->load(p_gc);
		}

		smoke->draw(p_gc);
	}
#endif // !NO_SMOKES
}

void RaceGraphicsImpl::drawUI(CL_GraphicContext &p_gc)
{
	Gfx::SpeedMeter &speedMeter = m_raceUI.getSpeedMeter();
	speedMeter.setSpeed(Game::getInstance().getPlayer().getCar().getSpeedKMS());

	m_raceUI.draw(p_gc);
}

void RaceGraphicsImpl::drawTyreStripes(CL_GraphicContext &p_gc)
{
	m_tyreStripes.draw(p_gc);
}

void RaceGraphicsImpl::drawLevel(CL_GraphicContext &p_gc)
{
	m_level.draw(p_gc);

//	const Race::Level &level = m_logic->getLevel();

	drawBackBlocks(p_gc);

	drawSandpits(p_gc);

	drawForeBlocks(p_gc);

//	// draw bounds
//	const size_t boundCount = level.getBoundCount();
//	Gfx::Bound gfxBound;
//
//	for (size_t i = 0; i < boundCount; ++i) {
//		const Race::Bound &bound = level.getBound(i);
//		gfxBound.setSegment(bound.getSegment());
//
//		gfxBound.draw(p_gc);
//	}

#if !defined(NDEBUG) && defined(DRAW_CHECKPOINTS)

	const Race::Progress progress = m_logic->getProgress();
	const int cpCount = progress.getCheckpointCount();

	CL_Pen prevPen = p_gc.get_pen();

	CL_Pen pen;
	pen.set_point_size(10);
	pen.set_line_width(5);

	p_gc.set_pen(pen);

	for (int i = 0; i < cpCount; ++i) {
		const Race::Checkpoint &cp = progress.getCheckpoint(i);
		CL_Draw::point(p_gc, cp.getPosition(), CL_Colorf::red);
	}

	// draw car -> checkpoint links
	const int playerCount = m_logic->getPlayerCount();
	for (int i = 0; i < playerCount; ++i) {
		const Player &player = m_logic->getPlayer(i);

		const Race::Car &car = player.getCar();
		const Race::Checkpoint &cp = progress.getCheckpoint(car);

		CL_Draw::line(
				p_gc,
				car.getPosition(), cp.getPosition(),
				CL_Colorf::aliceblue
		);
	}

	// restore old pen
	p_gc.set_pen(pen);

#endif // !NDEBUG && DRAW_CHECKPOINTS
}

void RaceGraphicsImpl::drawBackBlocks(CL_GraphicContext &p_gc)
{
//	const Race::Level &level = m_logic->getLevel();
//
//	const size_t w = level.getWidth();
//	const size_t h = level.getHeight();
//
//	// draw grass
//	CL_SharedPtr<Gfx::GroundBlock> gfxGrassBlock = m_blockMapping[Common::BT_GRASS];
//
//	for (size_t iw = 0; iw < w; ++iw) {
//		for (size_t ih = 0; ih < h; ++ih) {
//			gfxGrassBlock->setPosition(real(CL_Pointf(iw, ih)));
//			gfxGrassBlock->draw(p_gc);
//
//			// draw grass decoration sprites
//			foreach(CL_SharedPtr<Gfx::DecorationSprite> &decoration, m_decorations) {
//				const CL_Pointf &position = decoration->getPosition();
//
//				if (position.x >= real(iw) && position.x < real(iw) + Race::Block::WIDTH && position.y >= real(ih) && position.y < real(ih) + Race::Block::WIDTH) {
//					decoration->draw(p_gc);
//				}
//			}
//		}
//	}
}

void RaceGraphicsImpl::drawForeBlocks(CL_GraphicContext &p_gc)
{
//	const Race::Level &level = m_logic->getLevel();
//
//	const size_t w = level.getWidth();
//	const size_t h = level.getHeight();
//
//	// draw foreground
//
//	for (size_t iw = 0; iw < w; ++iw) {
//		for (size_t ih = 0; ih < h; ++ih) {
//			drawGroundBlock(p_gc, level.getBlock(iw, ih), real(iw), real(ih));
//		}
//	}
}

void RaceGraphicsImpl::drawCars(CL_GraphicContext &p_gc)
{
	const Race::Level &level = m_logic->getLevel();
	size_t carCount = level.getCarCount();

	for (size_t i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);
		drawCar(p_gc, car);
	}
}

void RaceGraphicsImpl::drawCar(CL_GraphicContext &p_gc, const Race::Car &p_car)
{
	TCarMapping::iterator itor = m_carGfxMapping.find(&p_car);
	if (itor == m_carGfxMapping.end()) {
		return;
	}

	Gfx::Car &carGfx = *m_carGfxMapping[&p_car];

	if (!carGfx.isLoaded()) {
		carGfx.load(p_gc);
	}

	carGfx.draw(p_gc);
	
	#if defined(DRAW_CAR_VECTORS) && !defined(NDEBUG)
		const CL_Pointf &pos = p_car.getPosition();
		p_gc.push_translate(pos.x, pos.y);
		
		CL_Draw::line(p_gc, 0, 0, p_car.m_moveVector.x/10, p_car.m_moveVector.y/10, CL_Colorf::red);
		
		p_gc.pop_modelview();
	#endif // DRAW_CAR_VECTORS && !NDEBUG
	
}

void RaceGraphicsImpl::countFps()
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
	m_impl->update(p_timeElapsed);
}

void RaceGraphicsImpl::update(unsigned p_timeElapsed)
{
	G_ASSERT(m_loaded);

	updateViewport(p_timeElapsed);
	updateTyreStripes();
	updateSmokes(p_timeElapsed);
	updateCarGfxMapping();
	updateCarGfxs(p_timeElapsed);

	m_raceUI.update(p_timeElapsed);

#if !defined(NDEBUG)
	const CL_Pointf &carPos = Game::getInstance().getPlayer().getCar().getPosition();
	Gfx::Stage::getDebugLayer()->putMessage("car x",  cl_format("%1", carPos.x));
	Gfx::Stage::getDebugLayer()->putMessage("car y",  cl_format("%1", carPos.y));

	Gfx::Stage::getDebugLayer()->putMessage(
			"car world x",
			cl_format("%1", Units::toWorld(carPos.x))
	);

	Gfx::Stage::getDebugLayer()->putMessage(
			"car world y",
			cl_format("%1", Units::toWorld(carPos.y))
	);
#endif // !NDEBUG
}

void RaceGraphicsImpl::updateViewport(unsigned p_timeElapsed)
{
	static const float MIN_SCALE = 0.5f;
	static const float MAX_SCALE = 1.0f;
	static const float MAX_SPEED = 10.0f;

	float speed = Game::getInstance().getPlayer().getCar().getSpeed();

	if (speed > MAX_SPEED) {
		speed = MAX_SPEED;
	}

	const float scale = m_viewport.getScale();

	const float speedT = speed / MAX_SPEED;
	float targetScale = MAX_SCALE + speedT * (MIN_SCALE - MAX_SCALE);

	const float nextScale = scale + (targetScale - scale) / 100.0f;

	m_viewport.setScale(nextScale);



#ifndef NDEBUG
	Gfx::Stage::getDebugLayer()->putMessage("scale",  CL_StringHelp::float_to_local8(scale));
#endif
}

void RaceGraphicsImpl::updateTyreStripes()
{
	m_tyreStripes.update();
}

void RaceGraphicsImpl::updateSmokes(unsigned p_timeElapsed)
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

	static const unsigned SMOKE_PERIOD = 25;
	static const int RAND_LIMIT = 20;

	// if car is drifting then add new smokes
	const Race::Level &level = m_logic->getLevel();
	const int carCount = level.getCarCount();

	for (int i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);

		if (m_carSmokePeriod.find(&car) == m_carSmokePeriod.end()) {
			m_carSmokePeriod[&car] = SMOKE_PERIOD;
		}

		unsigned &timeFromLastSmoke = m_carSmokePeriod[&car];

		timeFromLastSmoke += p_timeElapsed;

		if (
				(car.isDrifting() || car.isChoking())
				&& timeFromLastSmoke >= SMOKE_PERIOD
			) {

			CL_Pointf smokePosition = car.getPosition();
			smokePosition.x += (rand() % (RAND_LIMIT * 2) - RAND_LIMIT);
			smokePosition.y += (rand() % (RAND_LIMIT * 2) - RAND_LIMIT);

			CL_SharedPtr<Gfx::Smoke> smoke(new Gfx::Smoke(smokePosition));
			smoke->start();

			m_smokes.push_back(smoke);

			timeFromLastSmoke = 0;
		}
	}
}

void RaceGraphicsImpl::updateCarGfxs(unsigned p_timeElapsed)
{
	TCarMappingPair pair;
	foreach(pair, m_carGfxMapping) {
		if (pair.second->isLoaded()) {
			pair.second->update(p_timeElapsed);
		}
	}
}

void RaceGraphicsImpl::updateCarGfxMapping()
{
	const Race::Level &level = m_logic->getLevel();
	const int carCount = level.getCarCount();

	TCarMapping newCarGfxMapping;
	TCarMapping::iterator itor;

	for (int i = 0; i < carCount; ++i) {
		const Race::Car &car = level.getCar(i);
		itor = m_carGfxMapping.find(&car);

		if (itor != m_carGfxMapping.end()) {
			newCarGfxMapping[&car] = itor->second;
		} else {
			newCarGfxMapping[&car] = CL_SharedPtr<Gfx::Car>(new Gfx::Car(&car));
		}
	}

	m_carGfxMapping = newCarGfxMapping;
}

Gfx::RaceUI &RaceGraphics::getUi()
{
	return m_impl->m_raceUI;
}

}
