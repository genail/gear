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
#include <ClanLib/network.h>

#include "gfx/Scene.h"
#include "gfx/race/RaceGraphics.h"
#include "logic/race/RaceLogic.h"
#include "logic/race/ScoreTable.h"

namespace Net {
	class CarState;
}

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

		void initialize(bool online);

		void destroy();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);


		int getLapsTotal() const { return m_lapsTotal; }

		void init(const CL_String &p_levelName);

		virtual void update(unsigned p_timeElapsed);

	private:

		enum InputState {
			Pressed,
			Released
		};


		/** Logic subsystem */
		Race::RaceLogic *m_logic;

		/** Graphics subsystem */
		Gfx::RaceGraphics m_graphics;



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

		/** Last drift car position. If null, then no drift was doing last time. */
		CL_Pointf m_lastDriftPoint;


		// other

		/** The slots container */
		CL_SlotContainer m_slots;


		//
		// Methods
		//

		// input

		void grabInput();

		bool onInputPressed(const CL_InputEvent &p_event);

		bool onInputReleased(const CL_InputEvent &p_event);

		void handleInput(InputState p_state, const CL_InputEvent& p_event);

		void updateCarTurn();

		// flow control

		void startRace();

		void endRace();

		bool isRaceFinished();

		void markPlayerFinished(const CL_String &p_name, unsigned p_time);

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
