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

#include "Player.h"
#include "race/Car.h"
#include "race/Level.h"

class Client {
	public:
		Client();
		virtual ~Client();

		void connect(const CL_String &p_host, int p_port, Player *p_player);

		bool isConnected() const { return m_connected; }

		//
		// client signals
		//

		CL_Signal_v1<Player*> &signalPlayerConnected() { return m_signalPlayerConnected; }

		CL_Signal_v1<Player*> &signalPlayerDisconnected() { return m_signalPlayerDisconnected; }


	private:
		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** Connected state */
		volatile bool m_connected;

		/** Local player pointer */
		Player *m_player;

		/** Remotely connected players (to server) */
		std::vector<Player*> m_remotePlayers;

		/** The slot container */
		CL_SlotContainer m_slots;

		//
		// This class signals
		//

		/** Player joined the game */
		CL_Signal_v1<Player*> m_signalPlayerConnected;

		/** Player leaved the game */
		CL_Signal_v1<Player*> m_signalPlayerDisconnected;

		//
		// connection events
		//

		/** This client is connected */
		void slotConnected();

		/** This client has been disconnected */
		void slotDisconnected();

		void slotEventReceived(const CL_NetGameEvent &p_netGameEvent);

		//
		// game events receivers
		//

		/** Player introduces to rest of players */
		void eventHi(const CL_NetGameEvent &p_netGameEvent);

		/** Car status update */
		void eventCarStatus(const CL_NetGameEvent &p_netGameEvent);

		/** Should prepare a race */
		void eventPrepareRace(const CL_NetGameEvent &p_netGameEvent);

};

#endif /* CLIENT_H_ */
