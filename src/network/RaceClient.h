/*
 * RaceClient.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
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

		CL_Signal_v1<const CL_NetGameEvent&> &signalCarStateReceived() { return m_signalCarStateReceived; }

		CL_Signal_v0 &signalLockCar() { return m_signalLockCar; }

		CL_Signal_v1<int> &signalRaceStateChanged() { return m_signalRaceStateChanged; }

		CL_Signal_v0 &signalStartCountdown() { return m_signalStartCountdownEvent; }

	private:
		/** Base game client */
		Client *m_client;

		//
		// Signals
		//

		CL_Signal_v1<const CL_NetGameEvent&> m_signalCarStateReceived;

		CL_Signal_v0 m_signalLockCar;

		CL_Signal_v1<int> m_signalRaceStateChanged;

		CL_Signal_v0 m_signalStartCountdownEvent;

		//
		// Event handlers
		//

		void handleCarStateChangeEvent(const CL_NetGameEvent &p_event);

		void handleLockCarEvent(const CL_NetGameEvent &p_event);

		void handleRaceStateEvent(const CL_NetGameEvent &p_event);

		void handleStartCountdownEvent(const CL_NetGameEvent &p_event);


};

#endif /* RACECLIENT_H_ */
