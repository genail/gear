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

#include "ConnectController.h"

#include "common/Game.h"
#include "gfx/Stage.h"
#include "gfx/scenes/ConnectScene.h"
#include "gfx/scenes/RaceScene.h"
#include "network/client/Client.h"
#include "network/packets/GameState.h"

class NetworkClientConnectRunnable : public CL_Runnable
{
	public:

		volatile TGameMode m_gameMode;
		Net::GameState m_gameState;

		CL_SlotContainer m_slots;

		NetworkClientConnectRunnable() :
			m_gameModeReceived(false),
			m_gameStateReceived(false)
		{ /* empty */ }

		virtual void run() {
			Game &game = Game::getInstance();
			Net::Client &client = game.getNetworkConnection();

			connectClientSlots(client);
			client.connect();

			waitForGameMode();
			waitForGameState();

			m_finished.invoke();
		}

		CL_Callback_v0 &finished() {
			return m_finished;
		}

	private:

		CL_Callback_v0 m_finished;

		volatile bool m_gameModeReceived;
		volatile bool m_gameStateReceived;


		void connectClientSlots(Net::Client &p_client) {
			m_slots.connect(
					p_client.sig_gameModeReceived(),
					this, &NetworkClientConnectRunnable::onGameModeReceived);

			m_slots.connect(
					p_client.sig_gameStateReceived(),
					this, &NetworkClientConnectRunnable::onGameStateReceived);
		}

		void waitForGameMode() {
			while (!m_gameModeReceived) {
				CL_KeepAlive::process();
				CL_System::sleep(2);
			}
		}

		void onGameModeReceived(TGameMode p_gameMode) {
			m_gameMode = p_gameMode;
			m_gameModeReceived = true;
		}

		void waitForGameState() {
			while (!m_gameStateReceived) {
				CL_KeepAlive::process();
				CL_System::sleep(2);
			}
		}

		void onGameStateReceived(const Net::GameState &p_gameState) {
			m_gameState = p_gameState;
			m_gameStateReceived = true;
		}

};

// --------------------------------------------------------

class ConnectControllerImpl
{
	public:

		ConnectController *m_parent;
		Gfx::ConnectScene *m_scene;

		CL_Thread m_connectThread;
		NetworkClientConnectRunnable m_connectRunnable;

		// future scene
		RaceScene m_raceScene;

		ConnectControllerImpl(ConnectController *p_parent, Gfx::ConnectScene *p_scene);
		~ConnectControllerImpl();

		void makeConnection(const CL_String &p_host, int p_port);
		void onThreadFinished();
};

// --------------------------------------------------------

ConnectController::ConnectController(Gfx::ConnectScene *p_scene) :
		m_impl(new ConnectControllerImpl(this, p_scene))
{
	// empty
}

ConnectControllerImpl::ConnectControllerImpl(
		ConnectController *p_parent, Gfx::ConnectScene *p_scene) :
		m_parent(p_parent),
		m_scene(p_scene),
		m_raceScene(*m_scene->get_parent_component())
{
	m_connectRunnable.finished().set(this, &ConnectControllerImpl::onThreadFinished);
}

ConnectController::~ConnectController()
{
	// empty
}

ConnectControllerImpl::~ConnectControllerImpl()
{
	// empty
}

// --------------------------------------------------------

void ConnectController::makeConnection(const CL_String &p_host, int p_port)
{
	m_impl->makeConnection(p_host, p_port);
}

void ConnectControllerImpl::makeConnection(const CL_String &p_host, int p_port)
{
	m_scene->setStatusText(_("Connecting to game server..."));

	Game &game = Game::getInstance();
	Net::Client &netClient = game.getNetworkConnection();

	netClient.setServerAddr(p_host);
	netClient.setServerPort(p_port);

	m_connectThread.start(&m_connectRunnable);
}

void ConnectControllerImpl::onThreadFinished()
{
	m_connectThread.join();

	Game &game = Game::getInstance();
	Net::Client &netClient = game.getNetworkConnection();

	if (netClient.isConnected()) {
		m_raceScene.initializeOnline(m_connectRunnable.m_gameMode, m_connectRunnable.m_gameState);
		Gfx::Stage::pushScene(&m_raceScene);
	} else {
		Gfx::Stage::popScene();
	}
}
