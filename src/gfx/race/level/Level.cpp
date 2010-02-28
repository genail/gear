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

#include "Level.h"

#include <vector>

#include "common/Units.h"
#include "gfx/Stage.h"
#include "gfx/Viewport.h"
#include "logic/race/level/Object.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackPoint.h"
#include "logic/race/level/TrackTriangulator.h"
#include "logic/race/level/TrackSegment.h"
#include "math/Integer.h"

namespace Gfx
{

const CL_Vec4f WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const int HELPER_SIZE = 4;

class LevelImpl
{
	public:

		const Race::Level m_levelLogic;

		const Viewport &m_viewport;

		const Race::TrackTriangulator &m_triangulator;


		/** street texture */
		CL_Texture m_streetTexture;


		/** trackpoints distances from start [seg_id][point_id] */
		std::vector<std::vector<float> > m_distances;

		/** Distance from last point to start point */
		float m_distanceToStart;


		// helpers to optimize drawing process
		CL_Vec2f m_helperVerts[HELPER_SIZE];
		CL_Vec4f m_helperColors[HELPER_SIZE];
		CL_Vec2f m_helperTexCoords[HELPER_SIZE];

		CL_PrimitivesArray *m_helperArr;


		LevelImpl(const Race::Level &p_levelLogic, const Viewport &p_viewport) :
				m_levelLogic(p_levelLogic),
				m_viewport(p_viewport),
				m_triangulator(p_levelLogic.getTrackTriangulator()),
				m_helperArr(NULL)
		{
			for (int i = 0; i < HELPER_SIZE; ++i) {
				m_helperColors[i] = WHITE;
			}
		}

		~LevelImpl() {
			if (m_helperArr) {
				delete m_helperArr;
			}
		}


		void drawQuads(CL_GraphicContext &p_gc);

		void drawQuad(
				CL_GraphicContext &p_gc,
				const CL_Pointf &p_a,
				const CL_Pointf &p_b,
				const CL_Pointf &p_c,
				const CL_Pointf &p_d,
				const CL_Vec2f &p_tca,
				const CL_Vec2f &p_tcb,
				const CL_Vec2f &p_tcc,
				const CL_Vec2f &p_tcd
		);

		void drawStartLine(CL_GraphicContext &p_gc);

		void drawObjects(CL_GraphicContext &p_gc);


		// loaders

