/*
 * Client.h
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "ClanLib/core.h"
#include "ClanLib/network.h"

#include "race/Car.h"

class Client {
	public:
		Client(const CL_String8 &p_host, const int p_port, Car *p_car);
		virtual ~Client();

		void update(int timeElapsed);

	private:
		/** Game client object */
		CL_NetGameClient m_gameClient;

		/** Car pointer */
		Car *m_car;

		/** Time that passed from last send-events */
		unsigned m_timeElapsed;

		/** The slot container */
		CL_SlotContainer m_slots;

		void slotEventReceived(const CL_NetGameEvent &p_netGameEvent);
};

#endif /* CLIENT_H_ */
