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

#include "GameWindow.h"

#include "common.h"
#include "gfx/Scene.h"
#include "gfx/Stage.h"
#include "common/Properties.h"

namespace Gfx {

GameWindow::GameWindow(CL_GUIManager *p_manager, const CL_DisplayWindowDescription &p_desc, CL_GraphicContext *p_nativeGC) :
	CL_Window(p_manager, p_desc),
	m_lastLogicUpdateTime(0),
	m_nativeGC(p_nativeGC),
	m_lastScene(NULL)
{
	func_render().set(this, &GameWindow::onRender);
	set_constant_repaint(true);
}

GameWindow::~GameWindow()
{
}

void GameWindow::repaint()
{
	request_repaint();
}

void GameWindow::r(CL_GraphicContext &p_gc)
{
	Scene *scene = Gfx::Stage::peekScene();

	renderScene(p_gc, scene);
}


void GameWindow::onRender(CL_GraphicContext &p_gc, const CL_Rect &p_clipRect)
{
	Scene *scene = Gfx::Stage::peekScene();

	updateLogic(scene);

	if (!scene->isGui()) {
//		cl_log_event(LOG_DEBUG, "native");
//		renderScene(*m_nativeGC, scene);
	} else {
//		renderScene(p_gc, scene);
	}
}

void GameWindow::updateLogic(Scene *p_scene)
{

	if (p_scene != NULL) {

		if (p_scene->isLoaded()) { // update only when loaded

			if (p_scene != m_lastScene) {

				if (m_lastScene != NULL) {
					m_lastScene->set_visible(false);
					m_lastScene->set_focus(false);
				}

				// set the scene visibility and focus
				p_scene->set_visible(true);
				p_scene->set_focus(true);

				m_lastScene = p_scene;
			}

			const unsigned now = CL_System::get_time();

			if (m_lastLogicUpdateTime == 0) {
				p_scene->update(0);
			} else {
				const unsigned timeChange = now - m_lastLogicUpdateTime;

#if !defined(NDEBUG)
				// apply iteration time change
				static float timeChangeF = 0.0f;
				const int iterSpeed = Properties::getPropertyAsInt("dbg_iterSpeed", 100);

				timeChangeF += timeChange * (iterSpeed / 100.0f);

				if (timeChangeF >= 1.0f) {
					const float total = floor(timeChangeF);
					p_scene->update((unsigned) total);

					timeChangeF -= total;
				}
#else // !NDEBUG
				p_scene->update(timeChange);
#endif // NDEBUG
			}

			m_lastLogicUpdateTime = now;
		}

	} else {
		// when there are no scenes on stack, then probably application
		// should be ended
		cl_log_event(LOG_DEBUG, "Closing application because of empty scene stack");
		exit_with_code(0);
	}
}

void GameWindow::renderScene(CL_GraphicContext &p_gc, Scene *p_scene)
{
	if (p_scene != NULL) {

		// load scene when not loaded yet
		if (!p_scene->isLoaded()) {
			cl_log_event("debug", "load()");
			p_scene->load(p_gc);
		}

		p_scene->draw(p_gc);

	}
}

} // namespace
