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

#include "EditorScene.h"

#include "common.h"
#include "gfx/Stage.h"
#include "editor/EditorHelper.h"
#include "editor/EditorManagement.h"

using namespace Editor;
using namespace Gfx;

class EditorSceneImpl
{
public:
	EditorSceneImpl() : 
			m_controller(NULL),
			m_editorManagement()
	{

	}

	~EditorSceneImpl()
	{

	}

	// editor variables

	EditorManagement m_editorManagement;

	// scene controller

	EditorController m_controller;

	// gui components


	// Methods

	void handleInput(InputState p_state, const CL_InputEvent& p_event);

	void mouseMoved(const CL_Point &p_pos);

	void draw(CL_GraphicContext &p_gc);

	void load(CL_GraphicContext &p_gc);

	void update(unsigned int p_timeElapsed);

	// action slots
};

void EditorSceneImpl::draw(CL_GraphicContext &p_gc)
{

}

void EditorSceneImpl::load(CL_GraphicContext &p_gc)
{

}

void EditorSceneImpl::update(unsigned int p_timeElapsed)
{

}

void EditorSceneImpl::handleInput(InputState p_state, const CL_InputEvent& p_event)
{

}

void EditorSceneImpl::mouseMoved(const CL_Point &p_pos)
{

}

EditorScene::EditorScene() :
	m_impl(new EditorSceneImpl())
{
	m_impl->m_controller = EditorController(this);
}

EditorScene::~EditorScene()
{
}

void EditorScene::draw(CL_GraphicContext &p_gc)
{
	DirectScene::draw(p_gc);

	CL_Draw::fill(p_gc, 0.0f, 0.0f, Stage::getWidth(), Stage::getHeight(), CL_Colorf::black);

	m_impl->draw(p_gc);
}

void EditorScene::load(CL_GraphicContext &p_gc)
{
	DirectScene::load(p_gc);

	m_impl->load(p_gc);
}

void EditorScene::update(unsigned int p_timeElapsed)
{
	m_impl->update(p_timeElapsed);
}

void EditorScene::inputPressed(const CL_InputEvent &p_event)
{
	m_impl->handleInput(Pressed, p_event);
}

void EditorScene::inputReleased(const CL_InputEvent &p_event)
{
	m_impl->handleInput(Released, p_event);
}

void EditorScene::mouseMoved(const CL_Point &p_pos)
{
	m_impl->mouseMoved(p_pos);
}