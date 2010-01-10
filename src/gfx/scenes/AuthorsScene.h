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

#include "common.h"
#include "gfx/GuiScene.h"
#include "controllers/AuthorsController.h"

class AuthorsScene : public Gfx::GuiScene
{
        SIGNAL_0(okClicked);

	public:
		AuthorsScene(CL_GUIComponent *p_parent);

		virtual ~AuthorsScene();

		virtual void draw(CL_GraphicContext &p_gc);

	private:

		bool m_isDown;

		bool m_isRight;
		
		int m_max;

		CL_Timer m_timer;

		CL_Font m_typeFont;

		CL_Font m_authorsFont;
		
		CL_Colorf m_fontColor;

		// scene controller

		AuthorsController m_controller;

		// gui components

		CL_Label m_leadProgramerLabel;

		CL_Label m_programersLabel;

		CL_Label m_graphicsLabel;

		CL_PushButton m_okButton;

		// Methods

		CL_SpanLayout getLeadProgramerSpan();

		CL_SpanLayout getProgramersSpan();

		CL_SpanLayout getGraphicsSpan();

		// action slots

        void onOkClick();

		void onTimerInterval();

		void changeLabelPosition(CL_Label* p_label, int p_changeTop, int p_changeLeft);
};
