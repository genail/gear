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

#include "OptionsController.h"

#include "common/Game.h"
#include "gfx/Stage.h"
#include "gfx/scenes/OptionsScene.h"
#include "common/Properties.h"

OptionController::OptionController(OptionScene *p_scene) :
	m_scene(p_scene)
{
    m_slots.connect(m_scene->sig_cancelClicked(), this, &OptionController::onCancelClicked);
    m_slots.connect(m_scene->sig_okClicked(), this, &OptionController::onOkClicked);
}

OptionController::~OptionController()
{

}

void OptionController::onCancelClicked()
{
    Gfx::Stage::popScene();
}

void OptionController::onOkClicked()
{
	if (CL_StringHelp::trim(m_scene->getPlayersName()) == "")
	{
		m_scene->displayError("No player's name choosen");
		return;
	}

	Properties::setProperty("opt_screen_width", m_scene->getResolutionWidth());
	Properties::setProperty("opt_screen_height", m_scene->getResolutionHeight());
	Properties::setProperty("opt_fullscreen", m_scene->getFullScreen());
	Properties::setProperty("opt_sound_volume", m_scene->getSound());
	Properties::setProperty("opt_player_name", m_scene->getPlayersName());
	Properties::setProperty("opt_use_wsad", m_scene->getWSAD());

	Gfx::Stage::popScene();
};