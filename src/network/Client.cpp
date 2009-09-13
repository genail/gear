/*
 * Client.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "Client.h"

Client::Client(const CL_String8 &p_host, const int p_port, Car *p_car) :
	m_car(p_car),
	m_timeElapsed(0)
{
	m_gameClient.connect(p_host, CL_StringHelp::int_to_local8(p_port));

	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::slotEventReceived);
}

Client::~Client() {
	m_gameClient.disconnect();
}

void Client::update(int timeElapsed) {

	static const unsigned sendEventsInterval = 100;

	m_gameClient.process_events();

	m_timeElapsed += timeElapsed;

	for (; m_timeElapsed >= sendEventsInterval; m_timeElapsed -= sendEventsInterval) {
		const CL_NetGameEventValue carPositionX(m_car->getPosition().x);
		const CL_NetGameEventValue carPositionY(m_car->getPosition().y);

		CL_NetGameEvent carPositionEvent("car_position", carPositionX, carPositionY);
		m_gameClient.send_event(carPositionEvent);
	}
}

void Client::slotEventReceived(const CL_NetGameEvent &p_netGameEvent) {
	CL_Console::write_line("event received");
}
