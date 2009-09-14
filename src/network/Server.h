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

		void update(int p_timeElapsed);

	private:
		CL_NetGameServer m_gameServer;
		CL_SlotContainer m_slots;

		/** List of active connections */
		std::map<CL_NetGameConnection*, Player> m_connections;

		/** Next car id */
		int m_nextCarId;

		void slotClientConnected(CL_NetGameConnection *p_netGameConnection);
		void slotClientDisconnected(CL_NetGameConnection *p_netGameConnection);
		void slotEventArrived(CL_NetGameConnection *p_netGameConnection, const CL_NetGameEvent &p_netGameEvent);

		int nextCarId() { return m_nextCarId++; }
};

#endif /* SERVER_H_ */
