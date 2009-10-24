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

#ifndef RACE_H_
#define RACE_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include <race/Level.h>
#include <race/RaceScene.h>
#include <race/RacePlayer.h>
#include <race/ScoreTable.h>
#include <network/RaceClient.h>

class Race {
	public:
		Race(CL_GUIComponent *p_parent, Player *p_player, Client *p_client);
		virtual ~Race();

		int getLapsNum() const { return m_lapsNum; }

		Level& getLevel() { return m_level; }

		RacePlayer& getLocalPlayer() { return m_localPlayer; }

		/** Executes the race and returns when it ends. */
		void exec();

	private:

		/** Set to true if game window is closing */
		bool m_close;

		/** Iteration mutex */
		CL_Mutex m_iterationMutex;

		/** Number of laps */
		int m_lapsNum;

		/** This machine player */
		RacePlayer m_localPlayer;

		/** The level */
		Level m_level;

		/** Initialized state */
		volatile bool m_initialized;

		/** Input lock */
		bool m_inputLock;

		/** The race network client */
		RaceClient *m_raceClient;

		/** The race scene */
		RaceScene m_raceScene;

		/** Start time of this race */
		unsigned m_raceStartTime;

		/** Race start timer */
		CL_Timer m_raceStartTimer;

		/** Players connected remotely */
		std::vector<RacePlayer*> m_remotePlayers;

		/** The score table */
		ScoreTable m_scoreTable;

		/** The slots container */
		CL_SlotContainer m_slots;

		//
		// Race flow
		//

		void endRace();

		void markPlayerFinished(const CL_String &p_name, unsigned p_time);

		void startRace();

		//
		// Other
		//


		void drawScene(unsigned delta);

		RacePlayer *findPlayer(const CL_String &p_name);

		void grabInput(unsigned delta);

		bool isRaceFinished();

		void loadAll();

		void updateWorld(unsigned delta);


		//
		// Slots
		//

		void slotCarStateChangedRemote(const CL_NetGameEvent &p_event);

		void slotCarStateChangedLocal(Car &p_car);

		void slotCountdownEnds();

		void slotInitRace(const CL_String &p_levelName);

		void slotInputLock();

		void slotPlayerFinished(const CL_NetGameEvent &p_event);

		void slotPlayerReady(Player *p_player);

		void slotPlayerLeaving(Player *p_player);

		void slotRaceStateChanged(int p_lapsNum);

		void slotStartCountdown();

		void slotWindowClose();

		friend class RaceScene;

};

#endif /* RACE_H_ */
