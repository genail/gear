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

#include "common.h"
#include "Player.h"
#include "race/Car.h"
#include "race/Level.h"
#include "network/CarState.h"
#include "network/GameState.h"

namespace Net {

class Client {

		/** Client is connected */
		SIGNAL_0(connected);

		/** Client is disconnected */
		SIGNAL_0(disconnected);

		/** Player info accepted */
		SIGNAL_0(accepted);

		/** Received game state */
		SIGNAL_1(const Net::GameState&, gameStateReceived);

		/** New player joined */
		SIGNAL_1(const CL_String&, playerJoined);

		/** Player leaved */
		SIGNAL_1(const CL_String&, playerLeaved);

		/** Got new car state */
		SIGNAL_1(const Net::CarState&, carStateReceived);

	public:
		Client();

		virtual ~Client();

		void connect();

		void disconnect();


		void sendCarState(const Net::CarState &p_state);


		const CL_String& getServerAddr() const { return m_addr; }

		int getServerPort() const { return m_port; }


		void setServerAddr(const CL_String& p_addr) { m_addr = p_addr; }

		void setServerPort(int p_port) { m_port = p_port; }


	private:

		/** Server addr */
		CL_String m_addr;

		/** Server port */
		int m_port;

		/** Connected state */
		bool m_connected;

		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** The slot container */
		CL_SlotContainer m_slots;


		void send(const CL_NetGameEvent &p_event);

		//
		// connection events
		//

		/** This client is connected */
		void onConnected();

		/** This client has been disconnected */
		void onDisconnected();

		/** Event received */
		void onEventReceived(const CL_NetGameEvent &p_event);

		//
		// game events receivers
		//

		void onGoodbye(const CL_NetGameEvent &p_event);

		void onGameState(const CL_NetGameEvent &p_gameState);

		void onPlayerJoined(const CL_NetGameEvent &p_event);

		void onPlayerLeaved(const CL_NetGameEvent &p_event);

		void onCarState(const CL_NetGameEvent &p_event);

};

} // namespace

#endif /* CLIENT_H_ */
