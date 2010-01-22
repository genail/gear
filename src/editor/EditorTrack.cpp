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

#include "EditorTrack.h"

using namespace Race;
using namespace Gfx;

const int NET_LINE_MARGIN = 100;

namespace Editor
{
	class EditorTrackImpl
	{
	public:
		EditorTrackImpl(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Track& p_track, Viewport& p_viewport) : 
			m_raceLevel(p_raceLevel),
			m_track(p_track),
			m_viewport(p_viewport),
			m_gfxLevel(p_gfxLevel),
			m_pressedId(CL_NONE_PRESSED),
			m_lookPoint(400.0f, 300.0f),
			m_netLineColor(CL_Colorf::white)
		{
			m_viewport.attachTo(&m_lookPoint);
		}

		~EditorTrackImpl()
		{

		}

		// help variables

		CL_Pointf m_lookPoint;

		CL_Colorf m_netLineColor;

		// input

		int m_pressedId;

		// references

		Race::Level& m_raceLevel;
		
		Gfx::Level& m_gfxLevel;

		Track& m_track;

		Viewport& m_viewport;

		// methods

		void handleInput(bool p_pressed, const CL_InputEvent &p_event);

		void mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos);
	
		void mouseScrolled(bool p_up);

		void drawTrack(CL_GraphicContext &p_gc);

		void drawNet(CL_GraphicContext &p_gc);

		void load(CL_GraphicContext &p_gc);

		void update(unsigned int p_timeElapsed);

		void setDefaultView();

		bool getHandle() const;
	};

	void EditorTrackImpl::drawTrack(CL_GraphicContext &p_gc)
	{
		m_gfxLevel.draw(p_gc);
	}

	void EditorTrackImpl::drawNet(CL_GraphicContext &p_gc)
	{
		const CL_Rect& worldRect = m_viewport.getWorldClipRect();

		int x = worldRect.left - (worldRect.left % NET_LINE_MARGIN);
		int y = worldRect.top - (worldRect.top % NET_LINE_MARGIN);

		int absolutlyWidth = (worldRect.right - worldRect.left) + x + NET_LINE_MARGIN;
		int absolutlyHeight = (worldRect.bottom - worldRect.top) + y + NET_LINE_MARGIN;

		for ( ; x <= absolutlyWidth; x += NET_LINE_MARGIN)
		{
			CL_Draw::line(p_gc, x, worldRect.top, x, absolutlyHeight, m_netLineColor);
		}
		for ( ; y <= absolutlyHeight; y += NET_LINE_MARGIN)
		{
			CL_Draw::line(p_gc, worldRect.left, y, absolutlyWidth, y, m_netLineColor);
		}
	}

	void EditorTrackImpl::load(CL_GraphicContext &p_gc)
	{

	}

	void EditorTrackImpl::update(unsigned int p_timeElapsed)
	{

	}

	void EditorTrackImpl::handleInput(bool p_pressed, const CL_InputEvent &p_event)
	{
		if (p_pressed && m_pressedId == CL_NONE_PRESSED)
		{
			m_pressedId = p_event.id;
		}
		else if (!p_pressed && p_event.id == m_pressedId)
		{
			m_pressedId = CL_NONE_PRESSED;
		}

		if (m_pressedId == CL_KEY_SPACE)
		{
			setDefaultView();
		}
	}

	void EditorTrackImpl::mouseMoved(const CL_Pointf &p_inconditionalMousePos, const CL_Pointf &p_inconditionalLastMousePos, const CL_Pointf &p_inconditionalDeltaPos)
	{
		if (m_pressedId == CL_MOUSE_RIGHT)
			m_lookPoint -= p_inconditionalDeltaPos;
	}

	void EditorTrackImpl::mouseScrolled(bool p_up)
	{
		if (m_pressedId == CL_NONE_PRESSED)
		{
			float set = p_up ? 0.1f : -0.1f;

			float newScale = m_viewport.getScale() + set;

			if (newScale > 0.0f && newScale <= 2.0f)
				m_viewport.setScale(newScale);
		}
	}

	void EditorTrackImpl::setDefaultView()
	{
		m_viewport.setScale(1.0f);
		m_lookPoint = m_track.getPoint(0).getPosition();
	}

	bool EditorTrackImpl::getHandle() const
	{
		return (!m_pressedId == CL_NONE_PRESSED);
	}

	EditorTrack::EditorTrack(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Track& p_track, Viewport& p_viewport) : 
		m_impl(new EditorTrackImpl(p_raceLevel, p_gfxLevel, p_track, p_viewport))
	{

	}

	EditorTrack::~EditorTrack()
	{

	}

	void EditorTrack::drawTrack(CL_GraphicContext &p_gc)
	{
		m_impl->drawTrack(p_gc);
	}

	void EditorTrack::drawNet(CL_GraphicContext &p_gc)
	{
		m_impl->drawNet(p_gc);
	}

	void EditorTrack::load(CL_GraphicContext &p_gc)
	{
		m_impl->load(p_gc);
	}

	void EditorTrack::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos)
	{
		m_impl->mouseMoved(p_mousePos, p_lastMousePos, p_deltaPos);
	}

	void EditorTrack::mouseScrolled(bool p_up)
	{
		m_impl->mouseScrolled(p_up);
	}

	void EditorTrack::update(unsigned int p_timeElapsed)
	{
		m_impl->update(p_timeElapsed);
	}

	void EditorTrack::handleInput(bool p_pressed, const CL_InputEvent& p_event)
	{
		m_impl->handleInput(p_pressed, p_event);
	}

	bool EditorTrack::getHandle() const
	{
		return m_impl->getHandle();
	}
}
