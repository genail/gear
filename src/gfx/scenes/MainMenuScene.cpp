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

#include "MainMenuScene.h"

#include "common.h"
#include "common/Properties.h"
#include "gfx/Stage.h"

const int LABEL_WIDTH = 80;
const int LABEL_HEIGHT = 20;

const int ERROR_LABEL_WIDTH = 200;
const int ERROR_LABEL_HEIGHT = 200;

const int EDIT_WIDTH = 200;
const int EDIT_HEIGHT = 20;

const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 20;

const int H_MARGIN = 20;
const int V_MARGIN = 40;

MainMenuScene::MainMenuScene(CL_GUIComponent *p_parent) :
	GuiScene(p_parent),
	m_controller(this),
	m_serverLabel(this),
	m_serverLineEdit(this),
	m_okButton(this),
	m_errorLabel(this),
	m_quitButton(this),
    m_optionButton(this)
{
	set_class_name("MainMenuScene");

	static const int START_X = 200;
	static const int START_Y = 300;

	int x = START_X;
	int y = START_Y;

	m_serverLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));
	m_serverLabel.set_text("Server addr");

	x += LABEL_WIDTH + H_MARGIN;

	m_serverLineEdit.set_geometry(CL_Rect(x, y, x + EDIT_WIDTH, y + EDIT_HEIGHT));

	x += EDIT_WIDTH;
	y += V_MARGIN;

	m_okButton.set_geometry(CL_Rect(x - BUTTON_WIDTH, y, x, y + BUTTON_HEIGHT));
	m_okButton.set_text("Start Race");

    y += V_MARGIN;

    m_optionButton.set_text("Option");
    m_optionButton.set_geometry(CL_Rect(x - BUTTON_WIDTH, y, x, y + BUTTON_HEIGHT));

	x = START_X;
    y -= V_MARGIN;

	m_errorLabel.set_geometry(CL_Rect(x, y, x + ERROR_LABEL_WIDTH, y + ERROR_LABEL_HEIGHT));

	m_quitButton.set_geometry(CL_Rect(get_width() - 100, get_height() - 40, get_width() - 20, get_height() - 20));
	m_quitButton.set_text("Quit");

	m_okButton.func_clicked().set(this, &MainMenuScene::onOkClicked);
	m_quitButton.func_clicked().set(this, &MainMenuScene::onQuitClicked);
    m_optionButton.func_clicked().set(this, &MainMenuScene::onOptionClicked);


	m_serverLineEdit.set_text(Properties::getPropertyAsString("hostname", ""));
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::load(CL_GraphicContext &p_gc)
{
	m_logoSprite = CL_Sprite(p_gc, "menu/logo", Gfx::Stage::getResourceManager());

	GuiScene::load(p_gc);
}


void MainMenuScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0.0f, 0.0f, get_width(), get_height(), CL_Colorf::white);

	const float w = Gfx::Stage::getWidth();
	const float h = w / m_logoSprite.get_width() * m_logoSprite.get_height();

	CL_Rectf spriteRect(0, 0, w, h);
	m_logoSprite.draw(p_gc, spriteRect);
}

void MainMenuScene::pushed()
{
	m_errorLabel.set_text("");

	GuiScene::pushed();
}

void MainMenuScene::onOkClicked()
{
	INVOKE_0(startRaceClicked);
}

void MainMenuScene::onQuitClicked()
{
	INVOKE_0(quitClicked);
}

void MainMenuScene::onOptionClicked()
{
    INVOKE_0(optionClicked);
}

void MainMenuScene::displayError(const CL_String& p_message)
{
	CL_Font font(get_gc(), "helvetica", 14);
	CL_SpanLayout span;
	span.add_text(p_message, font, CL_Colorf::red);
	m_errorLabel.set_span(span);
}
