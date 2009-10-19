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
