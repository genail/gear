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

#include "EditorMenu.h"

#include "common.h"
#include "gfx/Stage.h"

namespace Gfx {

const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 250;
const float WINDOW_ROUNDNESS = 20.0f;
const int MARGIN = 20;
const int BUTTON_HEIGHT = 30;
const int BUTTON_SPACE = 15;
const int EDIT_HEIGHT = 20;
const int ERROR_LABEL_HEIGHT = 50;
const int LABEL_HEIGHT = 30;

EditorMenu::EditorMenu(DirectScene &p_scene, Editor::EditorLogic& p_editorLogic) :
	Overlay(
			p_scene,
			CL_Rect(
					(int) (Stage::getWidth()  / 2 - WINDOW_WIDTH  / 2),
					(int) (Stage::getHeight() / 2 - WINDOW_HEIGHT / 2),
					(int) (Stage::getWidth()  / 2 + WINDOW_WIDTH  / 2),
					(int) (Stage::getHeight() / 2 + WINDOW_HEIGHT / 2)
			)
	),
	m_editorLogic(p_editorLogic),
	m_bg(CL_Sizef(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_ROUNDNESS),
	m_loadButton(this),
	m_saveButton(this),
	m_exitButton(this),
	m_helpOkButton(this),
	m_okButton(this),
	m_cancelButton(this),
	m_helpButton(this),
	m_testButton(this),
	m_fileNameLineEdit(this),
	m_errorLabel(this),
	m_helpLabel(this),
	m_informationLabel(this),
	m_action()
{
	setVisible(false);

	func_visibility_change().set(this, &EditorMenu::onVisibleChanged);
}

EditorMenu::~EditorMenu()
{
	// empty
}

void EditorMenu::load(CL_GraphicContext &p_gc)
{
	int	y = MARGIN;

	m_loadButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_loadButton.set_text(_("Load"));
	m_loadButton.func_clicked().set(this, &EditorMenu::onLoadClicked);


	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_saveButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_saveButton.set_text(_("Save"));
	m_saveButton.func_clicked().set(this, &EditorMenu::onSaveClicked);


	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_helpButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_helpButton.set_text(_("Help"));
	m_helpButton.func_clicked().set(this, &EditorMenu::onHelpClicked);


	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_testButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_testButton.set_text(_("Test"));
	m_testButton.func_clicked().set(this, &EditorMenu::onTestClicked);


	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_exitButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_exitButton.set_text(_("Exit"));
	m_exitButton.func_clicked().set(this, &EditorMenu::onExitClicked);


	y = MARGIN;

	m_informationLabel.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + LABEL_HEIGHT));
	m_informationLabel.set_alignment(CL_Label::align_center);


	y += LABEL_HEIGHT + MARGIN;
	int x1 = MARGIN;
	int x2 = WINDOW_WIDTH - MARGIN;

	m_fileNameLineEdit.set_geometry(CL_Rect(x1, y, x2, y + EDIT_HEIGHT));


	y += EDIT_HEIGHT + MARGIN;

	m_errorLabel.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + ERROR_LABEL_HEIGHT));


	y = WINDOW_HEIGHT - (2 * BUTTON_HEIGHT + BUTTON_SPACE + MARGIN);

	m_okButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_okButton.set_text(_("OK"));
	m_okButton.func_clicked().set(this, &EditorMenu::onOkClicked);

	
	y += BUTTON_HEIGHT + BUTTON_SPACE;

	m_cancelButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_cancelButton.set_text(_("Cancel"));
	m_cancelButton.func_clicked().set(this, &EditorMenu::onCancelClicked);


	int y1 = MARGIN;
	int y2 = WINDOW_HEIGHT - (2 * MARGIN + BUTTON_HEIGHT);
	x1 = MARGIN;
	x2 = WINDOW_WIDTH - MARGIN;

	m_helpLabel.set_geometry(CL_Rect(x1, y1, x2, y2));
	setHelpText();


	y = WINDOW_HEIGHT - (MARGIN + BUTTON_HEIGHT);
	
	m_helpOkButton.set_geometry(CL_Rect(MARGIN, y, WINDOW_WIDTH - MARGIN, y + BUTTON_HEIGHT));
	m_helpOkButton.set_text(_("OK"));
	m_helpOkButton.func_clicked().set(this, &EditorMenu::onHelpOkClicked);


	showMenu();
}

