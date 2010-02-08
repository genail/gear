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

#include "EditorBase.h"

namespace Editor
{
	EditorBase::EditorBase(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Race::Track& p_track, Gfx::Viewport& p_viewport)
		:
		m_keys(),
		m_raceLevel(p_raceLevel),
		m_gfxLevel(p_gfxLevel),
		m_track(p_track),
		m_viewport(p_viewport)
	{

	}

	EditorBase::~EditorBase()
	{

	}

	void EditorBase::draw(CL_GraphicContext &p_gc)
	{
		onDraw(p_gc);
	}

	void EditorBase::load(CL_GraphicContext &p_gc)
	{
		onLoad(p_gc);
	}

	void EditorBase::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_inconditionalMousePos)
	{
		m_lastMousePos = m_mousePos;
		m_mousePos = p_mousePos;
		m_deltaMousePos = m_mousePos - m_lastMousePos;

		m_inconditionalLastMousePos = m_inconditionalMousePos;
		m_inconditionalMousePos = p_inconditionalMousePos;
		m_inconditionalDeltaMousePos = m_inconditionalMousePos - m_inconditionalLastMousePos;

		onMouseMoved();
	}

	void EditorBase::mouseScrolled(bool p_up)
	{
		onMouseScrolled(p_up);
	}

	void EditorBase::update(unsigned int p_timeElapsed)
	{
		onUpdate(p_timeElapsed);
	}

	void EditorBase::handleInput(InputState p_state, const CL_InputEvent& p_event)
	{
		switch (p_state)
		{
		case Pressed:
			addKey(p_event.id);
			break;
		case Released:
			removeKey(p_event.id);
			break;
		}

		onHandleInput();
	}

	bool EditorBase::hasContainsKey(int p_key)
	{
		if (findKey(p_key) != -1)
			return true;
		else
			return false;
	}

	const std::vector<int>& EditorBase::getKeys() const
	{
		return m_keys;
	}

	void EditorBase::addKey(int p_key)
	{
		if (!hasContainsKey(p_key))
			m_keys.push_back(p_key);
	}

	void EditorBase::removeKey(int p_key)
	{
		std::vector<int>::iterator it = m_keys.begin();

		if (hasContainsKey(p_key))
		{
			it += findKey(p_key);

			m_keys.erase(it);
		}
	}

	int EditorBase::findKey(int p_key)
	{
		for (unsigned i = 0; i < m_keys.size(); ++i)
		{
			if (m_keys[i] == p_key)
				return i;
		}

		return -1;
	}

	// null methods:

	void EditorBase::onDraw(CL_GraphicContext &p_gc) { }

	void EditorBase::onLoad(CL_GraphicContext &p_gc) { }

	void EditorBase::onMouseMoved() { }

	void EditorBase::onMouseScrolled(bool p_up) { }

	void EditorBase::onUpdate(unsigned int p_timeElapsed) { }

	void EditorBase::onHandleInput() { }
}