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

#include "EditorManagement.h"

#include "EditorTrack.h"
#include "EditorPoint.h"

#include "gfx/Viewport.h"
#include "gfx/race/level/Level.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"

using namespace Gfx;
using namespace Race;

namespace Editor
{
	class EditorManagementImpl
	{
	public:
		EditorManagementImpl() : 
			m_track(),
			m_viewport(),
			m_raceLevel(),
			m_gfxLevel(m_raceLevel, m_viewport),
			m_editorTrack(m_raceLevel, m_gfxLevel, m_track, m_viewport),
			m_editorPoint(m_track, m_gfxLevel),
			m_lastMousePos(0.0f, 0.0f)
		{
			m_raceLevel.setTrack(m_track);
		}

		~EditorManagementImpl()
		{
			m_raceLevel.destroy();
		}

		// editor variables

		EditorPoint m_editorPoint;

		EditorTrack m_editorTrack;

		// track

		Viewport m_viewport;

		Gfx::Level m_gfxLevel;

		Race::Level m_raceLevel;

		Track m_track;

		// help variables

		CL_Pointf m_lastMousePos;

	};

	EditorManagement::EditorManagement() : 
		m_impl(new EditorManagementImpl())
	{

	}

	EditorManagement::~EditorManagement()
	{
		
	}

	void EditorManagement::draw(CL_GraphicContext &p_gc)
	{
		m_impl->m_viewport.prepareGC(p_gc);

		m_impl->m_editorTrack.draw(p_gc);
		m_impl->m_editorPoint.draw(p_gc);

		m_impl->m_viewport.finalizeGC(p_gc);
	}

	void EditorManagement::load(CL_GraphicContext &p_gc)
	{
		m_impl->m_editorPoint.load(p_gc);
		m_impl->m_editorTrack.load(p_gc);
	}

	void EditorManagement::mouseMoved(const CL_Point &p_pos)
	{
		CL_Pointf mousePos = m_impl->m_viewport.toWorld((CL_Pointf)p_pos);
		CL_Pointf deltaPos = mousePos - m_impl->m_lastMousePos;

		m_impl->m_editorPoint.mouseMoved(mousePos, m_impl->m_lastMousePos, deltaPos);
		m_impl->m_editorTrack.mouseMoved(mousePos, m_impl->m_lastMousePos, deltaPos);
	}

	void EditorManagement::update(unsigned int p_timeElapsed)
	{
		m_impl->m_editorPoint.update(p_timeElapsed);
		m_impl->m_editorTrack.update(p_timeElapsed);
	}

	void EditorManagement::handleInput(InputState p_state, const CL_InputEvent& p_event)
	{
		bool pressed;

		switch (p_state) 
		{
			case Pressed:
				pressed = true;
				break;
			case Released:
				pressed = false;
				break;
			default:
				assert(0 && "unknown input state");
		}

		bool ok = true;

		if (ok)
			ok = m_impl->m_editorPoint.handleInput(pressed, p_event);

		if (ok)
			ok = m_impl->m_editorTrack.handleInput(pressed, p_event);
	}
}
