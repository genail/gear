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

const int EXIT_CODE_QUIT = 1;

GameWindow::GameWindow(CL_GUIManager *p_guiMgr, CL_GUIWindowManagerTexture *p_winMgr, CL_InputContext *p_ic) :
	CL_GUIComponent(p_guiMgr, CL_GUITopLevelDescription("GameWindow", CL_Rect(0, 0, Stage::getWidth(), Stage::getHeight()), false)),
	m_winMgr(p_winMgr),
	m_guiMgr(p_guiMgr),
	m_ic(p_ic),
	m_lastLogicUpdateTime(0),
	m_lastScene(NULL)
{
	CL_InputDevice &keyboard = m_ic->get_keyboard();

	m_slotContainer.connect(keyboard.sig_key_down(), this, &GameWindow::onKeyDown);
	m_slotContainer.connect(keyboard.sig_key_up(), this, &GameWindow::onKeyUp);
}

GameWindow::~GameWindow()
{
}

void GameWindow::repaint()
{
	//request_repaint();
}

bool GameWindow::update()
{
	const int result = m_guiMgr->exec(false);

	if (result == EXIT_CODE_QUIT) {
		return false;
	}

	dispatchEvents();

	Scene *scene = Gfx::Stage::peekScene();
	updateLogic(scene);

	return true;
}

void GameWindow::draw(CL_GraphicContext &p_gc)
{
	Scene *scene = Gfx::Stage::peekScene();
	renderScene(p_gc, scene);

	m_winMgr->draw_windows(p_gc);
}


void GameWindow::updateLogic(Scene *p_scene)
{

	if (p_scene != NULL) {

		if (p_scene->isLoaded()) { // update only when loaded

			if (p_scene != m_lastScene) {

				if (m_lastScene != NULL) {
					m_lastScene->setActive(false);
				}

				m_lastScene = p_scene;

				p_scene->setActive(true);
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
		exit_with_code(EXIT_CODE_QUIT);
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

//		if (!p_scene->isGui()) {
			p_scene->draw(p_gc);
//		}

	}
}

void GameWindow::onKeyDown(const CL_InputEvent &p_event, const CL_InputState &p_state)
{
	m_events.push_back(p_event);

	// uncoment when repeat_count fixed
//	Scene *scene = Gfx::Stage::peekScene();
//
//	if (scene != NULL) {
//		scene->inputPressed(p_event);
//	}
}

void GameWindow::onKeyUp(const CL_InputEvent &p_event, const CL_InputState &p_state)
{
	m_events.push_back(p_event);

	// uncoment when repeat_count fixed
//	Scene *scene = Gfx::Stage::peekScene();
//
//	if (scene != NULL) {
//		scene->inputReleased(p_event);
//	}
}

void GameWindow::dispatchEvents()
{
	Scene *scene = Gfx::Stage::peekScene();

	if (scene != NULL) {
		std::vector<int> usedKeys;

		std::list<CL_InputEvent>::reverse_iterator ritor = m_events.rbegin();
		bool used;

		for (;ritor != m_events.rend(); ++ritor) {
			used = false;
			foreach (int k, usedKeys) {
				if (ritor->id == k) {
					used = true;
					break;
				}
			}

			if (!used) {
				if (ritor->type == CL_InputEvent::pressed) {
					scene->inputPressed(*ritor);
				} else {
					scene->inputReleased(*ritor);
				}

				usedKeys.push_back(ritor->id);
			}
		}
	}

	m_events.clear();
}

} // namespace
