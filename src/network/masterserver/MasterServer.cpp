/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#include "MasterServer.h"

#include <vector>
#include <ClanLib/network.h>

#include "common.h"

const int NETWORK_PORT_MAX = 65535;

const int TIMEOUT_MS = 10000;

const char *EVENT_SUCCEED = "SUCCEED";

const char *EVENT_FAILED = "FAILED";

const char *EVENT_SERVERLIST = "SERVERLIST";

namespace Net
{

class MasterServerImpl
{
	public:

		const CL_String m_host;

		const int m_port;

		CL_NetGameClient m_netGameClient;

		bool m_connected, m_introduced;

		bool m_registered, m_keptAlive;

		bool m_gotGameServerList;

		CL_Event m_eventConnected, m_eventDisconnected;

		CL_Event m_receivedSucceed, m_receivedFailed;

		CL_Event m_receivedGameServerList;

		std::vector<MasterServer::GameServer> m_gameServers;

		CL_SlotContainer m_slots;


		MasterServerImpl(const CL_String &p_host, int p_port) :
				m_host(p_host),
				m_port(p_port),
				m_connected(false),
				m_introduced(false),
				m_registered(false),
				m_keptAlive(false),
				m_gotGameServerList(false) {
			G_ASSERT(!p_host.empty());
			G_ASSERT(p_port > 0 && p_port <= NETWORK_PORT_MAX);

			m_slots.connect(
					m_netGameClient.sig_connected(),
					this, &MasterServerImpl::onConnect
			);

			m_slots.connect(
					m_netGameClient.sig_disconnected(),
					this, &MasterServerImpl::onDisconnect
			);

			m_slots.connect(
					m_netGameClient.sig_event_received(),
					this, &MasterServerImpl::onEventArrived
			);
		}


		void initializeConnection();

		bool isConnectionReady() const;

		void destroyConnection();


		void tryConnect();

		void onConnect();

		void onDisconnect();

		bool isConnected() const;

		void onEventArrived(const CL_NetGameEvent &p_event);


		void tryIntroduce();

		bool isIntroduced() const;


		void tryRegister(int p_gameServerPort);

		bool isRegistered() const;


		void tryKeepAlive(int p_gameServerPort);

		bool isKeptAlive() const;


		void tryGettingGameServerList();

		void parseServerListEvent(const CL_NetGameEvent &p_event);

