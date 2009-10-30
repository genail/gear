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
	} else if (eventName == EVENT_PLAYER_FINISHED) {
		handlePlayerFinishedEvent(p_event);
	} else {
		cl_log_event("event", "event remains unhandled: %1", p_event.to_string());
	}
}

void RaceClient::handleCarStateChangeEvent(const CL_NetGameEvent &p_event)
{
	cl_log_event("debug", "RaceClient::handleCarStateChangeEvent()");
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

void RaceClient::initRace(const CL_String &p_levelName)
{
	const CL_NetGameEvent initRaceEvent(EVENT_INIT_RACE, p_levelName);
	m_client->send(initRaceEvent);
}

void RaceClient::markFinished(unsigned p_raceTime)
{
	const CL_NetGameEvent finishedEvent(EVENT_PLAYER_FINISHED, p_raceTime);
	m_client->send(finishedEvent);
}

void RaceClient::handlePlayerFinishedEvent(const CL_NetGameEvent &p_event)
{
	m_signalPlayerFinished.invoke(p_event);
}
