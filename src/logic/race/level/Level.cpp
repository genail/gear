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

#include "logic/race/Block.h"
#include "logic/race/level/Bound.h"
#include "logic/race/level/Checkpoint.h"
#include "logic/race/Car.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackPoint.h"
#include "logic/race/TyreStripes.h"
#include "logic/race/resistance/Geometry.h"
#include "logic/race/resistance/ResistanceMap.h"

namespace Race {

template <typename T>
class LoopVector : public std::vector<T>
{
	public:

		T &operator[] (int p_index) {
			return std::vector<T>::operator[](norm(p_index));
		}

		const T &operator[] (int p_index) const {
			return std::vector<T>::operator[](norm(p_index));
		}

	private:

		int norm(int p_index) const {
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

		RoadPoint(float x, float y, float p_modifier = 0.0f) :
			CL_Pointf(x, y),
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

		/** Car's last drift points for all four tires: fr, rr, rl, fl */
		std::map<Car*, CL_Pointf*> m_carsDriftPoints;

		/** Map of start positions */
		std::map<int, CL_Pointf> m_startPositions;


		/** Checkpoint track */
		Track m_track;

		/** Tyre stripes */
		TyreStripes m_tyreStripes;

		/** Resistance mapping */
		RaceResistance::ResistanceMap m_resistanceMap;


		LevelImpl() :
			m_initialized(false),
			m_loaded(false)
			{}

		// level loading

		void loadMetaElement(const CL_DomNode &p_metaNode);

		void loadTrackElement(const CL_DomNode &p_trackNode);

		void loadBoundsElement(const CL_DomNode &p_boundsNode);

//		void loadSandElement(const CL_DomNode &p_sandNode);

		CL_SharedPtr<RaceResistance::Geometry> buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const;


		// helpers

		CL_Pointf real(const CL_Pointf &p_point) const;

		float real(float p_coord) const;


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
		foreach(entry, m_impl->m_carsDriftPoints) {
			delete[] entry.second;
		}

		m_impl->m_carsDriftPoints.clear();

		m_impl->m_startPositions.clear();
		m_impl->m_tyreStripes.clear();
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
		m_impl->loadMetaElement(metaNode);

		// gets level's content
		const CL_DomNode contentNode = root.named_item("content");

//		// load sand
//		const CL_DomNode sandNode = contentNode.named_item("sand");
//		loadSandElement(sandNode);

		// load track
		const CL_DomNode trackNode = contentNode.named_item("track");
		m_impl->loadTrackElement(trackNode);

		// load track bounds
		const CL_DomNode boundsNode = contentNode.named_item("bounds");
		m_impl->loadBoundsElement(boundsNode);

		file.close();

		cl_log_event(LOG_DEBUG, "level loaded");
		m_impl->m_loaded = true;

	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}

}

void Level::save(const CL_String &p_filename)
{

}

void LevelImpl::loadMetaElement(const CL_DomNode &p_metaNode)
{
	// TODO
}

void LevelImpl::loadTrackElement(const CL_DomNode &p_trackNode)
{
	const CL_DomNodeList blockList = p_trackNode.get_child_nodes();
	const int blockListSize = blockList.get_length();

	for (int i = 0; i < blockListSize; ++i) {
		const CL_DomNode blockNode = blockList.item(i);

		if (blockNode.get_node_name() == "point") {
			const float x = blockNode.select_float("@x");
			const float y = blockNode.select_float("@y");
			const float radius = blockNode.select_float("@radius");
			const float modifier = blockNode.select_float("@modifier");

			cl_log_event(LOG_DEBUG, "Loaded track point %1 x %2, rad = %3, mod = %4", x, y, radius, modifier);
			m_track.addPoint(real(CL_Pointf(x, y)), real(radius), modifier);
		} else {
			cl_log_event(LOG_WARN, "Unknown element in <track>: %1", blockNode.get_node_name());
		}
	}

}

CL_SharedPtr<RaceResistance::Geometry> LevelImpl::buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const
{
	CL_SharedPtr<RaceResistance::Geometry> geom(new RaceResistance::Geometry());

	CL_Pointf p, q;
	CL_Pointf topLeft = real(CL_Pointf(p_x, p_y));
	CL_Pointf bottomRight = real(CL_Pointf(p_x + 1, p_y + 1));

	switch (p_blockType) {
		case Common::BT_GRASS:
			break;
		case Common::BT_STREET_HORIZ:
			p = real(CL_Pointf(p_x, p_y + 0.1f));
			q = real(CL_Pointf(p_x + 1, p_y + 0.9f));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		case Common::BT_STREET_VERT:
			p = real(CL_Pointf(p_x + 0.1f, p_y));
			q = real(CL_Pointf(p_x + 0.9f, p_y + 1));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		case Common::BT_TURN_BOTTOM_RIGHT:
			p = real(CL_Pointf(p_x + 1, p_y + 1));

			geom->addCircle(CL_Circlef(p, real(0.9f)));
			geom->subtractCircle(CL_Circlef(p, real(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));

			break;
		case Common::BT_TURN_BOTTOM_LEFT:
			p = real(CL_Pointf(p_x, p_y + 1));

			geom->addCircle(CL_Circlef(p, real(0.9f)));
			geom->subtractCircle(CL_Circlef(p, real(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));

			break;
		case Common::BT_TURN_TOP_RIGHT:
			p = real(CL_Pointf(p_x + 1, p_y));

			geom->addCircle(CL_Circlef(p, real(0.9f)));
			geom->subtractCircle(CL_Circlef(p, real(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));
			break;
		case Common::BT_TURN_TOP_LEFT:
			p = real(CL_Pointf(p_x, p_y));

			geom->addCircle(CL_Circlef(p, real(0.9f)));
			geom->subtractCircle(CL_Circlef(p, real(0.1f)));

			geom->andRect(CL_Rectf(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y));
			break;
		case Common::BT_START_LINE_UP:
			p = real(CL_Pointf(p_x + 0.1f, p_y));
			q = real(CL_Pointf(p_x + 0.9f, p_y + 1));

			geom->addRectangle(CL_Rectf(p.x, p.y, q.x, q.y));
			break;
		default:
			assert(0 && "unknown block type");
	}

	return geom;
}

void LevelImpl::loadBoundsElement(const CL_DomNode &p_boundsNode)
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

float Level::getResistance(float p_realX, float p_realY)
{
	return m_impl->m_resistanceMap.resistance(CL_Pointf(p_realX, p_realY));
}

void Level::addCar(Car *p_car) {

	assert(m_impl->m_loaded && "Level is not loaded");

	p_car->m_level = this;
	p_car->updateCurrentCheckpoint(NULL);


	m_impl->m_cars.push_back(p_car);
	m_impl->m_carsDriftPoints[p_car] = new CL_Pointf[4];
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

	delete[] m_impl->m_carsDriftPoints[p_car];
	m_impl->m_carsDriftPoints.erase(m_impl->m_carsDriftPoints.find(p_car));
}

CL_Pointf Level::getStartPosition(int p_num) const {

	std::map<int, CL_Pointf>::const_iterator startPositionItor = m_impl->m_startPositions.find(p_num);

	if (startPositionItor != m_impl->m_startPositions.end()) {
		return startPositionItor->second;
	} else {
		return CL_Pointf(200, 200);
	}

}

CL_Pointf LevelImpl::real(const CL_Pointf &p_point) const
{
	return CL_Pointf(p_point.x * 15.0f, p_point.y * 15.0f);
}

float LevelImpl::real(float p_coord) const
{
	return p_coord * 15.0f;
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

const TyreStripes &Level::getTyreStripes() const
{
	return m_impl->m_tyreStripes;
}

bool Level::isLoaded() const
{
	return m_impl->m_loaded;
}

const Track &Level::getTrack() const
{
	G_ASSERT(m_impl->m_loaded);
	return m_impl->m_track;
}

void Level::setTrack(const Track &p_track)
{
	m_impl->m_track.clear();
	const int size = p_track.getPointCount();

	for (int i = 0; i < size; ++i) {
		const TrackPoint &p_point = p_track.getPoint(i);

		m_impl->m_track.addPoint(
				p_point.getPosition(), p_point.getRadius(), p_point.getShift()
		);
	}
}

} // namespace
