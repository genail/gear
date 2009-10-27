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

		void connect();

		void disconnect();

		bool isConnected() const { return m_connected; }

		Player* getPlayer(int p_index) { return m_remotePlayers[p_index]; }

		int getPlayersCount() const { return m_remotePlayers.size(); }

		RaceClient& getRaceClient() { return m_raceClient; }

		void setServerAddr(const CL_String& p_addr) { m_addr = p_addr; }

		void setServerPort(int p_port) { m_port = p_port; }

		//
		// client signals
		//

		CL_Signal_v0 &signalConnected() { return m_signalConnected; }

		CL_Signal_v0 &signalConnectionInitialized() { return m_signalConnectionInitialized; }

		CL_Signal_v0 &signalDisconnected() { return m_signalDisconnected; }

		CL_Signal_v1<const CL_String&> &signalInitRace() { return m_signalInitRace; }

		CL_Signal_v1<Player*> &signalPlayerConnected() { return m_signalPlayerConnected; }

		CL_Signal_v1<Player*> &signalPlayerDisconnected() { return m_signalPlayerDisconnected; }


	private:

		/** Server addr */
		CL_String m_addr;

		/** Server port */
		int m_port;

		/** Connected state */
		volatile bool m_connected;

		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** The race client */
		RaceClient m_raceClient;

		/** Remotely connected players (to server) */
		std::vector<Player*> m_remotePlayers;

		/** The slot container */
		CL_SlotContainer m_slots;

		/** Tells if connection is fully initialized */
		volatile bool m_welcomed;

		//
		// This class signals
		//

		/** The client is connected */
		CL_Signal_v0 m_signalConnected;

		/** The connection is initialized (ready to use) */
		CL_Signal_v0 m_signalConnectionInitialized;

		/** The client is no longed connected */
		CL_Signal_v0 m_signalDisconnected;

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

		void handleWelcomeEvent(const CL_NetGameEvent &p_netGameEvent);

		/** Car status update */
		void eventCarStatus(const CL_NetGameEvent &p_netGameEvent);

		friend class RaceClient;

};

#endif /* CLIENT_H_ */
