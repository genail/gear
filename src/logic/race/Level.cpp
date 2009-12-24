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

#include "Block.h"
#include "Bound.h"
#include "Checkpoint.h"
#include "Car.h"
#include "resistance/Geometry.h"

namespace Race {

Level::Level() :
	m_initialized(false),
	m_loaded(false),
	m_width(0),
	m_height(0)
{
}

void Level::initialize(const CL_String &p_filename)
{
	if (!m_initialized) {
		loadFromFile(p_filename);

		m_initialized = true;
	}
}

void Level::destroy()
{
	if (m_initialized) {
		m_blocks.clear();
		m_track.clear();
		m_bounds.clear();
		m_sandpits.clear();
		m_resistanceMap.clear();
		m_cars.clear();

		std::pair<Car*, CL_Pointf*> entry;
		foreach(entry, m_carsDriftPoints) {
			delete[] entry.second;
		}

		m_carsDriftPoints.clear();

		m_startPositions.clear();
		m_tyreStripes.clear();

		m_loaded = false;
	}
}

Level::~Level() {
	m_blocks.clear();
}

CL_String8 Level::readLine(CL_File& p_file) {
	CL_String8 line = p_file.read_string_text("", "\n", false);
	p_file.seek(1, CL_File::seek_cur);

	return line;
}

void Level::loadFromFile(const CL_String& p_filename)
{
	assert(!m_loaded && "level is already loaded");

	try {
		CL_File file(p_filename, CL_File::open_existing, CL_File::access_read);


		CL_DomDocument document(file);
		const CL_DomElement root = document.get_document_element();

		// load meta element
		const CL_DomNode metaNode = root.named_item("meta");
		loadMetaElement(metaNode);

		// gets level's content
		const CL_DomNode contentNode = root.named_item("content");

		// load sand
		const CL_DomNode sandNode = contentNode.named_item("sand");
		loadSandElement(sandNode);

		// load track
		const CL_DomNode trackNode = contentNode.named_item("track");
		loadTrackElement(trackNode);

		// load track bounds
		const CL_DomNode boundsNode = contentNode.named_item("bounds");
		loadBoundsElement(boundsNode);

		file.close();
		m_loaded = true;

	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}

}

void Level::loadMetaElement(const CL_DomNode &p_metaNode)
{
	m_width = p_metaNode.select_int("size/width");
	m_height = p_metaNode.select_int("size/height");

	cl_log_event("race", "level size set to %1 x %2", m_width, m_height);
}

void Level::loadTrackElement(const CL_DomNode &p_trackNode)
{
	// build block type map
	typedef std::map<CL_String, Common::GroundBlockType> blockMap_t;
	blockMap_t blockMap;
	blockMap_t::iterator blockMapItor;

	blockMap["vert"] = Common::BT_STREET_VERT;
	blockMap["horiz"] = Common::BT_STREET_HORIZ;
	blockMap["turn_bottom_right"] = Common::BT_TURN_BOTTOM_RIGHT;
	blockMap["turn_bottom_left"] = Common::BT_TURN_BOTTOM_LEFT;
	blockMap["turn_top_right"] = Common::BT_TURN_TOP_RIGHT;
	blockMap["turn_top_left"] = Common::BT_TURN_TOP_LEFT;
	blockMap["start_line_up"] = Common::BT_START_LINE_UP;

	// prepare level blocks
	const int blocksCount = m_width * m_height;
	m_blocks.clear();
	m_blocks.reserve(blocksCount);

	for (int i = 0; i < blocksCount; ++i) {
		m_blocks.push_back(CL_SharedPtr<Block>(new Block(Common::BT_GRASS)));
	}

	// create global resistance geometry
	CL_SharedPtr<RaceResistance::Geometry> globalResGeom(new RaceResistance::Geometry());
	globalResGeom->addRectangle(CL_Rectf(real(0), real(0), real(m_width), real(m_height)));

	m_resistanceMap.addGeometry(globalResGeom, 0.3f);

	// add sand resistance
	foreach (const Sandpit &sandpit, m_sandpits) {
		const unsigned circleCount = sandpit.getCircleCount();

		CL_SharedPtr<RaceResistance::Geometry> sandpitGeometry(new RaceResistance::Geometry());

		for (unsigned i = 0; i < circleCount; ++i) {
			// sandpit values are real
			const Sandpit::Circle &circle = sandpit.circleAt(i);
			sandpitGeometry->addCircle(CL_Circlef(circle.getCenter().x, circle.getCenter().y, circle.getRadius()));
		}

		m_resistanceMap.addGeometry(sandpitGeometry, 0.8f);
	}

	// read blocks
	const CL_DomNodeList blockList = p_trackNode.get_child_nodes();
	const int blockListSize = blockList.get_length();

	cl_log_event("debug", "Track node child count: %1", blockListSize);

	CL_Pointf lastCP; // last checkpoint

	for (int i = 0; i < blockListSize; ++i) {
		const CL_DomNode blockNode = blockList.item(i);

		if (blockNode.get_node_name() == "block") {
			CL_DomNamedNodeMap attrs = blockNode.get_attributes();

			const int x = CL_StringHelp::local8_to_int(attrs.get_named_item("x").get_node_value());
			const int y = CL_StringHelp::local8_to_int(attrs.get_named_item("y").get_node_value());
			const CL_String typeStr = attrs.get_named_item("type").get_node_value();

			if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
				cl_log_event("debug", "coords x=%1, y=%2", x, y);
				throw CL_Exception("Blocks coords out of bounds");
			}

			blockMapItor = blockMap.find(typeStr);

			if (blockMapItor != blockMap.end()) {

				const Common::GroundBlockType blockType = blockMapItor->second;
				m_blocks[m_width * y + x]->setType(blockType);

				// add checkpoint to track
				if (blockType == Common::BT_START_LINE_UP) {
					lastCP = CL_Pointf((x + 0.5f) * Block::WIDTH, (y + 0.2f) * Block::WIDTH);
					const CL_Pointf firstCP((x + 0.5f) * Block::WIDTH, (y + 0.2 - 0.01f) * Block::WIDTH);

					m_track.addCheckpointAtPosition(firstCP);
				} else {
					const CL_Pointf checkPosition((x + 0.5f) * Block::WIDTH, (y + 0.5f) * Block::WIDTH);

					m_track.addCheckpointAtPosition(checkPosition);
				}

				// add resistance geometry based on block
				CL_SharedPtr<RaceResistance::Geometry> resGeom = buildResistanceGeometry(x, y, blockType);
				m_resistanceMap.addGeometry(resGeom, 0.0f);

			} else {
				cl_log_event("race", "Unknown block type: %1", typeStr);
			}

		} else {
			cl_log_event("race", "Unknown node '%1', ignoring", blockNode.get_node_name());
		}

	}