void EditorMenu::draw(CL_GraphicContext &p_gc, const CL_Rect &p_clip)
{
	static const CL_Colorf FILL_COLOR(1.0f, 1.0f, 1.0f, 0.9f);
	static const CL_Pointf ZERO_POINT(0, 0);

	m_bg.fill(p_gc, ZERO_POINT, FILL_COLOR);
}

void EditorMenu::onSaveClicked()
{
	m_action = Save;
	displayError("");
	setInformationText(_("Save"));
	m_fileNameLineEdit.set_text("");
	m_fileNameLineEdit.set_focus();
	showSaveLoad();
}

void EditorMenu::onLoadClicked()
{
	m_action = Load;
	displayError("");
	setInformationText(_("Load"));
	m_fileNameLineEdit.set_text("");
	m_fileNameLineEdit.set_focus();
	showSaveLoad();
}

void EditorMenu::onExitClicked()
{
	setVisible(false);
	Gfx::Stage::popScene();
}

void EditorMenu::onOkClicked()
{
	CL_String fileName = m_fileNameLineEdit.get_text();

	if (fileName != "")
	{
		fileName += ".map";

		switch (m_action)
		{
		case Load:
			if (!m_editorLogic.load(fileName))
				displayError("Cannot load map " + fileName);
			else
				displayError("Map " + fileName + " load");
			break;
		case Save:
			m_editorLogic.save(fileName);
			displayError("Map " + fileName + " saved");
			break;
		}
	}
}

void EditorMenu::onCancelClicked()
{
	showMenu();
}

void EditorMenu::onHelpClicked()
{
	showHelp();
}

void EditorMenu::onTestClicked()
{
	setVisible(false);
	m_editorLogic.startTest();
}

void EditorMenu::onHelpOkClicked()
{
	showMenu();
}

void EditorMenu::onVisibleChanged(bool visible)
{
	if (visible)
	{
		showMenu();
	}
}

void EditorMenu::displayError(CL_String p_message)
{
	CL_Font font(get_gc(), "helvetica", 14);
	CL_SpanLayout span;
	span.add_text(p_message, font, CL_Colorf::red);
	m_errorLabel.set_span(span);
}

void EditorMenu::setInformationText(CL_String p_text)
{
	CL_Font font(get_gc(), "helvetica", 30);
	CL_SpanLayout span;
	span.add_text(p_text, font, CL_Colorf::black);
	m_informationLabel.set_span(span);
}

void EditorMenu::setHelpText()
{
	CL_String text = "";
	text += "Add point:\n";
	text += "CTRL + LEFT MOUSE or MIDDLE MOUSE\n";
	text += "Delete point:\n";
	text += "DELETE or D\n";
	text += "Change radius:\n";
	text += "CTRL + MOUSE ROLL\\'+','-'\n";
	text += "Change shift:\n";
	text += "SHIFT\n";
	text += "Change scale:\n";
	text += "MOUSE ROLL or '+','-'\n";
	text += "Go to start line:\n";
	text += "SPACE";

	m_helpLabel.set_text(text);
}

void EditorMenu::setVisibleMenu(bool visible)
{
	m_loadButton.set_visible(visible);
	m_saveButton.set_visible(visible);
	m_exitButton.set_visible(visible);
	m_testButton.set_visible(visible);
	m_helpButton.set_visible(visible);
}

void EditorMenu::setVisibleSaveLoad(bool visible)
{
	m_fileNameLineEdit.set_visible(visible);
	m_okButton.set_visible(visible);
	m_cancelButton.set_visible(visible);
	m_errorLabel.set_visible(visible);
	m_informationLabel.set_visible(visible);
}

void EditorMenu::setVisibleHelp(bool visible)
{
	m_helpLabel.set_visible(visible);
	m_helpOkButton.set_visible(visible);
}

void EditorMenu::showMenu()
{
	setVisibleMenu(true);
	setVisibleSaveLoad(false);
	setVisibleHelp(false);
}

void EditorMenu::showSaveLoad()
{
	setVisibleMenu(false);
	setVisibleSaveLoad(true);
	setVisibleHelp(false);
}

void EditorMenu::showHelp()
{
	setVisibleMenu(false);
	setVisibleSaveLoad(false);
	setVisibleHelp(true);
}


}
