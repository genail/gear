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

#pragma once

#include <ClanLib/core.h>
#include <ClanLib/gui.h>

#include "Player.h"
#include "race/Car.h"
#include "race/Level.h"
#include "graphics/RaceUI.h"
#include "race/Viewport.h"
#include "race/ScoreTable.h"
#include "graphics/Drawable.h"
#include "graphics/Car.h"
#include "graphics/GroundBlock.h"
#include "graphics/DecorationSprite.h"
#include "graphics/Smoke.h"

#include "graphics/Scene.h"

#if defined(RACE_SCENE_ONLY)

class RaceScene
{

#else // RACE_SCENE_ONLY

class RaceScene: public Scene
{

#endif // !RACE_SCENE_ONLY

	public:

		RaceScene(CL_GUIComponent *p_guiParent);

		virtual ~RaceScene();

		void initialize();

		void destroy();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

		int getLapsTotal() const { return m_lapsTotal; }

		Gfx::RaceUI& getUI() { return m_raceUI; }

		Viewport& getViewport() { return m_viewport; }

		void init(const CL_String &p_levelName);

		virtual void update(unsigned p_timeElapsed);

	private:

		enum InputState {
			Pressed,
			Released
		};

		/** All players list */
		std::list<Player*> m_players;

		/** Total number of laps */
		int m_lapsTotal;

		/** The score table */
		ScoreTable m_scoreTable;

		/** Race start timer */
		CL_Timer m_raceStartTimer;

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

		/** Race user interface */
		Gfx::RaceUI m_raceUI;

		/** How player sees the scene */
		Viewport m_viewport;

		/** Last drift car position. If null, then no drift was doing last time. */
		CL_Pointf m_lastDriftPoint;

		/** TODO: What is this? */
		float oldSpeed;

		/** FPS counter */
		unsigned m_fps, m_nextFps;

		/** Last fps count time */
		unsigned m_lastFpsRegisterTime;

		/** Logic car to gfx car mapping */
		typedef std::map<const Race::Car*, CL_SharedPtr<Gfx::Car> > carMapping_t;
		carMapping_t m_carMapping;

		/** Block types to gfx ground blocks */
		typedef std::map<Common::GroundBlockType, CL_SharedPtr<Gfx::GroundBlock> > blockMapping_t;
		blockMapping_t m_blockMapping;

		/** Car smoke clounds */
		typedef std::list< CL_SharedPtr<Gfx::Smoke> > smokeList_t;
		smokeList_t m_smokes;

		/** Decorations */
		typedef std::list< CL_SharedPtr<Gfx::DecorationSprite> > decorationList_t;
		decorationList_t m_decorations;

		// other

		/** The slots container */
		CL_SlotContainer m_slots;


		//
		// Methods
		//

		// display

		void loadGroundBlocks(CL_GraphicContext &p_gc);

		void countFps();

		void drawCars(CL_GraphicContext &p_gc);

		void drawCar(CL_GraphicContext &p_gc, const Race::Car &p_car);

		void drawLevel(CL_GraphicContext &p_gc);

		void drawGroundBlock(CL_GraphicContext &p_gc, const Race::Block& p_block, size_t x, size_t y);

		void drawTireTracks(CL_GraphicContext &p_gc);

		void drawUI(CL_GraphicContext &p_gc);

		void drawSmokes(CL_GraphicContext &p_gc);

		// input

		void grabInput();

		bool onInputPressed(const CL_InputEvent &p_event);

		bool onInputReleased(const CL_InputEvent &p_event);

		void handleInput(InputState p_state, const CL_InputEvent& p_event);

		void updateCarTurn();

		// logic

		void updateCars(unsigned p_timeElapsed);

		void updateScale();

		void updateSmokes(unsigned p_timeElapsed);

		// flow control

		void startRace();

		void endRace();

		bool isRaceFinished();

		void markPlayerFinished(const CL_String &p_name, unsigned p_time);

		// helpers

		Player *findPlayer(const CL_String& p_name);

		// event handlers

		void onCarStateChangedRemote(const CL_NetGameEvent& p_event);

		void onCarStateReceived(const Net::CarState &p_carState);

		void onCarStateChangedLocal(Race::Car &p_car);

		void onPlayerJoined(const CL_String &p_name);

		void onPlayerLeaved(const CL_String &p_name);

		void onStartCountdown();

		void onCountdownEnds();

		void onInputLock();

		void onRaceStateChanged(int p_lapsNum);

		void onInitRace(const CL_String& p_levelName);

		void onPlayerFinished(const CL_NetGameEvent &p_event);

#if defined(RACE_SCENE_ONLY)
		friend class Application;
#endif // RACE_SCENE_ONLY

};
