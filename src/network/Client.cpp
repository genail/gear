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

	for (std::vector<Player*>::iterator itor = m_remotePlayers.begin(); itor != m_remotePlayers.end(); ++itor) {
		(*itor)->getCar().update(p_timeElapsed);
	}

	m_localPlayer->getCar().update(p_timeElapsed);
}

void Client::slotEventReceived(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String eventName = p_netGameEvent.get_name();

	CL_Console::write_line(CL_String8("got event: ") + eventName);

	if (eventName == "hi") {
		eventHi(p_netGameEvent);
	} else if (eventName == "car_status") {
		eventCarStatus(p_netGameEvent);
	}

}

void Client::eventHi(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String nickname = (CL_String) p_netGameEvent.get_argument(0);

	if (getPlayerByName(nickname) != NULL) {
		return;
	}

	CL_Console::write_line("Client {1} connected", nickname);

	Player *player = new Player(nickname);
	m_remotePlayers.push_back(player);

	m_level->addCar(&player->getCar());

	// send hi again (TODO: this should be done by server)
	CL_NetGameEvent hiEvent("hi");
	CL_NetGameEventValue name(m_localPlayer->getName());
	hiEvent.add_argument(name);

	m_gameClient.send_event(hiEvent);
}

void Client::eventCarStatus(const CL_NetGameEvent &p_netGameEvent) {
	const CL_String nickname = (CL_String) p_netGameEvent.get_argument(0);
	Player* player = getPlayerByName(nickname);

	if (player == NULL) {
		CL_Console::write_line("error: got signal from unknown player");
		return;
	}

	player->getCar().applyStatusEvent(p_netGameEvent, 1);
}

Player* Client::getPlayerByName(const CL_String& p_name) {
	for (std::vector<Player*>::iterator itor = m_remotePlayers.begin(); itor != m_remotePlayers.end(); ++itor) {
		if ((*itor)->getName() == p_name) {
			return (*itor);
		}
	}

	return NULL;
}

void Client::slotCarInputChanged(Car &p_car) {
	CL_NetGameEvent carStatus("car_status");

	CL_NetGameEventValue nickname = p_car.getPlayer()->getName();
	carStatus.add_argument(nickname);

	m_localPlayer->getCar().prepareStatusEvent(carStatus);
	m_gameClient.send_event(carStatus);
}

void Client::eventPrepareRace(const CL_NetGameEvent &p_netGameEvent) {
	const int position = (int) p_netGameEvent.get_argument(0);


}
