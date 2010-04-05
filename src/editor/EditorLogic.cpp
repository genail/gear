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

#include "EditorLogic.h"

using namespace Race;

namespace Editor
{
	class EditorLogicImpl
	{
	public:
		EditorLogicImpl(Race::Level& p_raceLevel, Gfx::Level* p_gfxLevel, Race::Track& p_track, Gfx::Viewport& p_viewport, RaceScene& p_raceScene) :
			m_raceLevel(p_raceLevel),
			m_gfxLevel(p_gfxLevel),
			m_track(p_track),
			m_viewport(p_viewport),
			m_raceScene(p_raceScene)
		{

		}

		~EditorLogicImpl()
		{

		}

		// editor variables

		Race::Level& m_raceLevel;
		
		Gfx::Level* m_gfxLevel;

		Race::Track& m_track;

		Gfx::Viewport& m_viewport;

		RaceScene& m_raceScene;
	};

	EditorLogic::EditorLogic(Race::Level& p_raceLevel, Gfx::Level* p_gfxLevel, Race::Track& p_track, Gfx::Viewport& p_viewport, RaceScene& p_raceScene) : 
		m_impl(new EditorLogicImpl(p_raceLevel, p_gfxLevel, p_track, p_viewport, p_raceScene))
	{

	}

	EditorLogic::~EditorLogic()
	{

	}

	void EditorLogic::save(const CL_String& p_fileName)
	{
		m_impl->m_raceLevel.save(p_fileName);
	}

	bool EditorLogic::load(const CL_String& p_fileName)
	{
		bool loaded = m_impl->m_raceLevel.load(p_fileName);

		if (loaded)
		{
			m_impl->m_track = m_impl->m_raceLevel.getTrack();

			m_impl->m_gfxLevel = new Gfx::Level(&m_impl->m_raceLevel, &m_impl->m_viewport);
		}

		return loaded;
	}

	void EditorLogic::startTest()
	{
		m_impl->m_raceScene.initializeOffline(&m_impl->m_raceLevel);

		Gfx::Stage::pushScene(&m_impl->m_raceScene);
	}
}
