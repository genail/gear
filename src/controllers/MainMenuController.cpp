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

MainMenuController::MainMenuController(MainMenuScene *p_scene) :
	m_scene(p_scene),
	m_raceScene(new RaceScene(*p_scene->get_parent_component())),
	m_optionScene(new OptionScene(p_scene->get_parent_component()))
{
	m_slots.connect(m_scene->sig_startRaceClicked(), this, &MainMenuController::onRaceStartClicked);
	m_slots.connect(m_scene->sig_quitClicked(), this, &MainMenuController::onQuitClicked);
	m_slots.connect(m_scene->sig_optionClicked(), this, &MainMenuController::onOptionClicked);
}

MainMenuController::~MainMenuController()
{
	delete m_raceScene;
	delete m_optionScene;
}


void MainMenuController::onRaceStartClicked()
{
	m_scene->displayError("");

	if (m_scene->getPlayerName().empty()) {
		m_scene->displayError(_("No player's name chosen. See Options."));
		return;
	}

	Game &game = Game::getInstance();

	game.getPlayer().setName(m_scene->getPlayerName());

	// create race scene
	m_raceScene->destroy();

	if (!m_scene->getServerAddr().empty()) {
		// separate server addr from port if possible
		std::vector<CL_TempString> parts = CL_StringHelp::split_text(m_scene->getServerAddr(), ":");

		const CL_String serverAddr = parts[0];
		const int serverPort = (parts.size() == 2 ? CL_StringHelp::local8_to_int(parts[1]) : DEFAULT_PORT);

		// online initialization
		m_raceScene->initialize(serverAddr, serverPort);
	} else {
		// offline initialization
		m_raceScene->initialize();
	}

#if !defined(RACE_SCENE_ONLY)
	Gfx::Stage::pushScene(m_raceScene);
#endif // !RACE_SCENE_ONLY
}

void MainMenuController::onQuitClicked()
{
	Gfx::Stage::popScene();
}

void MainMenuController::onOptionClicked()
{
	m_scene->displayError("");

	Gfx::Stage::pushScene(m_optionScene);
}
