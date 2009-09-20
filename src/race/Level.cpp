/*
 * Level.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#define BOX_WIDTH 200

#include "Level.h"

#include <assert.h>
#include <ClanLib/display.h>

#include "graphics/Stage.h"
#include "race/Checkpoint.h"

Level::Level() :
	m_blocks(NULL),
	m_width(0),
	m_height(0)
{
	loadFromFile("resources/level.txt");
}

Level::~Level() {
	if (m_blocks != NULL) {
		unload();
	}
}

void Level::draw(CL_GraphicContext &p_gc) {

	for (int x = 0; x < 10; ++x) {
		for (int y = 0; y < 10; ++y) {
			p_gc.push_translate(x * BOX_WIDTH, y * BOX_WIDTH);

			m_blocks[m_width * y + x].draw(p_gc);

			p_gc.pop_modelview();
		}
	}


	for (std::vector<Car*>::iterator itor = m_cars.begin(); itor != m_cars.end(); ++itor) {
		(*itor)->draw(p_gc);
	}

	// draw bounds
	for (std::vector<Bound>::iterator itor = m_bounds.begin(); itor != m_bounds.end(); ++itor) {
		(*itor).draw(p_gc);
	}

}

void Level::load(CL_GraphicContext &p_gc) {
	for (int x = 0; x < m_width; ++x) {
			for (int y = 0; y < m_height; ++y) {
				m_blocks[m_width * y + x].load(p_gc);
			}
		}
}

CL_String8 Level::readLine(CL_File& p_file) {
	CL_String8 line = p_file.read_string_text("", "\n", false);
	p_file.seek(1, CL_File::seek_cur);

	return line;
}

void Level::loadFromFile(const CL_String& p_filename)
{
	try {
		CL_File file(p_filename, CL_File::open_existing);
		CL_String8 line;

		line = readLine(file);

		std::vector<CL_TempString> parts = CL_StringHelp::split_text(line, " ", true);

		m_width = CL_StringHelp::text_to_int(parts[0]);
		m_height = CL_StringHelp::text_to_int(parts[2]);

		m_blocks = new Block[m_width * m_height];

//		int row = 0;

		for (int i = 0; i < m_height; ++i) {
			line = readLine(file);

			parts = CL_StringHelp::split_text(line, " ", true);

			for (int j = 0; j < m_width; ++j) {
				m_blocks[m_width * i + j] = Block(decodeBlock(parts[j]), BOX_WIDTH);
			}

//			++row;
		}

		// read bounds num
		line = readLine(file);
		int boundsCount = CL_StringHelp::text_to_int(line);

		float x1, y1, x2, y2;

		for (int i = 0; i < boundsCount; ++i) {
			line = readLine(file);
			parts = CL_StringHelp::split_text(line, " ", true);

			x1 = CL_StringHelp::text_to_float(parts[0]) * BOX_WIDTH;
			y1 = CL_StringHelp::text_to_float(parts[1]) * BOX_WIDTH;
			x2 = CL_StringHelp::text_to_float(parts[2]) * BOX_WIDTH;
			y2 = CL_StringHelp::text_to_float(parts[3]) * BOX_WIDTH;

			Bound bound(CL_LineSegment2f(CL_Vec2f(x1, y1), CL_Vec2f(x2, y2)));
			m_bounds.push_back(bound);
		}


		file.close();


	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}


}

void Level::unload() {
//	const int blocksTotal = m_width * m_height;
//	for (int i = 0; i < blocksTotal; ++i) {
//	}

	delete[] m_blocks;
}

Block::BlockType Level::decodeBlock(const CL_String8& p_str) {

	const char c = p_str[0];

	switch (c) {
		case '0':
			return Block::BT_NONE;
		case '|':
			return Block::BT_STREET_VERT;
		case '-':
			return Block::BT_STREET_HORIZ;
		case '1':
			return Block::BT_TURN_BOTTOM_RIGHT;
		case '2':
			return Block::BT_TURN_BOTTOM_LEFT;
		case '3':
			return Block::BT_TURN_TOP_LEFT;
		case '4':
			return Block::BT_TURN_TOP_RIGHT;
		case '5':
			return Block::BT_START_LINE;
		default:
			assert(0 && "unknown char");
	}

}

float Level::getResistance(float p_x, float p_y) {
	if (p_x < 0 || p_y < 0 || p_x >= BOX_WIDTH * m_width || p_y >= BOX_WIDTH * m_height) {
		return 0.0f;
	}

	int blockX = (int) floor(p_x / BOX_WIDTH);
	int blockY = (int) floor(p_y / BOX_WIDTH);

	int localX = (int) (p_x - blockX * BOX_WIDTH);
	int localY = (int) (p_y - blockY * BOX_WIDTH);

	return m_blocks[blockY * m_width + blockX].getResistance(localX, localY);
}

void Level::addCar(Car *p_car) {
	p_car->m_level = this;

	// fill car checkpoints
	for (int x = 0; x < m_width; ++x) {
		for (int y = 0; y < m_height; ++y) {

			const Block &block = getBlock(x, y);

			if (block.getType() != Block::BT_NONE) {

				const CL_Rectf rect(x * BOX_WIDTH, y * BOX_WIDTH, (x + 1) * BOX_WIDTH, (y + 1) * BOX_WIDTH);
				const Checkpoint checkpoint(rect);

				p_car->m_checkpoints.push_back(checkpoint);

			}

			// if this is a start/finish line, then add finish line checkpoint
			if (block.getType() == Block::BT_START_LINE) {
				const CL_Rectf rect(x * BOX_WIDTH, (y - 1) * BOX_WIDTH, (x + 1) * BOX_WIDTH, y * BOX_WIDTH);
				p_car->m_lapCheckpoint = Checkpoint(rect);
			}
		}
	}


	m_cars.push_back(p_car);
}
