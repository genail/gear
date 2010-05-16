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
#pragma once

#include <ClanLib/core.h>

#include "common.h"
#include "common/Properties.h"
#include "gfx/GuiScene.h"
#include "controllers/MainMenuController.h"
#include "gfx/MessageBox.h"

class MainMenuScene : public Gfx::GuiScene
{
		SIGNAL_0(startRaceClicked);
		SIGNAL_0(quitClicked);
		SIGNAL_0(optionClicked);
		SIGNAL_0(authorsClicked);
		SIGNAL_0(editorClicked);
		SIGNAL_0(findServersClicked);

	public:
		MainMenuScene(CL_GUIComponent *p_parent);
		virtual ~MainMenuScene();

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);

		virtual void pushed();

		CL_String getServerAddr() const { return m_serverLineEdit.get_text(); }

		void displayError(const CL_String& p_message);
		void displayConnectingMessageBox();
		void displayConnectionErrorMessageBox();
		void hideMessageBox();

	private:

		// scene controller

		MainMenuController m_controller;

		// gui components

		CL_Label m_serverLabel;
		CL_LineEdit m_serverLineEdit;
		CL_PushButton m_okButton;
		CL_PushButton m_findServersButton;
		CL_Label m_errorLabel;
		CL_PushButton m_optionButton;
		CL_PushButton m_authorsButton;
		CL_PushButton m_editorButton;
		CL_PushButton m_quitButton;
		Gfx::MessageBox m_messageBox;


		// logo

		CL_Sprite m_logoSprite;

		//
		// Methods
		//

		// action slots

		void onOkClicked();
		void onQuitClicked();
		void onOptionClicked();
		void onAuthorsClicked();
		void onEditorClicked();
		void onFindServersClicked();

};