	m_track.addCheckpointAtPosition(lastCP);
	m_track.close();

}

void Level::loadSandElement(const CL_DomNode &p_sandNode)
{
	const CL_DomNodeList sandChildren = p_sandNode.get_child_nodes();
	const int sandChildrenCount = sandChildren.get_length();

	CL_DomNode sandChildNode, groupChildNode;

	for (int i = 0; i < sandChildrenCount; ++i) {
		sandChildNode = sandChildren.item(i);

		if (sandChildNode.get_node_name() == "group") {
			const CL_DomNodeList groupChildren = sandChildNode.get_child_nodes();
			const int groupChildrenCount = groupChildren.get_length();

			// create new sandpit
			m_sandpits.push_back(Sandpit());
			Sandpit &sandpit = m_sandpits.back();

			for (int j = 0; j < groupChildrenCount; ++j) {
				groupChildNode = groupChildren.item(j);

				if (groupChildNode.get_node_name() == "circle") {

					CL_DomNamedNodeMap attrs = groupChildNode.get_attributes();

					const float x = CL_StringHelp::local8_to_float(attrs.get_named_item("x").get_node_value());
					const float y = CL_StringHelp::local8_to_float(attrs.get_named_item("y").get_node_value());
					const float radius = CL_StringHelp::local8_to_float(attrs.get_named_item("radius").get_node_value());

					// add to sandpit

					// must save as integer
					const CL_Pointf centerFloat = real(CL_Pointf(x, y));
					const CL_Point centerInt = CL_Point((int) floor(centerFloat.x), (int) floor(centerFloat.y));

					sandpit.addCircle(centerInt, real(radius));

//					m_resistanceMap.addGeometry(geom, 0.8f);
				} else {
					cl_log_event("error", "unknown element in <sand><group></group></sand>: <%1>", sandChildNode.get_node_name());
				}
			}
		} else {
			cl_log_event("error", "unknown element in <sand></sand>: <%1>", sandChildNode.get_node_name());
		}
	}
}

