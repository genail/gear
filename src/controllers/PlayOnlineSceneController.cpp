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

#include <ClanLib/Core/Signals/callback_0.h>
#include <ClanLib/Network/NetGame/client.h>
#include <ClanLib/Network/NetGame/event.h>

#include "gfx/scenes/PlayOnlineScene.h"
#include "network/masterserver/MasterServer.h"
#include "network/packets/ServerInfoRequest.h"

class ServerInterviewer : public CL_Runnable
{
	public:
		ServerInterviewer();

		void run();

	private:
		bool m_working;
		bool m_succeed;

		void onEventReceived(const CL_NetGameEvent &p_event);
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
		msClient.connect(MS_DEFAULT_HOST, CL_StringHelp::int_to_text(MS_DEFAULT_PORT));

		const unsigned before = CL_System::get_time();
		msClient.send_event(Net::ServerInfoRequest().buildEvent());
	} catch (CL_Exception &e) {
		m_working = false;
		m_succeed = false;
	}
}

void ServerInterviewer::onEventReceived(const CL_NetGameEvent &p_event)
{
	if (p_event.get_name() == "server_info") {
		m_succeed = true;
	} else {
		m_succeed = false;
	}
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
	m_scene->refreshClicked().set(this, &PlayOnlineSceneControllerImpl::onRefreshButtonClicked);
}

void PlayOnlineSceneControllerImpl::onRefreshButtonClicked()
{
	m_scene->clearServerEntries();

	const bool gotList = masterServerClient.requestGameServerList();

	if (gotList) {
		const int count = masterServerClient.gameServerListCount();
		cl_log_event("a", "%1", count);
		for (int i = 0; i < count; ++i) {
			Net::MasterServer::GameServer gameServer = masterServerClient.gameServerAt(i);
			PlayOnlineScene::Entry entry;
			entry.serverName = gameServer.m_addr;

			m_scene->addServerEntry(entry);
		}
	}
}
