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
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "EditorHelper.h"
#include "EditorBase.h"

#include "common.h"
#include "gfx/Viewport.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackPoint.h"
#include "logic/race/level/TrackTriangulator.h"
#include "gfx/race/level/Level.h"

namespace Editor
{ 
	class EditorPointImpl;

	class EditorPoint : public EditorBase
	{
	public:
		EditorPoint(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Race::Track& p_track, Gfx::Viewport& p_viewport);

		~EditorPoint();

		virtual void onDraw(CL_GraphicContext &p_gc);

		virtual void onMouseScrolled(bool p_up);

		virtual void onMouseMoved();

		virtual void onUpdate(unsigned int p_timeElapsed);

		virtual void onHandleInput();

	private:
		void setDefaultPoints();

		void drawPoints(CL_GraphicContext &p_gc);

		void drawPoint(int p_index, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc);

		void findPointAt(const CL_Pointf &p_pos, int &p_index);

		void getShiftRect(int p_index, int* x1, int* y1, int* x2, int* y2);

		CL_Rect getPointRect(int p_index);

		CL_Rect getPointRect(const CL_Pointf &p_point);

		void setToPerpendicular(CL_Vec2f &p_vector2, bool p_isInvert);

		void setMinAndMaxShiftPoint(int p_index);

		void setRadius(int p_index, int p_set);

		void triangulate(int p_index);

		CL_SharedPtr<EditorPointImpl> m_impl;
	};
}