CL_SharedPtr<RaceResistance::Geometry> Level::buildResistanceGeometry(int p_x, int p_y, Common::GroundBlockType p_blockType) const
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

void Level::loadBoundsElement(const CL_DomNode &p_boundsNode)
{
	const CL_DomNodeList boundList = p_boundsNode.get_child_nodes();
	const int boundListSize = boundList.get_length();

	for (int i = 0; i < boundListSize; ++i) {
		const CL_DomNode boundNode = boundList.item(i);

		if (boundNode.get_node_name() == "bound") {
			CL_DomNamedNodeMap attrs = boundNode.get_attributes();

			float x1 = CL_StringHelp::local8_to_float(attrs.get_named_item("x1").get_node_value());
			float y1 = CL_StringHelp::local8_to_float(attrs.get_named_item("y1").get_node_value());
			float x2 = CL_StringHelp::local8_to_float(attrs.get_named_item("x2").get_node_value());
			float y2 = CL_StringHelp::local8_to_float(attrs.get_named_item("y2").get_node_value());

			x1 *= Block::WIDTH;
			y1 *= Block::WIDTH;
			x2 *= Block::WIDTH;
			y2 *= Block::WIDTH;

			cl_log_event("debug", "Loading bound %1 x %2 -> %3 x %4", x1, y1, x2, y2);

			const CL_LineSegment2f segment(CL_Pointf(x1, y1), CL_Pointf(x2, y2));
			m_bounds.push_back(CL_SharedPtr<Bound>(new Bound(segment)));
		} else {
			cl_log_event("race", "Unknown node '%1', ignoring", boundNode.get_node_name());
		}
	}
}

float Level::getResistance(float p_realX, float p_realY)
{
	return m_resistanceMap.resistance(CL_Pointf(p_realX, p_realY));
}

void Level::addCar(Car *p_car) {

	assert(m_loaded && "Level is not loaded");

	p_car->m_level = this;
	p_car->updateCurrentCheckpoint(NULL);


	m_cars.push_back(p_car);
	m_carsDriftPoints[p_car] = new CL_Pointf[4];
}

void Level::removeCar(Car *p_car) {
	for (
		std::vector<Car*>::iterator itor = m_cars.begin();
		itor != m_cars.end();
		++itor
	) {
		if (*itor == p_car) {
			m_cars.erase(itor);
			break;
		}
	}

	p_car->m_level = NULL;

	delete[] m_carsDriftPoints[p_car];
	m_carsDriftPoints.erase(m_carsDriftPoints.find(p_car));
}

CL_Pointf Level::getStartPosition(int p_num) const {

	std::map<int, CL_Pointf>::const_iterator startPositionItor = m_startPositions.find(p_num);

	if (startPositionItor != m_startPositions.end()) {
		return startPositionItor->second;
	} else {
		return CL_Pointf(200, 200);
	}

}

