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

const int CRACKS_COUNT = 5;

// length of street in meters
const float STREET_TILE_LENGTH_M = 30.0f;

class LevelImpl
{
	public:

		enum Direction {
			D_LEFT,
			D_RIGHT
		};

		struct Crack {
			CL_Pointf m_position;

			/** m_cracks index */
			int m_crackId;

			Crack(const CL_Pointf &p_pos, int p_id) :
				m_position(p_pos),
				m_crackId(p_id)
			{ /* empty */ }
		};


		const Race::Level m_levelLogic;

		const Viewport &m_viewport;

		const Race::TrackTriangulator &m_triangulator;


		/** Street texture */
		CL_Texture m_streetTexture;

		/** Sand texture */
		CL_Texture m_sandTexture;


		// crack sprites in street
		CL_Sprite m_crackSprites[CRACKS_COUNT];

		// cracks collection to draw
		std::vector<Crack> m_crackPositions;


		/** center track point distances from start [seg_id][point_id] */
		std::vector<std::vector<float> > m_distances;

		/** Left track side distances */
		std::vector<std::vector<float> > m_ldistances;

		/** Right track side distances */
		std::vector<std::vector<float> > m_rdistances;

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


		void drawTrack(CL_GraphicContext &p_gc);

		void drawQuads(
				CL_GraphicContext &p_gc,
				int p_segmentId
		);

		void drawSand(
				CL_GraphicContext &p_gc,
				int p_segmentId,
				const std::vector<Race::TrackSegment::PointPair> &p_pairs,
				Direction p_direction
		);

		void drawQuad(
				CL_GraphicContext &p_gc,
				const CL_Texture &p_texture,
				const CL_Pointf &p_a,
				const CL_Pointf &p_b,
				const CL_Pointf &p_c,
				const CL_Pointf &p_d,
				const CL_Vec2f &p_tca,
				const CL_Vec2f &p_tcb,
				const CL_Vec2f &p_tcc,
				const CL_Vec2f &p_tcd
		);

		/** Draw street cracks */
		void drawCracks(CL_GraphicContext &p_gc);

		void drawStartLine(CL_GraphicContext &p_gc);

		void drawObjects(CL_GraphicContext &p_gc);


		// loaders

		void loadTrackTexture(CL_GraphicContext &p_gc);

		void loadSandTexture(CL_GraphicContext &p_gc);

		void loadCracks(CL_GraphicContext &p_gc);


		void expand(CL_Rectf *p_rect, const CL_Pointf &p_point);

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
	m_impl->drawTrack(p_gc);
//	m_impl->drawTriangles(p_gc);
	m_impl->drawCracks(p_gc);
	m_impl->drawStartLine(p_gc);
	m_impl->drawObjects(p_gc);
}

void LevelImpl::drawTrack(CL_GraphicContext &p_gc)
{
	const Race::Track &track = m_levelLogic.getTrack();
	const int trackPointCount = track.getPointCount();

	const CL_Rectf &viewportBounds = m_viewport.getWorldClipRect();

	for (int i = 0; i < trackPointCount; ++i) {

		const int nextIdx = Math::Integer::clamp(i + 1, 0, trackPointCount - 1);

		const Race::TrackSegment &seg = m_triangulator.getSegment(i);

		CL_Rectf bounds = seg.getBounds();
		expand(&bounds, m_triangulator.getFirstLeftPoint(nextIdx));
		expand(&bounds, m_triangulator.getFirstRightPoint(nextIdx));

		// run only if this segment is visible
		if (viewportBounds.is_overlapped(bounds)) {
			drawSand(p_gc, i, seg.getPointPairs(), D_LEFT);
			drawSand(p_gc, i, seg.getPointPairs(), D_RIGHT);
			drawQuads(p_gc, i);
		}

	}
}