		void loadTrackTexture(CL_GraphicContext &p_gc);

};

Level::Level(const Race::Level &p_levelLogic, const Viewport &p_viewport) :
	m_impl(new LevelImpl(p_levelLogic, p_viewport))
{
	// empty
}

Level::Level(const Race::Level *p_levelLogic, const Viewport *p_viewport) :
	m_impl(new LevelImpl(*p_levelLogic, *p_viewport))
{
	// empty
}

Level::~Level()
{
	// empty
}

void Level::draw(CL_GraphicContext &p_gc)
{
	m_impl->drawQuads(p_gc);
//	m_impl->drawTriangles(p_gc);
	m_impl->drawStartLine(p_gc);
	m_impl->drawObjects(p_gc);
}

void LevelImpl::drawQuads(CL_GraphicContext &p_gc)
{
	// length of street in meters
	const float STREET_TILE_LENGTH_M = 30.0f;

	const Race::Track &track = m_levelLogic.getTrack();
	const int trackPointCount = track.getPointCount();

	const CL_Rectf &viewportBounds = m_viewport.getWorldClipRect();

	for (int i = 0; i < trackPointCount; ++i) {
		const Race::TrackSegment &seg = m_triangulator.getSegment(i);

		// run only if this segment is visible
		if (viewportBounds.is_overlapped(seg.getBounds())) {

			const std::vector<Race::TrackSegment::PointPair> &pairs =
					seg.getPointPairs();

			const int pairsCount = static_cast<signed>(pairs.size());
			CL_Pointf prevLeft, prevRight;

			for (int pairIdx = 0; pairIdx < pairsCount; ++pairIdx) {

				const CL_Pointf &currLeft = pairs[pairIdx].m_left;
				const CL_Pointf &currRight = pairs[pairIdx].m_right;

				if (pairIdx != 0) {
					const float bDist = m_distances[i][pairIdx - 1];
					const float fDist = m_distances[i][pairIdx];

					const float bCoord =
							fmod(bDist, STREET_TILE_LENGTH_M)
							/ STREET_TILE_LENGTH_M;

					const float fCoord =
							bCoord + (fDist - bDist)/ STREET_TILE_LENGTH_M;


					drawQuad(
							p_gc,
							prevLeft,
							prevRight,
							currRight,
							currLeft,
							CL_Vec2f(0.0f, bCoord),
							CL_Vec2f(1.0f, bCoord),
							CL_Vec2f(1.0f, fCoord),
							CL_Vec2f(0.0f, fCoord)
					);
				}

				prevLeft = currLeft;
				prevRight = currRight;
			}

		}

		// fill the gaps between two segments
		// FIXME: check when to not draw this
		const CL_Pointf &prevLeft = m_triangulator.getLastLeftPoint(i);
		const CL_Pointf &prevRight = m_triangulator.getLastRightPoint(i);

		const int nextIdx = i + 1 < trackPointCount ? i + 1 : 0;
		const CL_Pointf &nextLeft = m_triangulator.getFirstLeftPoint(nextIdx);
		const CL_Pointf &nextRight = m_triangulator.getFirstRightPoint(nextIdx);

		const float bDist = m_distances[i][m_distances[i].size() - 2];
		float fDist = m_distances[nextIdx][0];

		if (fDist < bDist) {
			// this is end point to start point distance
			fDist = m_distanceToStart;
		}


		// FIXME: this is a copy of above code!
		const float bCoord =
						fmod(bDist, STREET_TILE_LENGTH_M)
						/ STREET_TILE_LENGTH_M;

		const float fCoord =
				bCoord + (fDist - bDist)/ STREET_TILE_LENGTH_M;

		drawQuad(
				p_gc,
				prevLeft,
				prevRight,
				nextRight,
				nextLeft,
				CL_Vec2f(0.0f, bCoord),
				CL_Vec2f(1.0f, bCoord),
				CL_Vec2f(1.0f, fCoord),
				CL_Vec2f(0.0f, fCoord)
		);

	}
}

void LevelImpl::drawQuad(
		CL_GraphicContext &p_gc,
		const CL_Pointf &p_a,
		const CL_Pointf &p_b,
		const CL_Pointf &p_c,
		const CL_Pointf &p_d,
		const CL_Vec2f &p_tca,
		const CL_Vec2f &p_tcb,
		const CL_Vec2f &p_tcc,
		const CL_Vec2f &p_tcd
)
{
	// verticles
	m_helperVerts[0] = p_a;
	m_helperVerts[1] = p_b;
	m_helperVerts[2] = p_c;
	m_helperVerts[3] = p_d;

	// texture coords
	m_helperTexCoords[0] = p_tca;
	m_helperTexCoords[1] = p_tcb;
	m_helperTexCoords[2] = p_tcc;
	m_helperTexCoords[3] = p_tcd;

	p_gc.set_program_object(cl_program_single_texture);
	p_gc.set_texture(0, m_streetTexture);

	// draw
	p_gc.draw_primitives(cl_quads, 4, *m_helperArr);

	// preserve settings
	p_gc.set_program_object(cl_program_color_only);

#if !defined(NDEBUG) && defined(DRAW_WIREFRAME)
	CL_Draw::line(p_gc, p_a, p_b, CL_Colorf::purple);
	CL_Draw::line(p_gc, p_b, p_c, CL_Colorf::purple);
	CL_Draw::line(p_gc, p_c, p_d, CL_Colorf::purple);
	CL_Draw::line(p_gc, p_d, p_a, CL_Colorf::purple);
#endif
}

void LevelImpl::drawStartLine(CL_GraphicContext &p_gc)
{
	const Race::Track &track = m_levelLogic.getTrack();
	const int pointCount = track.getPointCount();

	G_ASSERT(pointCount >= 1);

	const CL_Pointf &a = m_triangulator.getFirstLeftPoint(0);
	const CL_Pointf &b = m_triangulator.getFirstRightPoint(0);

	// draw only if visible on screen
	const CL_Rectf &clip = m_viewport.getWorldClipRect();

	if (clip.contains(a) || clip.contains(b)) {
		CL_Pen oldPen = p_gc.get_pen();

		CL_Pen pen;
		pen.set_line_width(10);
		p_gc.set_pen(pen);

		CL_Draw::line(p_gc, a, b, CL_Colorf::white);

		p_gc.set_pen(oldPen);
	}
}

void LevelImpl::drawObjects(CL_GraphicContext &p_gc)
{
	CL_Pen oldPen = p_gc.get_pen();

	CL_Pen pen;
	pen.set_line_width(3);
	p_gc.set_pen(pen);
	const int objCount = m_levelLogic.getObjectCount();

	for (int i = 0; i < objCount; ++i) {
		const Race::Object &obj = m_levelLogic.getObject(i);
		const int ptCount = obj.getPointCount();

		if (ptCount > 1) {
			for (int j = 0; j < ptCount; ++j) {
				const CL_Pointf &prev = obj.getPoint(
						Math::Integer::clamp(j - 1, 0, ptCount - 1)
				);
				const CL_Pointf &curr = obj.getPoint(j);

				CL_Draw::line(p_gc, prev, curr, CL_Colorf::white);
			}
		}
	}

	p_gc.set_pen(oldPen);
}

void Level::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);
	m_impl->loadTrackTexture(p_gc);

	// prepare helper array
	m_impl->m_helperArr = new CL_PrimitivesArray(p_gc);

	m_impl->m_helperArr->set_attributes(0, m_impl->m_helperVerts);
	m_impl->m_helperArr->set_attributes(1, m_impl->m_helperColors);
	m_impl->m_helperArr->set_attributes(2, m_impl->m_helperTexCoords);
}

