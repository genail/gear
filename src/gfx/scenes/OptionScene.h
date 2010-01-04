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
#pragma once

#include <ClanLib/core.h>
#include <vector>

#include "common.h"
#include "gfx/GuiScene.h"
#include "controllers/OptionController.h"

class OptionScene : public Gfx::GuiScene
{
        SIGNAL_0(okClicked);

        SIGNAL_0(cancelClicked);

	public:
		OptionScene(CL_GUIComponent *p_parent);

		virtual ~OptionScene();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

		void displayError(const CL_String& p_message);

		void onLoad();
		
		void addResolution(int p_width, int p_height);

		CL_String getPlayersName() const { return m_nameLineEdit.get_text(); }

		int getResolutionWidth() const { return m_resolutions[m_resolutionComboBox.get_selected_item()].width; }

		int getResolutionHeight() const { return m_resolutions[m_resolutionComboBox.get_selected_item()].height; }

		bool getFullScreen() const { return m_fullScreenCheckBox.is_checked(); }

		int getSound() const { return m_soundSlider.get_position(); }

		bool getWSAD() const { return m_wsadCheckBox.is_checked(); }

	private:

		std::vector<CL_Size> m_resolutions;

		// scene controller

		OptionController m_controller;

		// gui components
        
        CL_PushButton m_okButton;

        CL_PushButton m_cancelButton;

		CL_Label m_nameLabel;

		CL_LineEdit m_nameLineEdit;

		CL_Label m_resolutionLabel;

		CL_ComboBox m_resolutionComboBox;

		CL_CheckBox m_fullScreenCheckBox;

		CL_Label m_soundLabel;

		CL_Label m_soundValueLabel;

		CL_Slider m_soundSlider;

		CL_CheckBox m_wsadCheckBox;

		CL_Label m_errorLabel;

		CL_PopupMenu m_menu;

		// Methods

		int searchResolution(const int& p_searchWidth, const int& p_searchHeight);

		void setSliderLabelValue();

		void useDefaultSetings();

		// action slots

        void onOkClick();

        void onCancelClick();

		void onSliderValueChange();
};