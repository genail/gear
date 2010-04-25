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

#include "MainMenuController.h"

#include "common/Game.h"
#include "gfx/Stage.h"
#include "gfx/scenes/MainMenuScene.h"
#include "gfx/scenes/RaceScene.h"
#include "gfx/scenes/OptionsScene.h"
#include "gfx/scenes/AuthorsScene.h"
#include "gfx/scenes/EditorScene.h"
#include "logic/race/level/Level.h"
#include "network/client/Client.h"
#include "network/packets/GameState.h"


class NetworkClientConnectRunnable : public CL_Runnable
{
	public:

		CL_Signal_v0 m_finished;

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

	private:

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

class MainMenuControllerImpl
{
	public:

		/** This scene */
		MainMenuScene *m_scene;

		/** Next scene */
		RaceScene *m_raceScene;

		/** Next scene */
		OptionScene *m_optionScene;

		/** Next scene */
		AuthorsScene *m_authorsScene;

		/** Next scene */
		EditorScene *m_editorScene;

		NetworkClientConnectRunnable m_connectRunnable;

		CL_Thread m_connectThread;

		Race::Level m_offlineRaceLevel;

		/** The slot container */
		CL_SlotContainer m_slots;


		MainMenuControllerImpl(MainMenuScene *p_scene);

		void createScenes();

		void connectMainMenuSceneButtons();

		void prepareConnectionThread();

		~MainMenuControllerImpl();

		void destroyScenes();


		// action handlers

		void onRaceStartClicked();

		void displayError(const CL_String &p_text);

		bool playerNameChosen() const;

		void makeNetworkConnection();

		void startOfflineGame();

		CL_String getGameServerHost();

		int getGameServerPort();


		void onQuitClicked();

		void onOptionClicked();

		void onAuthorsClicked();

		void onEditorClicked();

		void onConnectionThreadFinished();

		void startOnlineGame();
};

MainMenuController::MainMenuController(MainMenuScene *p_scene) :
	m_impl(new MainMenuControllerImpl(p_scene))
{
	// empty
}

MainMenuController::~MainMenuController()
{
	// empty
}

MainMenuControllerImpl::MainMenuControllerImpl(MainMenuScene *p_scene) :
		m_scene(p_scene)
{
	createScenes();
	connectMainMenuSceneButtons();
	prepareConnectionThread();
}

void MainMenuControllerImpl::createScenes()
{
	m_raceScene = new RaceScene(*m_scene->get_parent_component());
	m_optionScene = new OptionScene(m_scene->get_parent_component());
	m_authorsScene = new AuthorsScene(m_scene->get_parent_component());
	m_editorScene = new EditorScene(*m_scene->get_parent_component());
}

void MainMenuControllerImpl::connectMainMenuSceneButtons()
{
	m_slots.connect(m_scene->sig_startRaceClicked(), this, &MainMenuControllerImpl::onRaceStartClicked);
	m_slots.connect(m_scene->sig_quitClicked(), this, &MainMenuControllerImpl::onQuitClicked);
	m_slots.connect(m_scene->sig_optionClicked(), this, &MainMenuControllerImpl::onOptionClicked);
	m_slots.connect(m_scene->sig_authorsClicked(), this, &MainMenuControllerImpl::onAuthorsClicked);
	m_slots.connect(m_scene->sig_editorClicked(), this, &MainMenuControllerImpl::onEditorClicked);
}

void MainMenuControllerImpl::prepareConnectionThread()
{
	m_slots.connect(
			m_connectRunnable.m_finished,
			this, &MainMenuControllerImpl::onConnectionThreadFinished
	);
}

MainMenuControllerImpl::~MainMenuControllerImpl()
{
	destroyScenes();
}

void MainMenuControllerImpl::destroyScenes()
{
	delete m_raceScene;
	delete m_optionScene;
	delete m_authorsScene;
	delete m_editorScene;
}


void MainMenuControllerImpl::onRaceStartClicked()
{
	displayError("");

	if (!playerNameChosen()) {
		displayError(_("No player's name chosen. See Options."));
		return;
	}

	Game &game = Game::getInstance();

	game.getPlayer().setName(m_scene->getPlayerName());

	// create race scene
	if (!m_scene->getServerAddr().empty()) {
		makeNetworkConnection();
	} else {
		startOfflineGame();
	}
}

void MainMenuControllerImpl::startOfflineGame()
{
	// FIXME: how to choose a level?
	if (!m_offlineRaceLevel.isLoaded()) {
		m_offlineRaceLevel.load("levels/level2.0.xml");
	}

	m_raceScene->initializeOffline(&m_offlineRaceLevel);
	Gfx::Stage::pushScene(m_raceScene);
}

void MainMenuControllerImpl::displayError(const CL_String &p_text)
{
	m_scene->displayError(p_text);
}

bool MainMenuControllerImpl::playerNameChosen() const
{
	return !m_scene->getPlayerName().empty();
}

void MainMenuControllerImpl::makeNetworkConnection()
{
	m_scene->displayConnectingMessageBox();

	Game &game = Game::getInstance();
	Net::Client &netClient = game.getNetworkConnection();

	netClient.setServerAddr(getGameServerHost());
	netClient.setServerPort(getGameServerPort());

	m_connectThread.start(&m_connectRunnable);
}

void MainMenuControllerImpl::onConnectionThreadFinished()
{
	m_connectThread.join();
	startOnlineGame();
}

void MainMenuControllerImpl::startOnlineGame()
{
	Game &game = Game::getInstance();
	Net::Client &netClient = game.getNetworkConnection();

	if (netClient.isConnected()) {
		m_scene->hideMessageBox();
		m_raceScene->initializeOnline(m_connectRunnable.m_gameMode, m_connectRunnable.m_gameState);
		Gfx::Stage::pushScene(m_raceScene);
	} else {
		m_scene->displayConnectionErrorMessageBox();
	}
}

CL_String MainMenuControllerImpl::getGameServerHost()
{
	std::vector<CL_TempString> parts = CL_StringHelp::split_text(m_scene->getServerAddr(), ":");
	return parts[0];
}

int MainMenuControllerImpl::getGameServerPort()
{
	std::vector<CL_TempString> parts = CL_StringHelp::split_text(m_scene->getServerAddr(), ":");
	if (parts.size() == 2) {
		return CL_StringHelp::local8_to_int(parts[1]);
	} else {
		return DEFAULT_PORT;
	}
}

void MainMenuControllerImpl::onQuitClicked()
{
	Gfx::Stage::popScene();
}

void MainMenuControllerImpl::onOptionClicked()
{
	m_scene->displayError("");

	Gfx::Stage::pushScene(m_optionScene);
}

void MainMenuControllerImpl::onAuthorsClicked()
{
	m_scene->displayError("");

	Gfx::Stage::pushScene(m_authorsScene);
}

void MainMenuControllerImpl::onEditorClicked()
{
	Gfx::Stage::pushScene(m_editorScene);
}