void Level::updateCheckpoints()
{
	foreach(Car *car, m_cars) {

		const Checkpoint *currentCheckpoint = car->getCurrentCheckpoint() ? car->getCurrentCheckpoint() : m_track.getFirst();

		// find next checkpoint
		bool movingForward, newLap;
		const Checkpoint *nextCheckpoint = m_track.check(car->getPosition(), currentCheckpoint, &movingForward, &newLap);

		// apply to car
		if (nextCheckpoint != currentCheckpoint) {
			car->updateCurrentCheckpoint(nextCheckpoint);

			if (!movingForward) {
				cl_log_event("debug", "Wrong way");
			}

			if (newLap) {
				cl_log_event("debug", "New lap");
			}
		}

	}
}

void Level::update(unsigned p_timeElapsed)
{
	updateCheckpoints();

#ifdef CLIENT

	checkCollistions();

#ifndef NO_TYRE_STRIPES
	foreach (Car* car, m_cars) {

		CL_Pointf* lastDriftPoints = m_carsDriftPoints[car];

		const CL_Pointf &carPosition = car->getPosition();

		if (car->isDrifting()) {

			static const float tyreRadius = 10.0f;
			CL_Angle carAngle(car->getRotationRad(), cl_radians);

			CL_Vec2f v;
			float rad;

			for (int i = 0; i < 4; ++i) {

				carAngle += CL_Angle(i == 0 ? 45 : 90, cl_degrees);

				rad = carAngle.to_radians();

				v.x = cos(rad);
				v.y = sin(rad);

				v.normalize();

				v *= tyreRadius;

				CL_Pointf stripePointEnd(carPosition);
				stripePointEnd += v;

				// when last drift point is valid, then add the tire stripe
				// if not, only update the drift point
				if (lastDriftPoints[i].x != 0.0f && lastDriftPoints[i].y != 0.0f) {

					CL_Pointf stripePointStart(lastDriftPoints[i]);

					m_tyreStripes.add(stripePointStart, stripePointEnd, car);
				}

				lastDriftPoints[i].x = stripePointEnd.x;
				lastDriftPoints[i].y = stripePointEnd.y;

			}

		} else {
			// nullify all tires drift positions when no drift is done
			for (int i = 0; i < 4; ++i) {
				lastDriftPoints[i].x = 0;
				lastDriftPoints[i].y = 0;
			}
		}
	}
#endif // !NO_TYRE_STRIPES
#endif // CLIENT
}

#ifdef CLIENT
void Level::checkCollistions()
{
	CL_CollisionOutline coll1, coll2;


	foreach (Car *c1, m_cars) {
		coll1 = c1->calculateCurrentCollisionOutline();

		// check car collisions
		// TODO: later :-)
//		foreach (Car *c2, m_cars) {
//
//			if (c1 == c2) {
//				continue;
//			}
//
//			coll2 = c2->calculateCurrentCollisionOutline();
//
//			if (coll1.collide(coll2)) {
//				cl_log_event("debug", "collision");
//			}
//		}

		// check bounds collisions
		foreach (const CL_SharedPtr<Bound> &bound, m_bounds) {
			if (coll1.collide(bound->getCollisionOutline())) {
				c1->performBoundCollision(*bound);
			}
		}
	}

}
#endif // CLIENT

CL_Pointf Level::real(const CL_Pointf &p_point) const
{
	return CL_Pointf(real(p_point.x), real(p_point.y));
}

float Level::real(float p_coord) const
{
	return p_coord * Block::WIDTH;
}

unsigned Level::getSandpitCount() const
{
	return m_sandpits.size();
}

const Sandpit &Level::sandpitAt(unsigned p_index) const
{
	assert(p_index < m_sandpits.size());
	return m_sandpits[p_index];
}

int Level::getCarCount() const
{
	return static_cast<int>(m_cars.size());
}

const Car &Level::getCar(int p_index) const
{
	G_ASSERT(p_index >= 0 && p_index < getCarCount());
	return *m_cars[static_cast<unsigned>(p_index)];
}

Car &Level::getCar(int p_index)
{
	G_ASSERT(p_index >= 0 && p_index < getCarCount());
	return *m_cars[static_cast<unsigned>(p_index)];
}

} // namespace
