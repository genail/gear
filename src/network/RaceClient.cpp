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

RaceClient::~RaceClient()
{
}

void RaceClient::handleEvent(const CL_NetGameEvent &p_event)
{
	const CL_String eventName = p_event.get_name();

	if (eventName == EVENT_CAR_STATE_CHANGE) {
		handleCarStateChangeEvent(p_event);
	} else if (eventName == EVENT_START_COUNTDOWN) {
		handleStartCountdownEvent(p_event);
	} else if (eventName == EVENT_LOCK_CAR) {
		handleLockCarEvent(p_event);
	} else if (eventName == EVENT_RACE_STATE) {
		handleRaceStateEvent(p_event);
	} else {
		cl_log_event("event", "event remains unhandled: %1", p_event.to_string());
	}
}

void RaceClient::handleCarStateChangeEvent(const CL_NetGameEvent &p_event)
{
	m_signalCarStateReceived.invoke(p_event);
}

void RaceClient::sendCarStateEvent(const CL_NetGameEvent &p_event)
{
	m_client->send(p_event);
}

void RaceClient::triggerRaceStart(int p_lapsNum)
{
	CL_NetGameEvent raceStartEvent(EVENT_TRIGGER_RACE_START, p_lapsNum);
	m_client->send(raceStartEvent);
}

void RaceClient::handleStartCountdownEvent(const CL_NetGameEvent &p_event)
{
	m_signalStartCountdownEvent.invoke();
}

void RaceClient::handleLockCarEvent(const CL_NetGameEvent &p_event)
{
	m_signalLockCar.invoke();
}

void RaceClient::handleRaceStateEvent(const CL_NetGameEvent &p_event)
{
	m_signalRaceStateChanged.invoke((int) p_event.get_argument(0));
}
