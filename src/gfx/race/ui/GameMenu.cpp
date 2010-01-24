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

#include "GameMenu.h"

#include "common.h"
#include "gfx/Stage.h"

namespace Gfx {

const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 110;
const float WINDOW_ROUNDNESS = 20.0f;
const int MARGIN = 20;
const int BUTTON_HEIGHT = 30;
const int BUTTON_SPACE = 15;

GameMenu::GameMenu(DirectScene &p_scene) :
	Overlay(
			p_scene,
			CL_Rect(
					(int) (Stage::getWidth()  / 2 - WINDOW_WIDTH  / 2),
					(int) (Stage::getHeight() / 2 - WINDOW_HEIGHT / 2),
					(int) (Stage::getWidth()  / 2 + WINDOW_WIDTH  / 2),
					(int) (Stage::getHeight() / 2 + WINDOW_HEIGHT / 2)
			)
	),
	m_bg(CL_Sizef(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_ROUNDNESS),
	m_callVoteButton(this),
	m_exitButton(this)
{
	setVisible(false);
}

GameMenu::~GameMenu()
{
	// empty
}

void GameMenu::load(CL_GraphicContext &p_gc)
{
	int y = MARGIN;

	m_callVoteButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_callVoteButton.set_text(_("Call a vote"));

	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_exitButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_exitButton.set_text(_("Exit race"));
}

void GameMenu::draw(CL_GraphicContext &p_gc, const CL_Rect &p_clip)
{
	static const CL_Colorf FILL_COLOR(1.0f, 1.0f, 1.0f, 0.8f);
	static const CL_Pointf ZERO_POINT(0, 0);

	m_bg.fill(p_gc, ZERO_POINT, FILL_COLOR);
}

CL_Callback_v0 &GameMenu::func_exit_clicked()
{
	return m_exitButton.func_clicked();
}

CL_Callback_v0 &GameMenu::func_vote_clicked()
{
	return m_callVoteButton.func_clicked();
}

}
