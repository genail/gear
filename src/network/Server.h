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

		CL_NetGameConnection* getConnectionForPlayer(const Player* player);

		void update(int p_timeElapsed);

		// Signal accessors

		CL_Signal_v2<CL_NetGameConnection*, Player*> &signalPlayerConnected() { return m_signalPlayerConnected; }

		CL_Signal_v2<CL_NetGameConnection*, Player*> &signalPlayerDisconnected() { return m_signalPlayerDisconnected; }

	private:
		/** List of active connections */
		std::map<CL_NetGameConnection*, Player*> m_connections;

		/** ClanLib game server */
		CL_NetGameServer m_gameServer;

		/** Modifications mutex */
		CL_Mutex m_lockMutex;

		/** Connection that is permited to administrate */
		CL_NetGameConnection* m_permitedConnection;

		/** Race server */
		RaceServer m_raceServer;

		/** Slots container */
		CL_SlotContainer m_slots;

		// Signals

		CL_Signal_v2<CL_NetGameConnection*, Player*> m_signalPlayerConnected;

		CL_Signal_v2<CL_NetGameConnection*, Player*> m_signalPlayerDisconnected;


		bool isPermitted(const CL_NetGameConnection *p_connection) const { return p_connection == m_permitedConnection; }

		void send(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void sendToAll(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL);

		void slotClientConnected(CL_NetGameConnection *p_netGameConnection);

		void slotClientDisconnected(CL_NetGameConnection *p_netGameConnection);

		void slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent);

		//
		// event handlers
		//

		void handleGrantEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void handleHiEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);

		void handleInitRaceEvent(CL_NetGameConnection *p_connection, const CL_NetGameEvent &p_event);


		friend class RaceServer;
};

#endif /* SERVER_H_ */
