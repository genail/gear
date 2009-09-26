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

		/** Executes the race and returns when it ends. */
		void exec();

		Level& getLevel() { return m_level; }

	private:
		/** This machine player */
		RacePlayer m_localPlayer;

		/** The level */
		Level m_level;

		/** The race network client */
		RaceClient m_raceClient;

		/** The race scene */
		RaceScene m_raceScene;

		/** Players connected remotely */
		std::vector<RacePlayer*> m_remotePlayers;

		/** Display window */
		CL_DisplayWindow *m_displayWindow;

		void loadAll();
		void grabInput(unsigned delta);
		void updateWorld(unsigned delta);
		void drawScene(unsigned delta);

};

#endif /* RACE_H_ */
