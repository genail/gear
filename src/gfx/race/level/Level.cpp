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

#include "Level.h"

#include <vector>

#include "gfx/Viewport.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackTriangulator.h"
#include "logic/race/level/TrackSegment.h"

namespace Gfx
{

class LevelImpl
{
	public:

		const Race::Level m_levelLogic;

		const Viewport &m_viewport;

		const Race::TrackTriangulator &m_triangulator;

		LevelImpl(const Race::Level &p_levelLogic, const Viewport &p_viewport) :
				m_levelLogic(p_levelLogic),
				m_viewport(p_viewport),
				m_triangulator(p_levelLogic.getTrackTriangulator())
		{
			// empty
		}


		void drawTriangles(CL_GraphicContext &p_gc);

		void drawTriangle(
				CL_GraphicContext &p_gc,
				const CL_Pointf& p_a, const CL_Pointf& p_b, const CL_Pointf& p_c
		);

};

Level::Level(const Race::Level &p_levelLogic, const Viewport &p_viewport) :
		m_impl(new LevelImpl(p_levelLogic, p_viewport))
{
	// empty
}

Level::~Level()
{
	// empty
}

void Level::draw(CL_GraphicContext &p_gc)
{
	m_impl->drawTriangles(p_gc);
}

void LevelImpl::drawTriangles(CL_GraphicContext &p_gc)
{
	const Race::Track &track = m_levelLogic.getTrack();
	const int trackPointCount = track.getPointCount();

	const CL_Rectf &viewportBounds = m_viewport.getWorldClipRect();

	for (int i = 0; i < trackPointCount; ++i) {
		const Race::TrackSegment &seg = m_triangulator.getSegment(i);

		// run only if this segment is visible
		if (viewportBounds.is_overlapped(seg.getBounds())) {
			const std::vector<CL_Pointf> &points = seg.getTrianglePoints();

			const int triPointCount = static_cast<signed>(points.size());
			G_ASSERT(triPointCount % 3 == 0);

			for (int j = 0; j < triPointCount;) {
				const CL_Pointf &p1 = points[j++];
				const CL_Pointf &p2 = points[j++];
				const CL_Pointf &p3 = points[j++];

				drawTriangle(p_gc, p1, p2, p3);
			}

		}

		// fill the gaps between two segments
		// FIXME: check when to not draw this
		const CL_Pointf &prevLeft = m_triangulator.getLastLeftPoint(i);
		const CL_Pointf &prevRight = m_triangulator.getLastRightPoint(i);

		const int nextIdx = i + 1 < trackPointCount ? i + 1 : 0;
		const CL_Pointf &nextLeft = m_triangulator.getFirstLeftPoint(nextIdx);
		const CL_Pointf &nextRight = m_triangulator.getFirstRightPoint(nextIdx);

		drawTriangle(p_gc, prevLeft, prevRight, nextRight);
		drawTriangle(p_gc, prevLeft, nextRight, nextLeft);
	}
}

void LevelImpl::drawTriangle(
		CL_GraphicContext &p_gc,
		const CL_Pointf& p_a, const CL_Pointf& p_b, const CL_Pointf& p_c
)
{
	CL_Draw::triangle(p_gc, p_a, p_b, p_c, CL_Colorf::gray);

#if !defined(NDEBUG) && defined(DRAW_WIREFRAME)
	CL_Draw::line(p_gc, p_a, p_b, CL_Colorf::purple);
	CL_Draw::line(p_gc, p_b, p_c, CL_Colorf::purple);
	CL_Draw::line(p_gc, p_a, p_c, CL_Colorf::purple);
#endif
}

void Level::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);
}

Race::TrackTriangulator &Level::getTrackTriangulator()
{
	return const_cast<Race::TrackTriangulator&>(m_impl->m_triangulator);
}

} // namespace