		bool gameServerListAvailable() const;

};

MasterServer::MasterServer(const CL_String &p_host, int p_port) :
		m_impl(new MasterServerImpl(p_host, p_port))
{
	// empty
}

MasterServer::~MasterServer()
{
	// empty
}

void MasterServerImpl::initializeConnection()
{
	tryConnect();
	if (isConnected()) {
		tryIntroduce();
	}
}

inline
bool MasterServerImpl::isConnectionReady() const
{
	return isConnected() && isIntroduced();
}

void MasterServerImpl::destroyConnection()
{
	m_netGameClient.disconnect();
}

void MasterServerImpl::tryConnect()
{
	m_eventConnected.reset();
	m_eventDisconnected.reset();
	m_connected = false;

	m_netGameClient.connect(m_host, CL_StringHelp::int_to_text(m_port));
	int result = CL_Event::wait(m_eventConnected, m_eventDisconnected);

	if (result == 0) {
		m_connected = true;
	}
}

inline
bool MasterServerImpl::isConnected() const
{
	return m_connected;
}

void MasterServerImpl::onConnect()
{
	m_eventConnected.set();
}

void MasterServerImpl::onDisconnect()
{
	m_eventDisconnected.set();
}

void MasterServerImpl::onEventArrived(const CL_NetGameEvent &p_event)
{
	if (p_event.get_name() == EVENT_SUCCEED) {
		m_receivedSucceed.set();
	} else if (p_event.get_name() == EVENT_FAILED) {
		m_receivedFailed.set();
	} else if (p_event.get_name() == EVENT_SERVERLIST) {
		parseServerListEvent(p_event);
	}
}

void MasterServerImpl::tryIntroduce()
{
	m_receivedSucceed.reset();
	m_receivedFailed.reset();
	m_introduced = false;

	const CL_NetGameEvent helloEvent("HELLO", MS_PROTOCOL_MAJOR, MS_PROTOCOL_MINOR);
	m_netGameClient.send_event(helloEvent);

	const int result = CL_Event::wait(m_receivedSucceed, m_receivedFailed, TIMEOUT_MS);

	if (result == 0) {
		m_introduced = true;
	} else if (result == 1) {
		throw CL_Exception(_("version mismatch"));
	} else {
		throw CL_Exception(_("timeout reached"));
	}
}

inline
bool MasterServerImpl::isIntroduced() const
{
	return m_introduced;
}

bool MasterServer::registerGameServer(int p_gameServerPort)
{
	m_impl->initializeConnection();
	if (m_impl->isConnectionReady()) {
		m_impl->tryRegister(p_gameServerPort);
		m_impl->destroyConnection();

		return m_impl->isRegistered();
	}

	return false;
}

void MasterServerImpl::tryRegister(int p_gameServerPort)
{
	m_receivedSucceed.reset();
	m_receivedFailed.reset();
	m_registered = false;

	const CL_NetGameEvent registerEvent("REGISTER", p_gameServerPort);
	m_netGameClient.send_event(registerEvent);

	const int result = CL_Event::wait(m_receivedSucceed, m_receivedFailed, TIMEOUT_MS);

	if (result == 0) {
		m_registered = true;
	} else if (result < 0) {
		throw CL_Exception(_("timeout reached"));
	}
}

inline
bool MasterServerImpl::isRegistered() const
{
	return m_registered;
}

bool MasterServer::keepAliveGameServer(int p_gameServerPort)
{
	m_impl->initializeConnection();
	if (m_impl->isConnectionReady()) {
		m_impl->tryKeepAlive(p_gameServerPort);
		m_impl->destroyConnection();

		return m_impl->isKeptAlive();
	}

	return false;
}

void MasterServerImpl::tryKeepAlive(int p_gameServerPort)
{
	m_receivedSucceed.reset();
	m_receivedFailed.reset();
	m_keptAlive = false;

	const CL_NetGameEvent registerEvent("KEEPALIVE", p_gameServerPort);
	m_netGameClient.send_event(registerEvent);

	const int result = CL_Event::wait(m_receivedSucceed, m_receivedFailed, TIMEOUT_MS);

	if (result == 0) {
		m_keptAlive = true;
	} else if (result < 0) {
		throw CL_Exception(_("timeout reached"));
	}
}

inline
bool MasterServerImpl::isKeptAlive() const
{
	return m_keptAlive;
}

bool MasterServer::requestGameServerList()
{
	m_impl->initializeConnection();
	if (m_impl->isConnectionReady()) {
		m_impl->tryGettingGameServerList();

		return m_impl->gameServerListAvailable();
	}

	return false;
}

void MasterServerImpl::tryGettingGameServerList()
{
	m_gotGameServerList = false;
	m_gameServers.clear();

	const CL_NetGameEvent listRequestEvent("LISTREQUEST");
	m_netGameClient.send_event(listRequestEvent);

	const int result = CL_Event::wait(m_receivedGameServerList, TIMEOUT_MS);

	if (result == 0) {
		m_gotGameServerList = true;
	} else {
		throw CL_Exception(_("timeout reached"));
	}
}

void MasterServerImpl::parseServerListEvent(const CL_NetGameEvent &p_event)
{
	static const int HEADER_SIZE = 1;
	static const int ENTRY_SIZE = 2;

	const int entryCount = p_event.get_argument(0);
	MasterServer::GameServer gameServer;

	for (int entryIdx = 0; entryIdx < entryCount; ++entryIdx) {
		const int argIdx = HEADER_SIZE + (ENTRY_SIZE * entryIdx);

		gameServer.m_addr = p_event.get_argument(argIdx);
		gameServer.m_port = p_event.get_argument(argIdx + 1);

		m_gameServers.push_back(gameServer);
	}
}

inline
bool MasterServerImpl::gameServerListAvailable() const
{
	return m_gotGameServerList;
}


int MasterServer::gameServerListCount() const
{
	return m_impl->m_gameServers.size();
}

const MasterServer::GameServer &MasterServer::gameServerAt(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < gameServerListCount());
	return m_impl->m_gameServers[p_index];
}

} // namespace
