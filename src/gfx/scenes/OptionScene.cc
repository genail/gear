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

#include "OptionScene.h"

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

const int COMBOBOX_WIDTH = 200;
const int COMBOBOX_HEIGHT = 20;

const int CHECKBOX_WIDTH = 200;
const int CHECKBOX_HEIGHT = 20;

const int SLIDER_WIDTH = 200;
const int SLIDER_HEIGHT = 20;

const int H_MARGIN = 20;
const int V_MARGIN = 40;

OptionScene::OptionScene(CL_GUIComponent *p_parent) :
	GuiScene(p_parent),
    m_controller(this),
    m_cancelButton(this),
    m_okButton(this),
	m_nameLabel(this),
	m_nameLineEdit(this),
	m_resolutionLabel(this),
	m_resolutionComboBox(this),
	m_fullScreenCheckBox(this),
	m_soundLabel(this),
	m_soundValueLabel(this),
	m_soundSlider(this)
{
	set_class_name("OptionScene");

	static const int START_X = 240;
	static const int START_Y = V_MARGIN;

	int x = START_X;
	int y = START_Y;

	m_nameLabel.set_text("Player's name");
	m_nameLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));

	x += LABEL_WIDTH + H_MARGIN;

	m_nameLineEdit.set_geometry(CL_Rect(x, y, x + EDIT_WIDTH, y + EDIT_HEIGHT));

	x = START_X;
	y += V_MARGIN;

	m_resolutionLabel.set_text("Resolution");
	m_resolutionLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));

	x += LABEL_WIDTH + H_MARGIN;

	// bercik 3 sty.
	// do zmiany na wczytywanie rozdzielczoœci z pliku resolution.cpp lub .h
	CL_PopupMenu menu;
	menu.insert_item("800x600");
	menu.insert_item("1024x768");
	m_resolutionComboBox.set_popup_menu(menu);
	m_resolutionComboBox.set_geometry(CL_Rect(x, y, x + COMBOBOX_WIDTH, y + COMBOBOX_HEIGHT));
	m_resolutionComboBox.set_selected_item(0);

	x = START_X;
	y += V_MARGIN;

	m_fullScreenCheckBox.set_text("Full screen");
	m_fullScreenCheckBox.set_geometry(CL_Rect(x, y, x + CHECKBOX_WIDTH, y + CHECKBOX_HEIGHT));

	x = START_X;
	y += V_MARGIN;

	m_soundLabel.set_text("Sound");
	m_soundLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));

	x += LABEL_WIDTH + H_MARGIN;

	m_soundSlider.set_min(0);
	m_soundSlider.set_max(100);
	m_soundSlider.set_geometry(CL_Rect(x, y, x + SLIDER_WIDTH, y + SLIDER_HEIGHT));

	x += SLIDER_WIDTH + H_MARGIN;

	m_soundValueLabel.set_geometry(CL_Rect(x, y, x + LABEL_WIDTH, y + LABEL_HEIGHT));

    x = (Gfx::Stage::getWidth() - (2 * BUTTON_WIDTH + H_MARGIN)) / 2;
    y = Gfx::Stage::getHeight() - (BUTTON_HEIGHT + V_MARGIN);

    m_okButton.set_text("Ok");
    m_okButton.set_geometry(CL_Rect(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT));

    x += V_MARGIN + BUTTON_WIDTH;

    m_cancelButton.set_text("Cancel");
    m_cancelButton.set_geometry(CL_Rect(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT));

	m_soundSlider.func_value_changed().set(this, &OptionScene::onSliderValueChange);
	m_okButton.func_clicked().set(this, &OptionScene::onOkClick);
	m_cancelButton.func_clicked().set(this, &OptionScene::onCancelClick);
}

OptionScene::~OptionScene()
{
}

void OptionScene::load(CL_GraphicContext &p_gc)
{
	GuiScene::load(p_gc);
}


void OptionScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0.0f, 0.0f, get_width(), get_height(), CL_Colorf::white);

	GuiScene::draw(p_gc);
}

void OptionScene::onCancelClick()
{
    INVOKE_0(cancelClicked);
}

void OptionScene::onOkClick()
{
    INVOKE_0(okClicked);
}

void OptionScene::onSliderValueChange()
{
	m_soundValueLabel.set_text(CL_StringHelp::int_to_local8(m_soundSlider.get_position()));
}