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

#include <list>

#include <ClanLib/core.h>
#include <ClanLib/gui.h>

namespace Gfx {
class Scene;

class GameWindow : public CL_GUIComponent {
	public:

		GameWindow(CL_GUIManager *p_guiMgr, CL_GUIWindowManagerTexture *p_winMgr, CL_InputContext *p_ic);

		virtual ~GameWindow();

		bool update();

		void draw(CL_GraphicContext &p_gc);

	private:

		CL_GUIWindowManagerTexture *m_winMgr;

		CL_GUIManager *m_guiMgr;

		CL_InputContext *m_ic;

		/** Last update logic time. When 0 then no logic update has been done before. */
		unsigned m_lastLogicUpdateTime;

		// scene flow

		/** Last scene */
		Scene *m_lastScene;

		CL_SlotContainer m_slotContainer;

		//
		// workaround for missing implementation for repead_count of CL_InputEvent
		// remove this if http://www.rtsoft.com/forums/showthread.php?t=2839 will
		// be commited to ClanLib release
		//

		std::list<CL_InputEvent> m_events;

		//
		// methods
		//

		void dispatchEvents();

		void renderGui(CL_GraphicContext &p_gc, const CL_Rect &p_clipRect);

		void repaint();

		void updateLogic(Scene *p_scene);

		void renderScene(CL_GraphicContext &p_gc, Scene *p_scene);


		// signal handlers

		void onKeyDown(
				const CL_InputEvent &p_event,
				const CL_InputState &p_state
		);

		void onKeyUp(
				const CL_InputEvent &p_event,
				const CL_InputState &p_state
		);

		void onMouseMove(
				const CL_InputEvent &p_event,
				const CL_InputState &p_state
		);

};

} // namespace