void LevelImpl::loadTrackTexture(CL_GraphicContext &p_gc)
{
	// load texture
	m_streetTexture = CL_Texture(
			"race/street", Gfx::Stage::getResourceManager(), p_gc
	);

	m_streetTexture.set_wrap_mode(cl_wrap_repeat, cl_wrap_repeat);
	m_streetTexture.set_generate_mipmap(true);
	m_streetTexture.set_mag_filter(cl_filter_linear);
	m_streetTexture.set_min_filter(cl_filter_linear);

	// make sure that level is usable
	G_ASSERT(m_levelLogic.isUsable());

	const Race::Track &track = m_levelLogic.getTrack();
	const int count = track.getPointCount();

	// collect all checkpoint distances in m_distances vector
	// like this:
	// m_distances[segment_id][point_id]

	m_distances.reserve(count);

	G_ASSERT(count >= 1);

	CL_Pointf prevPointL, prevPointR;
	float distL = 0.0f, distR = 0.0f;
	bool hasFirst = false;

	for (int i = 0; i < count; ++i) {
		m_distances.push_back(std::vector<float>());

		const Race::TrackSegment &seg = m_triangulator.getSegment(i);
		const std::vector<Race::TrackSegment::PointPair> &pairs =
				seg.getPointPairs();

		foreach (const Race::TrackSegment::PointPair &pair, pairs) {
			if (hasFirst) {
				distL += Units::toWorld(prevPointL.distance(pair.m_left));
				distR += Units::toWorld(prevPointR.distance(pair.m_right));
			} else {
				hasFirst = true;
			}

			m_distances[i].push_back((distL + distR) / 2);

			prevPointL = pair.m_left;
			prevPointR = pair.m_right;
		}
	}

	// calculate distance from last point to start
	const CL_Pointf &startPoint =
			m_triangulator.getSegment(0).getMidPoints()[0];

	distL += Units::toWorld(prevPointL.distance(startPoint));
	distR += Units::toWorld(prevPointR.distance(startPoint));

	m_distanceToStart = distL;

}

Race::TrackTriangulator &Level::getTrackTriangulator()
{
	return const_cast<Race::TrackTriangulator&>(m_impl->m_triangulator);
}

} // namespace
