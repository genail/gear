/*
 * Race.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#ifndef RACE_H_
#define RACE_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include <race/RacePlayer.h>
#include <race/Level.h>
#include <race/RaceScene.h>
#include <network/RaceClient.h>

class Race {
	public:
		Race(CL_DisplayWindow *p_window, Player *p_player, Client *p_client);
		virtual ~Race();

		Level& getLevel() { return m_level; }

		RacePlayer& getLocalPlayer() { return m_localPlayer; }

		/** Executes the race and returns when it ends. */
		void exec();

	private:

		/** Display window */
		CL_DisplayWindow *m_displayWindow;

		/** Iteration mutex */
		CL_Mutex m_iterationMutex;

		/** This machine player */
		RacePlayer m_localPlayer;

		/** The level */
		Level m_level;

		/** Input lock */
		bool m_inputLock;

		/** The race network client */
		RaceClient *m_raceClient;

		/** The race scene */
		RaceScene m_raceScene;

		/** Race start timer */
		CL_Timer m_raceStartTimer;

		/** Players connected remotely */
		std::vector<RacePlayer*> m_remotePlayers;

		/** The slots container */
		CL_SlotContainer m_slots;


		void drawScene(unsigned delta);

		RacePlayer *findPlayer(const CL_String& p_name);

		void grabInput(unsigned delta);

		void loadAll();

		void updateWorld(unsigned delta);


		//
		// Slots
		//

		void slotCarStateChangedRemote(const CL_NetGameEvent& p_event);

		void slotCarStateChangedLocal(Car &p_car);

		void slotCountdownEnds();

		void slotInputLock();

		void slotPlayerReady(Player* p_player);

		void slotPlayerLeaving(Player* p_player);

		void slotStartCountdown();

};

#endif /* RACE_H_ */