void LevelImpl::drawQuads(
		CL_GraphicContext &p_gc,
		int p_segmentId
)
{
	const std::vector<Race::TrackSegment::PointPair> &p_pairs =
			m_triangulator.getSegment(p_segmentId).getPointPairs();

	// next segment index
	const Race::Track &track = m_levelLogic.getTrack();
	const int trackPointCount = track.getPointCount();
	const int nextSegId = Math::Integer::clamp(
			p_segmentId + 1, 0, trackPointCount - 1
	);

	const int pairsCount = static_cast<signed>(p_pairs.size());
	CL_Pointf prevLeft, prevRight;
	CL_Pointf currLeft, currRight;
	float bDist, fDist;

	// draw each quad
	for (int pairIdx = 0; pairIdx <= pairsCount; ++pairIdx) {

		if (pairIdx < pairsCount) {
			currLeft = p_pairs[pairIdx].m_left;
			currRight = p_pairs[pairIdx].m_right;
		} else {
			// last quad is connecting this segment with next one
			currLeft = m_triangulator.getFirstLeftPoint(nextSegId);
		    currRight = m_triangulator.getFirstRightPoint(nextSegId);
		}

		if (pairIdx != 0) {

			if (pairIdx < pairsCount) {
				bDist = m_distances[p_segmentId][pairIdx - 1];
				fDist = m_distances[p_segmentId][pairIdx];
			} else {
				// connector
				bDist = m_distances[p_segmentId][pairIdx - 1];
				fDist = m_distances[nextSegId][0];
			}

			const float bCoord =
					fmod(bDist, STREET_TILE_LENGTH_M)
					/ STREET_TILE_LENGTH_M;

			const float fCoord =
					bCoord + (fDist - bDist)/ STREET_TILE_LENGTH_M;


			drawQuad(
					p_gc,
					m_streetTexture,
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

void LevelImpl::drawSand(
		CL_GraphicContext &p_gc,
		int p_segmentId,
		const std::vector<Race::TrackSegment::PointPair> &p_pairs,
		Direction p_direction
)
{
	const std::vector<std::vector<float> > &distances =
			p_direction == D_LEFT ? m_ldistances : m_rdistances;

	const int pairsCount = static_cast<signed>(p_pairs.size());

	// far coordinates (away from track)
	CL_Pointf nearPrev, nearCurr, farA, farB;

	// previous point on wanted side
	CL_Pointf prev;
	// previous point on oposite side
	CL_Pointf prevOposite;

	for (int pairIdx = 0; pairIdx < pairsCount; ++pairIdx) {

		const Race::TrackSegment::PointPair &pair = p_pairs[pairIdx];

		const CL_Pointf &curr =
				p_direction == D_LEFT ? pair.m_left : pair.m_right;
		const CL_Pointf &currOposite =
				p_direction == D_LEFT ? pair.m_right : pair.m_left;

		if (pairIdx != 0) {
			const float bDist = distances[p_segmentId][pairIdx - 1];
			const float fDist = distances[p_segmentId][pairIdx];

			const float bCoord =
					fmod(bDist, STREET_TILE_LENGTH_M)
					/ STREET_TILE_LENGTH_M;

			const float fCoord =
					bCoord + (fDist - bDist)/ STREET_TILE_LENGTH_M;

			// calculate far coords
			farA = prev + ((prev - prevOposite) * 0.25);
			farB = curr + ((curr - currOposite) * 0.25);

			nearPrev = prev;
			nearCurr = curr;

			nearPrev += (prevOposite - prev) * 0.04;
			nearCurr += (currOposite - curr) * 0.04;

			switch (p_direction) {
				case D_LEFT:
					drawQuad(
							p_gc,
							m_sandTexture,
							nearPrev, nearCurr,
							farB, farA,
							CL_Vec2f(0.0f, bCoord),
							CL_Vec2f(0.0f, fCoord),
							CL_Vec2f(1.0f, fCoord),
							CL_Vec2f(1.0f, bCoord)
					);

					break;

				case D_RIGHT:
					drawQuad(
							p_gc,
							m_sandTexture,
							nearCurr, nearPrev,
							farA, farB,
							CL_Vec2f(0.0f, fCoord),
							CL_Vec2f(0.0f, bCoord),
							CL_Vec2f(1.0f, bCoord),
							CL_Vec2f(1.0f, fCoord)
					);

					break;

				default:
					G_ASSERT(0);
			}
		}

		prev = curr;
		prevOposite = currOposite;
	}
}

void LevelImpl::drawQuad(
		CL_GraphicContext &p_gc,
		const CL_Texture &p_texture,
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
	p_gc.set_texture(0, p_texture);

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

void LevelImpl::drawCracks(CL_GraphicContext &p_gc)
{
	float x, y;

	foreach (const Crack &crack, m_crackPositions) {

		x = crack.m_position.x;
		y = crack.m_position.y;

		m_crackSprites[crack.m_crackId].draw(p_gc, x, y);
	}
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
	m_impl->loadSandTexture(p_gc);
	m_impl->loadCracks(p_gc);

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
	m_ldistances.reserve(count);
	m_rdistances.reserve(count);

	G_ASSERT(count >= 1);

	CL_Pointf prevPointL, prevPointR;
	float distL = 0.0f, distR = 0.0f;
	bool hasFirst = false;

	for (int i = 0; i < count; ++i) {
		m_distances.push_back(std::vector<float>());
		m_ldistances.push_back(std::vector<float>());
		m_rdistances.push_back(std::vector<float>());

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
			m_ldistances[i].push_back(distL);
			m_rdistances[i].push_back(distR);

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

void LevelImpl::loadSandTexture(CL_GraphicContext &p_gc)
{
	m_sandTexture = CL_Texture(
			"race/street_sand", Gfx::Stage::getResourceManager(), p_gc
	);

	m_sandTexture.set_wrap_mode(cl_wrap_clamp_to_edge, cl_wrap_repeat);
	m_sandTexture.set_generate_mipmap(true);
	m_sandTexture.set_mag_filter(cl_filter_linear);
	m_sandTexture.set_min_filter(cl_filter_linear);
}

void LevelImpl::loadCracks(CL_GraphicContext &p_gc)
{
	for (int i = 0; i < CRACKS_COUNT; ++i) {
		m_crackSprites[i] =
				CL_Sprite(
						p_gc,
						cl_format("race/street_crack_%1", (i + 1)),
						Stage::getResourceManager()
				);
	}


	// generate cracks
	static int MIN_CRACK_DIST = 50;
	static int MAX_CRACK_DIST = 200;

	const Race::Track &track = m_levelLogic.getTrack();
	const int count = track.getPointCount();

	CL_Pointf prevPointL, prevPointR;
	bool hasFirst = false;
	float dist = 0.0f;
	float next = rand() % (MAX_CRACK_DIST - MIN_CRACK_DIST) + MIN_CRACK_DIST;

	for (int i = 0; i < count; ++i) {
		const Race::TrackSegment &seg = m_triangulator.getSegment(i);
		const std::vector<Race::TrackSegment::PointPair> &pairs =
				seg.getPointPairs();

		foreach (const Race::TrackSegment::PointPair &pair, pairs) {
			if (hasFirst) {
				dist += (
						Units::toWorld(prevPointL.distance(pair.m_left))
						+ Units::toWorld(prevPointR.distance(pair.m_right))
				) / 2;
			} else {
				hasFirst = true;
			}

			if (dist >= next) {
				// if distance reached, then put the crack
				const CL_Pointf pos((pair.m_left + pair.m_right) / 2);
				const int index = rand() % CRACKS_COUNT;

				m_crackPositions.push_back(Crack(pos, index));

				// add new next distance
				next +=
						rand()
						% (MAX_CRACK_DIST - MIN_CRACK_DIST) + MIN_CRACK_DIST;
			}

			prevPointL = pair.m_left;
			prevPointR = pair.m_right;
		}
	}

}

Race::TrackTriangulator &Level::getTrackTriangulator()
{
	return const_cast<Race::TrackTriangulator&>(m_impl->m_triangulator);
}

void LevelImpl::expand(CL_Rectf *p_rect, const CL_Pointf &p_point)
{
	if (p_point.x < p_rect->left) {
		p_rect->left = p_point.x;
	} else if (p_point.x > p_rect->right) {
		p_rect->right = p_point.x;
	}

	if (p_point.y < p_rect->top) {
		p_rect->top = p_point.y;
	} else if (p_point.y > p_rect->bottom) {
		p_rect->bottom = p_point.y;
	}
}

} // namespace
