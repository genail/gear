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

const int LABEL_WIDTH = 280;
const int LABEL_HEIGHT = 200;

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
	m_leadProgramerLabel(this),
	m_programersLabel(this),
	m_graphicsLabel(this),
    m_okButton(this),
	isDown(true),
	isRight(true),
	m_timer()
{
	set_class_name("AuthorsScene");

	typeFont = CL_Font(get_gc(), "helvetica", 40);
	authorsFont = CL_Font(get_gc(), "helvetica", 25);
	fontColor = CL_Colorf::black;

	CL_SpanLayout span;
	CL_Size size;

	int y = 0;
	int x = 0;

	span = getLeadProgramerSpan();
	size = span.find_preferred_size(get_gc());
	m_leadProgramerLabel.set_span(span);
	m_leadProgramerLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + size.height));

	y += size.height;

	span = getProgramersSpan();
	size = span.find_preferred_size(get_gc());
	m_programersLabel.set_span(span);
	m_programersLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + size.height));

	y += size.height;

	span = getGraphicsSpan();
	size = span.find_preferred_size(get_gc());
	m_graphicsLabel.set_span(span);
	m_graphicsLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + size.height));

	x = (get_width() - BUTTON_WIDTH) / 2;
	y = get_height() - (V_MARGIN + BUTTON_HEIGHT);

	m_okButton.set_geometry(CL_Rect(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT));
	m_okButton.set_text(_("Ok"));

	m_okButton.func_clicked().set(this, &AuthorsScene::onOkClick);

	max = y;

	m_timer.func_expired().set(this, &AuthorsScene::onTimerInterval);
	m_timer.start(40);
}

AuthorsScene::~AuthorsScene()
{
}

CL_SpanLayout AuthorsScene::getLeadProgramerSpan()
{
	CL_SpanLayout span;
	CL_String type, authors;

	type = _("Lead programer:\n");
	authors = _("Piotr Korzuszek");
	span.add_text(type, typeFont, fontColor);
	span.add_text(authors, authorsFont, fontColor);

	return span;
}

CL_SpanLayout AuthorsScene::getProgramersSpan()
{
	CL_SpanLayout span;
	CL_String type, authors;

	type = _("Programers:\n");
	authors = _("Robert Cebula\n");
	authors += _("Bartosz Platak\n");
	authors += _("Pawe³ Rybarczyk");
	span.add_text(type, typeFont, fontColor);
	span.add_text(authors, authorsFont, fontColor);

	return span;
}

CL_SpanLayout AuthorsScene::getGraphicsSpan()
{
	CL_SpanLayout span;
	CL_String type, authors;

	type = _("Graphics:\n");
	authors = _("Piotr Uchman");
	span.add_text(type, typeFont, fontColor);
	span.add_text(authors, authorsFont, fontColor);

	return span;
}

void AuthorsScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0.0f, 0.0f, get_width(), get_height(), CL_Colorf::white);

	GuiScene::draw(p_gc);
}

void AuthorsScene::onOkClick()
{
    INVOKE_0(okClicked);
}

void AuthorsScene::onTimerInterval()
{
	int topChange = 0;
	int rightChange = 0;

	if (isRight)
	{
		if (m_leadProgramerLabel.get_geometry().right >= get_width())
		{
			isRight = false;
			return;
		}

		rightChange = 1;
	}
	else
	{
		if (m_leadProgramerLabel.get_geometry().left <= 0)
		{
			isRight = true;
			return;
		}

		rightChange = -1;
	}

	if (isDown)
	{
		if (m_graphicsLabel.get_geometry().bottom >= max)
		{
			isDown = false;
			return;
		}

		topChange = 1;
	}
	else
	{
		if (m_leadProgramerLabel.get_geometry().top <= 0)
		{
			isDown = true;
			return;
		}

		topChange = -1;
	}

	ChangeLabelPosition(&m_leadProgramerLabel, topChange, rightChange);
	ChangeLabelPosition(&m_programersLabel, topChange, rightChange);
	ChangeLabelPosition(&m_graphicsLabel, topChange, rightChange);
}

void AuthorsScene::ChangeLabelPosition(CL_Label *p_label, int p_changeTop, int p_changeLeft)
{
	CL_Rect rect = (*p_label).get_geometry();
	rect.top += p_changeTop;
	rect.bottom += p_changeTop;
	rect.left += p_changeLeft;
	rect.right += p_changeLeft;
	(*p_label).set_geometry(rect);

	(*p_label).paint();
}