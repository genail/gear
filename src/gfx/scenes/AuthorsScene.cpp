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

#include "AuthorsScene.h"

#include "common.h"
#include "gfx/Stage.h"

const int LABEL_WIDTH = 200;
const int LABEL_HEIGHT = 400;

const int ERROR_LABEL_WIDTH = 200;
const int ERROR_LABEL_HEIGHT = 20;

const int EDIT_WIDTH = 200;
const int EDIT_HEIGHT = 20;

const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 20;

const int COMBOBOX_WIDTH = 200;
const int COMBOBOX_HEIGHT = 20;

const int CHECKBOX_WIDTH = 200;
const int CHECKBOX_HEIGHT = 20;

const int SLIDER_WIDTH = 200;
const int SLIDER_HEIGHT = 20;

const int H_MARGIN = 20;
const int V_MARGIN = 40;

AuthorsScene::AuthorsScene(CL_GUIComponent *p_parent) :
	GuiScene(p_parent),
    m_controller(this),
	m_authorsLabel(this),
    m_okButton(this)
{
	set_class_name("AuthorsScene");

	m_logoSprite = CL_Sprite(get_gc(), "menu/logo", Gfx::Stage::getResourceManager());

	int x = 200;
	int y = m_logoSprite.get_height();
	m_authorsLabel.set_span(getAuthorsSpan());
	m_authorsLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));

	x = (get_width() - BUTTON_WIDTH) / 2;
	y = get_height() - (V_MARGIN + BUTTON_HEIGHT);

	m_okButton.set_geometry(CL_Rect(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT));
	m_okButton.set_text(_("Ok"));


	m_okButton.func_clicked().set(this, &AuthorsScene::onOkClick);
}

AuthorsScene::~AuthorsScene()
{
}

CL_SpanLayout AuthorsScene::getAuthorsSpan()
{
	CL_SpanLayout span;
	CL_String type, authors;
	
	CL_Colorf color(CL_Colorf::black);
	CL_Font typeFont(get_gc(), "helvetica", 30);
	CL_Font authorsFont(get_gc(), "helvetica", 20);

	type = "Programmers:\n";
	authors = "chudzielec\nbercik\n";
	span.add_text(type, typeFont, color);
	span.add_text(authors, authorsFont, color);

	type = "Graphics:\n";
	authors = "mrchipples\n";
	span.add_text(type, typeFont, color);
	span.add_text(authors, authorsFont, color);

	return span;
}

void AuthorsScene::load(CL_GraphicContext &p_gc)
{
	GuiScene::load(p_gc);
}

void AuthorsScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0.0f, 0.0f, get_width(), get_height(), CL_Colorf::white);

	const float w = Gfx::Stage::getWidth();
	const float h = w / m_logoSprite.get_width() * m_logoSprite.get_height();

	CL_Rectf spriteRect(0, 0, w, h);
	m_logoSprite.draw(p_gc, spriteRect);

	GuiScene::draw(p_gc);
}

void AuthorsScene::onOkClick()
{
    INVOKE_0(okClicked);
}