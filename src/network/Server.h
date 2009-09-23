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

class Server {

		typedef struct {
				int m_carId;
		} Player;

	public:
		Server(int p_port);
		virtual ~Server();

		void prepareRace();

		void update(int p_timeElapsed);

	private:
		CL_NetGameServer m_gameServer;
		CL_SlotContainer m_slots;

		/** List of active connections */
		std::map<CL_NetGameConnection*, Player> m_connections;

		/** Next car id */
		int m_nextCarId;

		/**
		 * Sends the event to all connected players. You can
		 * choose one player that wouldn't receive this event
		 * by passing <code>p_ignore</code> argument.
		 *
		 * @param p_event Event to send forward.
		 * @param p_ignore Client that wouldn't receive this event. Default: NULL
		 */
		void send(const CL_NetGameEvent &p_event, const CL_NetGameConnection* p_ignore = NULL);

		void slotClientConnected(CL_NetGameConnection *p_netGameConnection);
		void slotClientDisconnected(CL_NetGameConnection *p_netGameConnection);
		void slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent);

		int nextCarId() { return m_nextCarId++; }
};

#endif /* SERVER_H_ */
