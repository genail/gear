/*
 * Client.cpp
 *
 *  Created on: 2009-09-13
 *      Author: chudy
 */

#include "Client.h"

Client::Client(const CL_String8 &p_host, const int p_port, Player *p_localPlayer, Level *p_level) :
	m_localPlayer(p_localPlayer),
	m_timeElapsed(0),
	m_level(p_level)
{
	m_gameClient.connect(p_host, CL_StringHelp::int_to_local8(p_port));

	m_slots.connect(m_gameClient.sig_event_received(), this, &Client::slotEventReceived);
	m_slots.connect(m_gameClient.sig_connected(), this, &Client::slotConnected);

	// listen for local car input change to send it over
	m_slots.connect(m_localPlayer->getCar().sigStatusChange(), this, &Client::slotCarInputChanged);

}

Client::~Client() {
	m_gameClient.disconnect();
}

void Client::slotConnected() {
	// prepare and sent "hi" event

	CL_NetGameEvent hiEvent("hi");

	CL_NetGameEventValue nickname(m_localPlayer->getName());

	hiEvent.add_argument(nickname);

	m_gameClient.send_event(hiEvent);
}

void Client::update(int p_timeElapsed) {

//	static const unsigned sendEventsInterval = 100;
//
//	m_gameClient.process_events();
//	m_timeElapsed += p_timeElapsed;
//
	m_localPlayer->getCar().update(p_timeElapsed);
//
//	for (; m_timeElapsed >= sendEventsInterval; m_timeElapsed -= sendEventsInterval) {
//
//		CL_NetGameEvent carStatus("car_status");
//		m_car->prepareStatusEvent(carStatus);
//		m_gameClient.send_event(carStatus);
//	}
}

void Client::slotEventReceived(const CL_NetGameEvent &p_netGameEvent) {
//	CL_Console::write_line(CL_StringHelp::float_to_local8((float) p_netGameEvent.get_argument(0)) + " x "+ CL_StringHelp::float_to_local8((float) p_netGameEvent.get_argument(1)));

	const CL_String eventName = p_netGameEvent.get_name();

	if (eventName == "hi") {
		eventHi(p_netGameEvent);
	} else if (eventName == "car_status") {
		eventCarStatus(p_netGameEvent);
	}

//	static bool carAdded = false;
//
//	m_localPlayer->getCar().applyStatusEvent(p_netGameEvent, 0);
//
//	if (!carAdded) {
//		m_level->addCar(&m_remoteCar);
//		carAdded = true;
//	}
}

void Client::eventHi(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String nickname = (CL_String) p_netGameEvent.get_argument(0);

	CL_Console::write_line("Client %s connected", nickname);

	m_remotePlayers.push_back(Player(nickname));
	const Player &player = m_remotePlayers.back();
}

void Client::eventCarStatus(const CL_NetGameEvent &p_netGameEvent) {

}

void Client::slotCarInputChanged(Car &p_car) {
	CL_NetGameEvent carStatus("car_status");

	CL_NetGameEventValue nickname = p_car.getPlayer()->getName();

	m_localPlayer->getCar().prepareStatusEvent(carStatus);
	m_gameClient.send_event(carStatus);
}
