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

#include "LoadingController.h"

#include "Game.h"

LoadingController::LoadingController(LoadingScene *p_scene) :
	m_scene(p_scene)
{
	Game &game = Game::getInstance();
	Client &client = game.getNetworkConnection();
	RaceClient &raceClient = game.getNetworkRaceConnection();

	client.signalConnected().connect(this, &LoadingController::onClientConnected);
	client.signalConnectionInitialized().connect(this, &LoadingController::onClientInitialized);
}

LoadingController::~LoadingController()
{
}

void LoadingController::loadRace()
{
	Game &game = Game::getInstance();
	Client &client = game.getNetworkConnection();

	if (!client.getServerAddr().empty()) {

		cl_log_event("debug", "Starting online game - connecting to %1", client.getServerAddr());
		m_scene->setMessage("Connecting to server");

		client.connect();

	} else {
		cl_log_event("debug", "Starting offline game");
		loadLevel("resources/level.txt");

		RaceScene &raceScene = game.getSceneContainer().getRaceScene();
		Stage::pushScene(&raceScene);
	}
}

void LoadingController::onClientConnected()
{
	m_scene->setMessage("Connected");
}

void LoadingController::onClientInitialized()
{
	m_scene->setMessage("Waiting for gamestate...");
}

void LoadingController::loadLevel(const CL_String &p_name)
{
	m_scene->setMessage("Loading level");
	CL_KeepAlive::process();

	Game &game = Game::getInstance();
	Level &level = game.getLevel();

	level.destroy();
	level.initialize(p_name);
}
