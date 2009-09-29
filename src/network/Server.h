/*
 * Server.h
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "ClanLib/core.h"
#include "ClanLib/network.h"

#include "Player.h"
#include "network/RaceServer.h"

class Server {

	public:
		Server(int p_port);
		virtual ~Server();

		void prepareRace();

		void update(int p_timeElapsed);

		//
		// Signal accessors
		//

		CL_Signal_v2<CL_NetGameConnection*, Player*> &signalPlayerConnected() { return m_signalPlayerConnected; }

		CL_Signal_v2<CL_NetGameConnection*, Player*> &signalPlayerDisconnected() { return m_signalPlayerDisconnected; }

	private:
		/** ClanLib game server */
		CL_NetGameServer m_gameServer;

		/** List of active connections */
		std::map<CL_NetGameConnection*, Player*> m_connections;

		/** Race server */
		RaceServer m_raceServer;

		/** Slots container */
		CL_SlotContainer m_slots;



		void reply(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		//
		// Signals
		//

		CL_Signal_v2<CL_NetGameConnection*, Player*> m_signalPlayerConnected;

		CL_Signal_v2<CL_NetGameConnection*, Player*> m_signalPlayerDisconnected;

		void sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL);

		void slotClientConnected(CL_NetGameConnection *p_netGameConnection);

		void slotClientDisconnected(CL_NetGameConnection *p_netGameConnection);

		void slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent);

		//
		// event handlers
		//

		void handleHiEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);


		friend class RaceServer;
};

#endif /* SERVER_H_ */
