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
		EditorTrackImpl() : 
			m_lookPoint(400.0f, 300.0f),
			m_netLineColor(CL_Colorf::white)
		{

		}

		~EditorTrackImpl()
		{

		}

		// help variables

		CL_Pointf m_lookPoint;

		CL_Colorf m_netLineColor;
	};

	EditorTrack::EditorTrack(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Track& p_track, Viewport& p_viewport) : 
		m_impl(new EditorTrackImpl()),
		EditorBase(p_raceLevel, p_gfxLevel, p_track, p_viewport)
	{
		m_viewport.attachTo(&m_impl->m_lookPoint);
	}

	EditorTrack::~EditorTrack()
	{

	}

	void EditorTrack::drawTrack(CL_GraphicContext &p_gc)
	{
		m_gfxLevel.draw(p_gc);
	}

	void EditorTrack::drawNet(CL_GraphicContext &p_gc)
	{
		const CL_Rect& worldRect = m_viewport.getWorldClipRect();

		int x = worldRect.left - (worldRect.left % NET_LINE_MARGIN);
		int y = worldRect.top - (worldRect.top % NET_LINE_MARGIN);

		int absolutlyWidth = (worldRect.right - worldRect.left) + x + NET_LINE_MARGIN;
		int absolutlyHeight = (worldRect.bottom - worldRect.top) + y + NET_LINE_MARGIN;

		for ( ; x <= absolutlyWidth; x += NET_LINE_MARGIN)
		{
			CL_Draw::line(p_gc, x, worldRect.top, x, absolutlyHeight, m_impl->m_netLineColor);
		}
		for ( ; y <= absolutlyHeight; y += NET_LINE_MARGIN)
		{
			CL_Draw::line(p_gc, worldRect.left, y, absolutlyWidth, y, m_impl->m_netLineColor);
		}
	}

	void EditorTrack::setDefaultView()
	{
		m_viewport.setScale(1.0f);
		m_impl->m_lookPoint = m_track.getPoint(0).getPosition();
	}

	void EditorTrack::onHandleInput()
	{
		if (hasContainsKey(CL_KEY_SPACE))
		{
			setDefaultView();
		}
	}

	void EditorTrack::onMouseMoved()
	{
		if (hasContainsKey(CL_MOUSE_RIGHT))
		{
			CL_Pointf delta = m_inconditionalDeltaMousePos / m_viewport.getScale();
			m_impl->m_lookPoint -= delta;
		}
	}

	void EditorTrack::onDraw(CL_GraphicContext &p_gc)
	{
		drawNet(p_gc);
		drawTrack(p_gc);
	}

	void EditorTrack::onMouseScrolled(bool p_up)
	{
		if (getKeys().size() == 0)
		{
			float set = p_up ? 0.1f : -0.1f;

			float newScale = m_viewport.getScale() + set;

			if (newScale > 0.0f && newScale <= 2.0f)
				m_viewport.setScale(newScale);
		}
	}
}
