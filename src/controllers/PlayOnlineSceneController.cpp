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

#include "PlayOnlineSceneController.h"

#include "clanlib/core/system.h"
#include "clanlib/network/netgame.h"

#include "common.h"
#include "common/gassert.h"
#include "network/events.h"
#include "gfx/Stage.h"
#include "gfx/scenes/PlayOnlineScene.h"
#include "network/masterserver/MasterServer.h"
#include "network/packets/GameMode.h"
#include "network/packets/ServerInfoRequest.h"
#include "network/packets/ServerInfoResponse.h"

// server query response timeout
const unsigned TIMEOUT_MS = 1000;

class ServerInterviewer : public CL_Runnable
{
	public:
		ServerInterviewer();

		void setServerHost(const CL_String &p_serverHost) { m_serverHost = p_serverHost; }
		void setServerPort(int p_serverPort) { m_serverPort = p_serverPort; }

		void run();

		bool isWorking() const { return m_working; }
		bool isSucceed() const { return m_succeed; }

		TGameMode getGameMode() const { return m_gameMode; }
		const CL_String &getServerName() const { return m_serverName; }
		const CL_String &getMapName() const { return m_mapName; }
		int getPlayerCount() const { return m_playerCount; }
		int getPlayerLimit() const { return m_playerLimit; }
		int getPing() const { return m_ping; }

	private:
		CL_String m_serverHost;
		int m_serverPort;

		bool m_working;
		bool m_succeed;
		bool m_waitingResponse, m_waitingGameMode;
		unsigned m_startTime;

		TGameMode m_gameMode;
		CL_String m_serverName;
		CL_String m_mapName;
		int m_playerCount;
		int m_playerLimit;
		int m_ping;

		void onEventReceived(const CL_NetGameEvent &p_event);

		bool isTimeout() const;
};

ServerInterviewer::ServerInterviewer() :
		m_working(false),
		m_succeed(false)
{
	// empty
}

void ServerInterviewer::run()
{
	m_working = true;
	m_succeed = false;

	CL_NetGameClient msClient;
	CL_Slot receiverSlot =
			msClient.sig_event_received().connect(this, &ServerInterviewer::onEventReceived);

	try {
		msClient.connect(m_serverHost, CL_StringHelp::int_to_text(m_serverPort));

		m_waitingResponse = true;
		m_waitingGameMode = true;

		m_startTime = CL_System::get_time();
		msClient.send_event(Net::ServerInfoRequest().buildEvent());

		while (m_waitingResponse || m_waitingGameMode || isTimeout()) {
			CL_KeepAlive::process();
		}

		if (m_succeed) {
			const unsigned endTime = CL_System::get_time();
			m_ping = static_cast<signed>(endTime - m_startTime);
		}
	} catch (CL_Exception &e) {
		m_succeed = false;
	}

	m_working = false;
}

void ServerInterviewer::onEventReceived(const CL_NetGameEvent &p_event)
{
	if (p_event.get_name() == EVENT_GAME_MODE) {
		Net::GameMode gamemode;
		gamemode.parseEvent(p_event);
		m_gameMode = gamemode.getGameModeType();
		m_waitingGameMode = false;

	} else if (p_event.get_name() == EVENT_INFO_RESPONSE) {
		m_succeed = true;

		Net::ServerInfoResponse response;
		response.parseEvent(p_event);

		m_serverName = response.getServerName();
		m_mapName = response.getMapName();
		m_playerCount = response.getPlayerCount();
		m_playerLimit = response.getPlayerLimit();

		m_waitingResponse = false;

	} else {
		m_succeed = false;
	}
}

bool ServerInterviewer::isTimeout() const {
	return CL_System::get_time() > m_startTime + TIMEOUT_MS;
}

class PlayOnlineSceneControllerImpl
{
	public:

		PlayOnlineSceneController *m_parent;

		PlayOnlineScene *m_scene;
		Net::MasterServer masterServerClient;


		PlayOnlineSceneControllerImpl(PlayOnlineSceneController *p_parent, PlayOnlineScene *p_scene);
		~PlayOnlineSceneControllerImpl();

		void initialize();

		void onRefreshButtonClicked();
		void onMainMenuButtonClicked();
		void onConnectButtonClicked();
		void onServerEntrySelected(const PlayOnlineScene::Entry &p_serverEntry);
};

PlayOnlineSceneController::PlayOnlineSceneController(PlayOnlineScene *p_scene) :
		m_impl(new PlayOnlineSceneControllerImpl(this, p_scene))
{
	// empty
}

PlayOnlineSceneControllerImpl::PlayOnlineSceneControllerImpl(
		PlayOnlineSceneController *p_parent, PlayOnlineScene *p_scene) :
		m_parent(p_parent),
		m_scene(p_scene)
{
	// empty
}

PlayOnlineSceneController::~PlayOnlineSceneController()
{
	// empty
}

PlayOnlineSceneControllerImpl::~PlayOnlineSceneControllerImpl()
{
	// empty
}

void PlayOnlineSceneController::initialize()
{
	m_impl->initialize();
}

void PlayOnlineSceneControllerImpl::initialize()
{
	m_scene->refreshButtonClicked().set(
			this, &PlayOnlineSceneControllerImpl::onRefreshButtonClicked);
	m_scene->mainMenuButtonClicked().set(
			this, &PlayOnlineSceneControllerImpl::onMainMenuButtonClicked);
	m_scene->connectButtonClicked().set(
			this, &PlayOnlineSceneControllerImpl::onConnectButtonClicked);
	m_scene->serverEntrySelected().set(
			this, &PlayOnlineSceneControllerImpl::onServerEntrySelected);
}

void PlayOnlineSceneControllerImpl::onRefreshButtonClicked()
{
	m_scene->clearServerEntries();

	const bool gotList = masterServerClient.requestGameServerList();

	if (gotList) {
		const int count = masterServerClient.gameServerListCount();
		for (int i = 0; i < count; ++i) {
			Net::MasterServer::GameServer gameServer = masterServerClient.gameServerAt(i);

			ServerInterviewer iv;
			iv.setServerHost(gameServer.m_addr);
			iv.setServerPort(gameServer.m_port);
			iv.run();

			PlayOnlineScene::Entry entry;
			if (iv.isSucceed()) {
				entry.serverName = iv.getServerName();
				entry.playerCountCurrent = iv.getPlayerCount();
				entry.playerCountLimit = iv.getPlayerLimit();
				entry.ping = iv.getPing();

				TGameMode gameMode = iv.getGameMode();
				switch (gameMode) {
					case GM_ARCADE:
						entry.gamemode = _("ARCADE");
						break;

					case GM_TIME_TRAIL:
						entry.gamemode = _("TIME TRAIL");
						break;

					default:
						G_ASSERT(0 && "unknown game mode");
				}
			} else {
				entry.serverName = "??";
			}

			m_scene->addServerEntry(entry);
		}
	}
}

void PlayOnlineSceneControllerImpl::onMainMenuButtonClicked()
{
	Gfx::Stage::popScene();
}

void PlayOnlineSceneControllerImpl::onConnectButtonClicked()
{

}

void PlayOnlineSceneControllerImpl::onServerEntrySelected(
		const PlayOnlineScene::Entry &p_serverEntry)
{
	cl_log_event(LOG_DEBUG, "serverName = %1", p_serverEntry.serverName);
}
