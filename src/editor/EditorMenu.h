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
#include <ClanLib/gui.h>
#include <ClanLib/display.h>

#include "gfx/Overlay.h"
#include "editor/EditorLogic.h"

namespace Gfx {

class EditorMenu : public Overlay {

	public:

		enum Action
		{
			Save,
			Load
		};

		EditorMenu(DirectScene &p_scene, Editor::EditorLogic& p_editorLogic);

		virtual ~EditorMenu();


		virtual void load(CL_GraphicContext &p_gc);

		virtual void draw(CL_GraphicContext &p_gc, const CL_Rect &p_clip);

	private:

		/** Background */
		CL_RoundedRect m_bg;

		// gui

		CL_PushButton m_loadButton;

		CL_PushButton m_saveButton;

		CL_PushButton m_helpButton;

		CL_PushButton m_testButton;

		CL_PushButton m_exitButton;

		CL_PushButton m_helpOkButton;

		CL_PushButton m_okButton;

		CL_PushButton m_cancelButton;

		CL_LineEdit m_fileNameLineEdit;

		CL_Label m_errorLabel;

		CL_Label m_helpLabel;

		CL_Label m_informationLabel;

		// help variables

		Editor::EditorLogic& m_editorLogic;

		Action m_action;

		// colors

		CL_Colorf m_errorColor;
		CL_Colorf m_messageColor;

		// methods

		void setVisibleMenu(bool visible);

		void setVisibleSaveLoad(bool visible);

		void setVisibleHelp(bool visible);

		void showMenu();

		void showSaveLoad();

		void showHelp();

		void displayError(CL_String p_message, CL_Colorf p_color);

		void setInformationText(CL_String p_text);

		void setHelpText();

		// events

		void onSaveClicked();

		void onLoadClicked();

		void onExitClicked();

		void onOkClicked();

		void onCancelClicked();

		void onHelpClicked();

		void onTestClicked();

		void onHelpOkClicked();

		void onVisibleChanged(bool visible);
};

}

