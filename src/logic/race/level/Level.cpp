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

#include <assert.h>

#include "common/Limits.h"
#include "common/Units.h"
#include "logic/race/Block.h"
#include "logic/race/level/Bound.h"
#include "logic/race/level/Checkpoint.h"
#include "logic/race/Car.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackTriangulator.h"
#include "logic/race/level/TrackPoint.h"
#include "logic/race/level/TrackSegment.h"
#include "logic/race/resistance/Geometry.h"
#include "logic/race/resistance/ResistanceMap.h"

namespace Race {

template <typename T>
class LoopVector : public std::vector<T>
{
	public:

		T &operator[] (int p_index) {
			return std::vector<T>::operator[](clamp(p_index));
		}

		const T &operator[] (int p_index) const {
			return std::vector<T>::operator[](clamp(p_index));
		}

	private:

		int clamp(int p_index) const {
			const int s = static_cast<signed> (std::vector<T>::size());

			if (p_index >= 0 && p_index < s) {
				return p_index;
			}

			p_index = p_index % s;

			if (p_index < 0) {
				p_index += s;
			}

			return p_index;
		}
};

class RoadPoint : public CL_Pointf
{
	public:

		RoadPoint(float p_x, float p_y, float p_modifier = 0.0f) :
			CL_Pointf(p_x, p_y),
			m_modifier(p_modifier)
		{}

		RoadPoint(const RoadPoint &copy) :
			CL_Pointf(copy.x, copy.y),
			m_modifier(copy.get_modifier())
		{}

		RoadPoint(const CL_Vec2f &copy) :
			CL_Pointf(copy.x, copy.y),
			m_modifier(0.0f)
		{}

		float get_modifier() const { return m_modifier; }

		void set_modifier(float p_modifier) { m_modifier = p_modifier; }

	private:
		/** -1.0 (left) to 1.0 (right) */
		float m_modifier;
};

class LevelImpl
{
	public:

		bool m_initialized;

		bool m_loaded;


		/** All cars */
		std::vector<Car*> m_cars;

		/** Map of start positions */
		std::map<int, CL_Pointf> m_startPositions;

		/** Orginal track */
		Track m_track;

		/** Triangulator object */
		TrackTriangulator m_trackTriangulator;

		/** Resistance mapping */
		RaceResistance::ResistanceMap m_resistanceMap;


		LevelImpl() :
			m_initialized(false),
			m_loaded(false)
			{}

		CL_SharedPtr<RaceResistance::Geometry> buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const;


		// level loading

		void loadMetaEl(const CL_DomNode &p_metaNode);

		void loadTrackEl(const CL_DomNode &p_trackNode);

		void loadBoundsEl(const CL_DomNode &p_boundsNode);


		// saving

