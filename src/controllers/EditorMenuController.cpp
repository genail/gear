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

#include "EditorMenuController.h"

#include "gfx/Stage.h"
#include "editor/EditorMenu.h"
#include "editor/EditorLogic.h"

class EditorMenuControllerImpl
{
	public:

		Gfx::EditorMenu *m_editorMenu;

		Editor::EditorLogic& m_editorLogic;

		EditorMenuControllerImpl(
			Gfx::EditorMenu *p_editorMenu,
			Editor::EditorLogic& p_editorLogic
		) :
			m_editorMenu(p_editorMenu),
			m_editorLogic(p_editorLogic)
		{
			m_editorMenu->func_exit_clicked().set(
					this,
					&EditorMenuControllerImpl::onExitClicked
			);

			m_editorMenu->func_load_clicked().set(
					this,
					&EditorMenuControllerImpl::onLoadClicked
			);

			m_editorMenu->func_save_clicked().set(
					this,
					&EditorMenuControllerImpl::onSaveClicked
			);
		}


		void onExitClicked();

		void onLoadClicked();

		void onSaveClicked();

	private:

};

EditorMenuController::EditorMenuController(
	Gfx::EditorMenu *p_editorMenu,
	Editor::EditorLogic& p_editorLogic)
	:
	m_impl(new EditorMenuControllerImpl(p_editorMenu, p_editorLogic))
{
	// empty
}

EditorMenuController::~EditorMenuController()
{
	// empty
}

void EditorMenuControllerImpl::onExitClicked()
{
	m_editorMenu->setVisible(false);
	Gfx::Stage::popScene();
}

void EditorMenuControllerImpl::onLoadClicked()
{
	m_editorMenu->setVisible(false);
}

void EditorMenuControllerImpl::onSaveClicked()
{
	m_editorMenu->setVisible(false);
}
