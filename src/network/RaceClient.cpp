/*
 * RaceClient.cpp
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#include "RaceClient.h"

#include "network/Client.h"
#include "network/events.h"

RaceClient::RaceClient(Client *p_client) :
	m_client(p_client)
{
}

RaceClient::~RaceClient() {
}

void RaceClient::handleEvent(const CL_NetGameEvent &p_event) {
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_CAR_STATE_CHANGE) {
		handleCarStateChangeEvent(p_event);
	}
}

void RaceClient::handleCarStateChangeEvent(const CL_NetGameEvent &p_event) {
	m_signalCarStateReceived.invoke(p_event);
}

void RaceClient::sendCarStateEvent(const CL_NetGameEvent &p_event) {
	m_client->send(p_event);
}