		void saveTrackEl(CL_DomDocument &p_doc, CL_DomNode &p_trackNode);
};

Level::Level() :
		m_impl(new LevelImpl())
{
	// empty
}

Level::~Level()
{
	// empty
}

void Level::initialize()
{
	if (!m_impl->m_initialized) {
		m_impl->m_initialized = true;
	}
}

void Level::destroy()
{
	if (m_impl->m_initialized) {
		m_impl->m_resistanceMap.clear();
		m_impl->m_cars.clear();

		std::pair<Car*, CL_Pointf*> entry;

		m_impl->m_startPositions.clear();
		m_impl->m_track.clear();

		m_impl->m_loaded = false;
	}
}

void Level::load(const CL_String& p_filename)
{
	G_ASSERT(!m_impl->m_loaded && "level is already loaded");

	try {
		cl_log_event(LOG_DEBUG, "loading level %1", p_filename);
		CL_File file(p_filename, CL_File::open_existing, CL_File::access_read);

		CL_DomDocument document(file);
		const CL_DomElement root = document.get_document_element();

		// load meta element
		const CL_DomNode metaNode = root.named_item("meta");
		m_impl->loadMetaEl(metaNode);

		// gets level's content
		const CL_DomNode contentNode = root.named_item("content");

//		// load sand
//		const CL_DomNode sandNode = contentNode.named_item("sand");
//		loadSandElement(sandNode);

		// load track
		const CL_DomNode trackNode = contentNode.named_item("track");
		m_impl->loadTrackEl(trackNode);

		// load track bounds
		const CL_DomNode boundsNode = contentNode.named_item("bounds");
		m_impl->loadBoundsEl(boundsNode);

		file.close();

		cl_log_event(LOG_DEBUG, "level loaded");
		m_impl->m_loaded = true;

		// run triangulator
		m_impl->m_trackTriangulator.clear();
		m_impl->m_trackTriangulator.triangulate(m_impl->m_track);

	} catch (CL_Exception &e) {
		cl_log_event(LOG_ERROR, "cannot load level '%1': %2", p_filename, e.message);
	}

}

void LevelImpl::loadMetaEl(const CL_DomNode &p_metaNode)
{
	// TODO
}

void LevelImpl::loadTrackEl(const CL_DomNode &p_trackNode)
{
	const CL_DomNodeList blockList = p_trackNode.get_child_nodes();
	const int blockListSize = blockList.get_length();

	for (int i = 0; i < blockListSize; ++i) {
		const CL_DomNode blockNode = blockList.item(i);

		if (blockNode.get_node_name() == "point") {
			const float x = blockNode.select_float("@x");
			const float y = blockNode.select_float("@y");
			const float radius = blockNode.select_float("@radius");
			const float modifier = blockNode.select_float("@shift");

			cl_log_event(LOG_DEBUG, "Loaded track point %1 x %2, rad = %3, mod = %4", x, y, radius, modifier);
			m_track.addPoint(
					Units::toScreen(CL_Pointf(x, y)),
					Units::toScreen(radius),
					modifier
			);
		} else {
			cl_log_event(LOG_WARN, "Unknown element in <track>: %1", blockNode.get_node_name());
		}
	}

}

CL_SharedPtr<RaceResistance::Geometry> LevelImpl::buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const
{
	CL_SharedPtr<RaceResistance::Geometry> geom(new RaceResistance::Geometry());

	CL_Pointf p, q;
	CL_Pointf topLeft = Units::toScreen(CL_Pointf(p_x, p_y));
	CL_Pointf bottomRight = Units::toScreen(CL_Pointf(p_x + 1, p_y + 1));

	switch (p_blockType) {
		case Common::BT_GRASS:
			break;
		case Common::BT_STREET_HORIZ:
			p = Units::toScreen(CL_Pointf(p_x, p_y + 0.1f));
			q = Units::toScreen(CL_Pointf(p_x + 1, p_y + 0.9f));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		case Common::BT_STREET_VERT:
			p = Units::toScreen(CL_Pointf(p_x + 0.1f, p_y));
			q = Units::toScreen(CL_Pointf(p_x + 0.9f, p_y + 1));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		case Common::BT_TURN_BOTTOM_RIGHT:
			p = Units::toScreen(CL_Pointf(p_x + 1, p_y + 1));

			geom->addCircle(CL_Circlef(p, Units::toScreen(0.9f)));
			geom->subtractCircle(CL_Circlef(p, Units::toScreen(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));

			break;
		case Common::BT_TURN_BOTTOM_LEFT:
			p = Units::toScreen(CL_Pointf(p_x, p_y + 1));

			geom->addCircle(CL_Circlef(p, Units::toScreen(0.9f)));
			geom->subtractCircle(CL_Circlef(p, Units::toScreen(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));

			break;
		case Common::BT_TURN_TOP_RIGHT:
			p = Units::toScreen(CL_Pointf(p_x + 1, p_y));

			geom->addCircle(CL_Circlef(p, Units::toScreen(0.9f)));
			geom->subtractCircle(CL_Circlef(p, Units::toScreen(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));
			break;
		case Common::BT_TURN_TOP_LEFT:
			p = Units::toScreen(CL_Pointf(p_x, p_y));

			geom->addCircle(CL_Circlef(p, Units::toScreen(0.9f)));
			geom->subtractCircle(CL_Circlef(p, Units::toScreen(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));
			break;
		case Common::BT_START_LINE_UP:
			p = Units::toScreen(CL_Pointf(p_x + 0.1f, p_y));
			q = Units::toScreen(CL_Pointf(p_x + 0.9f, p_y + 1));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		default:
			assert(0 && "unknown block type");
	}

	return geom;
}

void LevelImpl::loadBoundsEl(const CL_DomNode &p_boundsNode)
{
//	const CL_DomNodeList boundList = p_boundsNode.get_child_nodes();
//	const int boundListSize = boundList.get_length();
//
//	for (int i = 0; i < boundListSize; ++i) {
//		const CL_DomNode boundNode = boundList.item(i);
//
//		if (boundNode.get_node_name() == "bound") {
//			CL_DomNamedNodeMap attrs = boundNode.get_attributes();
//
//			float x1 = CL_StringHelp::local8_to_float(attrs.get_named_item("x1").get_node_value());
//			float y1 = CL_StringHelp::local8_to_float(attrs.get_named_item("y1").get_node_value());
//			float x2 = CL_StringHelp::local8_to_float(attrs.get_named_item("x2").get_node_value());
//			float y2 = CL_StringHelp::local8_to_float(attrs.get_named_item("y2").get_node_value());
//
//			x1 *= Block::WIDTH;
//			y1 *= Block::WIDTH;
//			x2 *= Block::WIDTH;
//			y2 *= Block::WIDTH;
//
//			cl_log_event("debug", "Loading bound %1 x %2 -> %3 x %4", x1, y1, x2, y2);
//
//			const CL_LineSegment2f segment(CL_Pointf(x1, y1), CL_Pointf(x2, y2));
//			m_bounds.push_back(CL_SharedPtr<Bound>(new Bound(segment)));
//		} else {
//			cl_log_event("race", "Unknown node '%1', ignoring", boundNode.get_node_name());
//		}
//	}
}

void Level::save(const CL_String &p_filename)
{
	CL_File file;

	try {

		CL_DomDocument document;

		CL_DomElement root = document.create_element("level");
		document.append_child(root);

		CL_DomElement content = document.create_element("content");
		root.append_child(content);

		CL_DomElement track = document.create_element("track");
		content.append_child(track);

		m_impl->saveTrackEl(document, track);

		// save to file
		file = CL_File(p_filename, CL_File::create_always, CL_File::access_write);
		document.save(file, true);

		cl_log_event(LOG_DEBUG, "level '%1' saved", p_filename);

	} catch (CL_Exception &e) {
		cl_log_event(LOG_ERROR, "cannot save level '%1': %2", p_filename, e.message);
	}

	if (!file.is_null()) {
		file.close();
	}
}

void LevelImpl::saveTrackEl(CL_DomDocument &p_doc, CL_DomNode &p_trackNode)
{
	const int count = m_track.getPointCount();

	CL_DomElement pointEl;
	CL_DomAttr xAttr, yAttr, radiusAttr, shiftAttr;

	for (int i = 0; i < count; ++i) {
		const TrackPoint &point = m_track.getPoint(i);
		const CL_Pointf &pos = point.getPosition();

		pointEl = p_doc.create_element("point");
		p_trackNode.append_child(pointEl);

		pointEl.set_attribute("x", CL_StringHelp::float_to_local8(Units::toWorld(pos.x)));
		pointEl.set_attribute("y", CL_StringHelp::float_to_local8(Units::toWorld(pos.y)));
		pointEl.set_attribute("radius", CL_StringHelp::float_to_local8(Units::toWorld(point.getRadius())));
		pointEl.set_attribute("shift", CL_StringHelp::float_to_local8(point.getShift()));
	}
}

float Level::getResistance(float p_realX, float p_realY)
{
	return m_impl->m_resistanceMap.resistance(CL_Pointf(p_realX, p_realY));
}

void Level::addCar(Car *p_car) {

	assert(m_impl->m_loaded && "Level is not loaded");

	p_car->m_level = this;

	m_impl->m_cars.push_back(p_car);
}

void Level::removeCar(Car *p_car) {
	for (
		std::vector<Car*>::iterator itor = m_impl->m_cars.begin();
		itor !=m_impl-> m_cars.end();
		++itor
	) {
		if (*itor == p_car) {
			m_impl->m_cars.erase(itor);
			break;
		}
	}

	p_car->m_level = NULL;
}

void Level::getStartPosAndRot(
		int p_num,
		CL_Pointf *p_pos, CL_Angle *p_rot
) const
{
	G_ASSERT(m_impl->m_loaded);
	G_ASSERT(p_num >= 1 && "too low position number");
	G_ASSERT(p_num <= Limits::MAX_PLAYERS);

	// additional distance from start (per position)
	static const int DIST_MUL = static_cast<int>(Units::toScreen(3.0f));
	// distance from center
	static const int RADIUS = static_cast<int>(Units::toScreen(2.0f));
	// is first position on the right?
	static const bool RIGHT_FIRST = true;

	const int lastSegIdx = m_impl->m_track.getPointCount() - 1;

	// this will be car distance from start line
	const float cdist = p_num * DIST_MUL;

	float llen = 0, len = 0;
	CL_Pointf last;
	CL_LineSegment2f seg;
	float segPart;
	bool found;
	int lastMidsIdx;

	for (int i = lastSegIdx; i >= 0; --i) {
		// get track segment from the end
		const TrackSegment &s = m_impl->m_trackTriangulator.getSegment(i);
		const std::vector<CL_Pointf> &mids = s.getMidPoints();

		// find segment for car positioning
		found = false;
		lastMidsIdx = static_cast<signed>(mids.size()) - 1;

		for (int j = lastMidsIdx; j >= 0; --j) {
			const CL_Pointf &curr = mids[j];

			if (j != lastMidsIdx) {
				len += last.distance(curr);
			}

			if (cdist <= len) {
				// found segment
				seg.p = curr;
				seg.q = last;
				segPart = (len - cdist) / (len - llen);
				found = true;
				break;
			}

			last = curr;
			llen = len;
		}

		if (found) {
			break;
		}
	}

	if (!found) {
		G_ASSERT(p_num > 1 && "track too small?!");
		getStartPosAndRot(p_num - 1, p_pos, p_rot);
	}

	// calculate right normal
	// y, -x

	const CL_Vec2f segVec = seg.q - seg.p;
	CL_Vec2f norm(segVec.y, -segVec.x);

	norm.normalize();
	norm *= RADIUS;

	// calculate attach point
	CL_Pointf ap = seg.p;
	ap.x += (seg.q.x - seg.p.x) * segPart;
	ap.y += (seg.q.y - seg.p.y) * segPart;


	// put normal on proper side
	// (remember that y is reversed)
	if (p_num % 2 == 1) {
		if (RIGHT_FIRST) {
			norm *= -1;
		}
	} else {
		if (!RIGHT_FIRST) {
			norm *= -1;
		}
	}

	// save the position and rotation
	*p_pos = ap + norm;
	*p_rot = segVec.angle(CL_Vec2f(1.0f, 0.0f));

	if (segVec.y < 0) {
		*p_rot = CL_Angle(-(*p_rot).to_radians(), cl_radians);
	}
}


int Level::getCarCount() const
{
	return static_cast<int>(m_impl->m_cars.size());
}

const Car &Level::getCar(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < getCarCount());
	return *m_impl->m_cars[static_cast<unsigned>(p_index)];
}

Car &Level::getCar(int p_index)
{
	G_ASSERT(p_index >= 0 && p_index < getCarCount());
	return *m_impl->m_cars[static_cast<unsigned>(p_index)];
}

bool Level::isLoaded() const
{
	return m_impl->m_loaded;
}

const Track &Level::getTrack() const
{
	// track is available even if level is not loaded
	return m_impl->m_track;
}

void Level::setTrack(const Track &p_track)
{
	m_impl->m_track = p_track;
}

const TrackTriangulator &Level::getTrackTriangulator() const
{
	return m_impl->m_trackTriangulator;
}

TrackTriangulator &Level::getTrackTriangulator()
{
	return m_impl->m_trackTriangulator;
}

} // namespace
