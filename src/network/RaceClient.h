/*
 * RaceClient.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#ifndef RACECLIENT_H_
#define RACECLIENT_H_

#include "network/Client.h"

class RaceClient {
	public:
		RaceClient(Client *p_client);
		virtual ~RaceClient();

		Client &getClient() { return *m_client; }

	private:
		/** Base game client */
		Client *m_client;
};

#endif /* RACECLIENT_H_ */
