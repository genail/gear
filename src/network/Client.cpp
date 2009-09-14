/*
 * Client.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "Client.h"

Client::Client(const CL_String8 &p_host, const int p_port, Car *p_car, Level *p_level) :
	m_car(p_car),
	m_remoteCar(0, 0),
	m_timeElapsed(0),
	m_level(p_level)
{
	m_gameClient.connect(p_host, CL_StringHelp::int_to_local8(p_port));

	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::slotEventReceived);
}

Client::~Client() {
	m_gameClient.disconnect();
}

void Client::update(int p_timeElapsed) {

	static const unsigned sendEventsInterval = 100;

	m_gameClient.process_events();
	m_timeElapsed += p_timeElapsed;

	m_remoteCar.update(p_timeElapsed);

	for (; m_timeElapsed >= sendEventsInterval; m_timeElapsed -= sendEventsInterval) {
		const CL_NetGameEventValue carPositionX(m_car->getPosition().x);
		const CL_NetGameEventValue carPositionY(m_car->getPosition().y);
		const CL_NetGameEventValue carRotation(m_car->getRotation());

		CL_NetGameEvent carPositionEvent("car_position", carPositionX, carPositionY, carRotation);
		m_gameClient.send_event(carPositionEvent);
	}
}

void Client::slotEventReceived(const CL_NetGameEvent &p_netGameEvent) {
	CL_Console::write_line(CL_StringHelp::float_to_local8((float) p_netGameEvent.get_argument(0)) + " x "+ CL_StringHelp::float_to_local8((float) p_netGameEvent.get_argument(1)));

	static bool carAdded = false;

	m_remoteCar.setPosition(CL_Pointf((float) p_netGameEvent.get_argument(0), (float) p_netGameEvent.get_argument(1)));
	m_remoteCar.setRotation((float) p_netGameEvent.get_argument(2));

	if (!carAdded) {
		m_level->addCar(&m_remoteCar);
		carAdded = true;
	}
}
