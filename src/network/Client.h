/*
 * Client.h
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "ClanLib/core.h"
#include "ClanLib/network.h"

#include "race/Car.h"
#include "race/Level.h"
#include "race/Player.h"

class Client {
	public:
		Client(const CL_String8 &p_host, const int p_port, Player *p_localPlayer, Level *p_level);
		virtual ~Client();

		void update(int timeElapsed);

	private:
		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** Game level */
		Level *m_level;

		/** Local player pointer */
		Player *m_localPlayer;

		/** Remotly connected player */
		std::vector<Player*> m_remotePlayers;

		/** Time that passed from last send-events */
		unsigned m_timeElapsed;

		/** The slot container */
		CL_SlotContainer m_slots;

		/** Invoked when something changed in car input */
		void slotCarInputChanged(Car &p_car);

		Player* getPlayerByName(const CL_String& p_name);

		//
		// connection events
		//

		void slotConnected();

		void slotEventReceived(const CL_NetGameEvent &p_netGameEvent);

		//
		// game events receivers
		//

		/** Player introduces to rest of players */
		void eventHi(const CL_NetGameEvent &p_netGameEvent);

		/** Car status update */
		void eventCarStatus(const CL_NetGameEvent &p_netGameEvent);

};

#endif /* CLIENT_H_ */
