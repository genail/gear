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
#include "network/RaceClient.h"

class Client {
	public:
		Client();
		virtual ~Client();

		void connect(const CL_String &p_host, int p_port, Player *p_player);

		bool isConnected() const { return m_connected; }

		RaceClient& getRaceClient() { return m_raceClient; }

		//
		// client signals
		//

		CL_Signal_v1<const CL_String&> &signalInitRace() { return m_signalInitRace; }

		CL_Signal_v1<Player*> &signalPlayerConnected() { return m_signalPlayerConnected; }

		CL_Signal_v1<Player*> &signalPlayerDisconnected() { return m_signalPlayerDisconnected; }


	private:
		/** Connected state */
		volatile bool m_connected;

		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** Local player pointer */
		Player *m_player;

		/** The race client */
		RaceClient m_raceClient;

		/** Remotely connected players (to server) */
		std::vector<Player*> m_remotePlayers;

		/** The slot container */
		CL_SlotContainer m_slots;

		//
		// This class signals
		//

		/** Race initialize event received */
		CL_Signal_v1<const CL_String&> m_signalInitRace;

		/** Player joined the game */
		CL_Signal_v1<Player*> m_signalPlayerConnected;

		/** Player leaved the game */
		CL_Signal_v1<Player*> m_signalPlayerDisconnected;


		void send(const CL_NetGameEvent &p_event);

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

		/** Race is initialized on server side */
		void handleInitRaceEvent(const CL_NetGameEvent &p_event);

		/** New player is connected */
		void handlePlayerConnectedEvent(const CL_NetGameEvent &p_netGameEvent);

		/** Player disconnects */
		void handlePlayerDisconnectedEvent(const CL_NetGameEvent &p_netGameEvent);

		/** Car status update */
		void eventCarStatus(const CL_NetGameEvent &p_netGameEvent);

		/** Should prepare a race */
		void eventPrepareRace(const CL_NetGameEvent &p_netGameEvent);


		friend class RaceClient;

};

#endif /* CLIENT_H_ */
