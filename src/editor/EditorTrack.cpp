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

namespace Editor
{
	class EditorTrackImpl
	{
	public:
		EditorTrackImpl(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Track& p_track, Viewport& p_viewport) : 
			m_raceLevel(p_raceLevel),
			m_track(p_track),
			m_viewport(p_viewport),
			m_gfxLevel(p_gfxLevel)
		{
			m_raceLevel.setTrack(m_track);

			CL_Pointf m_lookPoint(Gfx::Stage::getWidth() / 2, Gfx::Stage::getHeight() / 2);

			m_viewport.attachTo(&m_lookPoint);
		}

		~EditorTrackImpl()
		{
			m_raceLevel.destroy();
		}

		// help variables

		CL_Pointf m_lookPoint;

		// references

		Race::Level& m_raceLevel;
		
		Gfx::Level& m_gfxLevel;

		Track& m_track;

		Viewport& m_viewport;

		// methods

		bool handleInput(bool p_pressed, const CL_InputEvent &p_event);

		void mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos);

		void draw(CL_GraphicContext &p_gc);

		void load(CL_GraphicContext &p_gc);

		void update(unsigned int p_timeElapsed);
	};

	void EditorTrackImpl::draw(CL_GraphicContext &p_gc)
	{
		m_gfxLevel.draw(p_gc);
	}

	void EditorTrackImpl::load(CL_GraphicContext &p_gc)
	{
		m_gfxLevel.load(p_gc);
	}

	void EditorTrackImpl::update(unsigned int p_timeElapsed)
	{

	}

	bool EditorTrackImpl::handleInput(bool p_pressed, const CL_InputEvent &p_event)
	{
		return true;
	}

	void EditorTrackImpl::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos)
	{
		
	}

	EditorTrack::EditorTrack(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Track& p_track, Viewport& p_viewport) : 
		m_impl(new EditorTrackImpl(p_raceLevel, p_gfxLevel, p_track, p_viewport))
	{

	}

	EditorTrack::~EditorTrack()
	{

	}

	void EditorTrack::draw(CL_GraphicContext &p_gc)
	{
		m_impl->draw(p_gc);
	}

	void EditorTrack::load(CL_GraphicContext &p_gc)
	{
		m_impl->load(p_gc);
	}

	void EditorTrack::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos)
	{
		m_impl->mouseMoved(p_mousePos, p_lastMousePos, p_deltaPos);
	}

	void EditorTrack::update(unsigned int p_timeElapsed)
	{
		m_impl->update(p_timeElapsed);
	}

	bool EditorTrack::handleInput(bool p_pressed, const CL_InputEvent& p_event)
	{
		return m_impl->handleInput(p_pressed, p_event);
	}
}
