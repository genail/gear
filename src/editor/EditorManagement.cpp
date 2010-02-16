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

#include "EditorManagement.h"

#include "EditorTrack.h"
#include "EditorPoint.h"
#include "EditorLogic.h"
#include "EditorBase.h"

#include "EditorMenu.h"

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
		EditorManagementImpl(Gfx::DirectScene& p_directScene) : 
			m_track(),
			m_viewport(),
			m_raceLevel(),
			m_gfxLevel(m_raceLevel, m_viewport),
			m_editorTrack(m_raceLevel, m_gfxLevel, m_track, m_viewport),
			m_editorLogic(m_raceLevel, m_gfxLevel, m_track, m_viewport),
			m_editorMenu(p_directScene, m_editorLogic),
			m_editorPoint(m_raceLevel, m_gfxLevel, m_track, m_viewport),
			m_editors()
		{
			m_raceLevel.setTrack(m_track);

			m_editors.push_back(&m_editorTrack);
			m_editors.push_back(&m_editorPoint);
		}

		~EditorManagementImpl()
		{

		}

		// track

		Track m_track;

		Viewport m_viewport;

		Race::Level m_raceLevel;

		Gfx::Level m_gfxLevel;

		// editor variables

		EditorPoint m_editorPoint;

		EditorTrack m_editorTrack;

		EditorLogic m_editorLogic;

		std::vector<EditorBase*> m_editors;

		// menu

		Gfx::EditorMenu m_editorMenu;

	};

	EditorManagement::EditorManagement(Gfx::DirectScene& p_directScene) : 
		m_impl(new EditorManagementImpl(p_directScene))
	{

	}

	EditorManagement::~EditorManagement()
	{
		m_impl->m_raceLevel.destroy();
	}

	void EditorManagement::draw(CL_GraphicContext &p_gc)
	{
		CL_Draw::fill(p_gc, 0.0f, 0.0f, Stage::getWidth(), Stage::getHeight(), CL_Colorf::green);

		m_impl->m_viewport.prepareGC(p_gc);

		// bercik, 15 luty:
		// ma³o wydajne rozwi¹zanie, które oblicza przed ka¿dym odrysowaniem ca³¹ trasê.
		// nale¿y usnu¹æ w przysz³oœci po usuniêciu b³êdu z niewyliczeniem ca³ej trasy na pocz¹tku
		m_impl->m_gfxLevel.getTrackTriangulator().triangulate(m_impl->m_track); // !!!

		for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
			m_impl->m_editors[i]->draw(p_gc);

		m_impl->m_viewport.finalizeGC(p_gc);
	}

	void EditorManagement::load(CL_GraphicContext &p_gc)
	{
		m_impl->m_gfxLevel.load(p_gc);

		for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
			m_impl->m_editors[i]->load(p_gc);
	}

	void EditorManagement::mouseMoved(const CL_Point &p_pos)
	{
		CL_Pointf mousePos = m_impl->m_viewport.toWorld((CL_Pointf)p_pos);

		if (!m_impl->m_editorMenu.isVisible())
		{
			for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
				m_impl->m_editors[i]->mouseMoved(mousePos, (CL_Pointf)p_pos);
		}
	}

	void EditorManagement::update(unsigned int p_timeElapsed)
	{
		for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
			m_impl->m_editors[i]->update(p_timeElapsed);
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

		if (pressed && p_event.id == CL_KEY_ESCAPE)
		{
			if (m_impl->m_editorMenu.isVisible())
				m_impl->m_editorMenu.setVisible(false);
			else
				m_impl->m_editorMenu.setVisible(true);

			return;
		}

		if (!m_impl->m_editorMenu.isVisible())
		{
			for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
				m_impl->m_editors[i]->handleInput(p_state, p_event);
		}
	}

	void EditorManagement::mouseScrolled(bool p_up)
	{
		if (!m_impl->m_editorMenu.isVisible())
		{
			for (unsigned i = 0; i < m_impl->m_editors.size(); ++i)
				m_impl->m_editors[i]->mouseScrolled(p_up);
		}
	}
}
