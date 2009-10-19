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

#ifndef RACECLIENT_H_
#define RACECLIENT_H_

#include "race/RacePlayer.h"

class Client;

class RaceClient {

	public:

		RaceClient(Client *p_client);

		virtual ~RaceClient();

		Client &getClient() { return *m_client; }

		void handleEvent(const CL_NetGameEvent &p_event);

		void initRace(const CL_String &p_levelName);

		void markFinished(unsigned p_raceTime);

		void sendCarStateEvent(const CL_NetGameEvent &p_event);

		void triggerRaceStart(int p_lapsNum);

		//
		// Signals
		//

		CL_Signal_v1<const CL_NetGameEvent&> &signalCarStateReceived()
		{ return m_signalCarStateReceived; }

		CL_Signal_v0 &signalLockCar()
		{ return m_signalLockCar; }

		CL_Signal_v1<const CL_NetGameEvent&> &signalPlayerFinished()
		{ return m_signalPlayerFinished; }

		CL_Signal_v1<int> &signalRaceStateChanged()
		{ return m_signalRaceStateChanged; }

		CL_Signal_v0 &signalStartCountdown()
		{ return m_signalStartCountdownEvent; }

	private:

		/** Base game client */
		Client *m_client;

		//
		// Signals
		//

		CL_Signal_v1<const CL_NetGameEvent&> 	m_signalCarStateReceived;

		CL_Signal_v0 							m_signalLockCar;

		CL_Signal_v1<const CL_NetGameEvent&>	m_signalPlayerFinished;

		CL_Signal_v1<int> 						m_signalRaceStateChanged;

		CL_Signal_v0 							m_signalStartCountdownEvent;

		//
		// Event handlers
		//

		void handleCarStateChangeEvent(const CL_NetGameEvent &p_event);

		void handleLockCarEvent(const CL_NetGameEvent &p_event);

		void handlePlayerFinishedEvent(const CL_NetGameEvent &p_event);

		void handleRaceStateEvent(const CL_NetGameEvent &p_event);

		void handleStartCountdownEvent(const CL_NetGameEvent &p_event);


};

#endif /* RACECLIENT_H_ */
