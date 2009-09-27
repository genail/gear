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

		void handleEvent(const CL_NetGameEvent &p_event);

		Client &getClient() { return *m_client; }

		void sendCarStateEvent(const CL_NetGameEvent &p_event);

		//
		// Signals
		//

		CL_Signal_v1<const CL_NetGameEvent&> &signalCarStateReceived() { return m_signalCarStateReceived; }

	private:
		/** Base game client */
		Client *m_client;

		//
		// Signals
		//

		CL_Signal_v1<const CL_NetGameEvent&> m_signalCarStateReceived;

		//
		// Event handlers
		//

		void handleCarStateChangeEvent(const CL_NetGameEvent &p_event);
};

#endif /* RACECLIENT_H_ */
