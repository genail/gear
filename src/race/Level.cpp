/*
 * Level.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "Level.h"

#include <assert.h>
#include <ClanLib/display.h>

#include "graphics/Stage.h"

Level::Level() :
	m_blocks(NULL)
{
	loadFromFile("resources/level.txt");
}

Level::~Level() {
	if (m_blocks != NULL) {
		unload();
	}
}

void Level::draw(CL_GraphicContext &p_gc) {

	if (m_blockSprite.is_null()) {
		m_blockSprite = CL_Sprite(p_gc, "race/block", Stage::getResourceManager());
		m_streetVert = CL_Sprite(p_gc, "race/street_vert", Stage::getResourceManager());
		m_streetHoriz = CL_Sprite(p_gc, "race/street_horiz", Stage::getResourceManager());
		m_turnBottomRight = CL_Sprite(p_gc, "race/turn_bottom_right", Stage::getResourceManager());
		m_turnBottomLeft = CL_Sprite(p_gc, "race/turn_bottom_left", Stage::getResourceManager());
		m_turnTopRight = CL_Sprite(p_gc, "race/turn_top_right", Stage::getResourceManager());
		m_turnTopLeft = CL_Sprite(p_gc, "race/turn_top_left", Stage::getResourceManager());
	}

	const float boxWidth = 200;

	for (int x = 0; x < 10; ++x) {
		for (int y = 0; y < 10; ++y) {

			CL_Rectf drawRect(
					x * boxWidth,
					y * boxWidth,
					CL_Sizef(boxWidth, boxWidth)
			);

			m_blockSprite.draw(p_gc, drawRect);

			switch (m_blocks[m_width * y + x].getType()) {
				case Block::BT_STREET_VERT:
					m_streetVert.draw(p_gc, drawRect);
					break;
				case Block::BT_STREET_HORIZ:
					m_streetHoriz.draw(p_gc, drawRect);
					break;
				case Block::BT_TURN_BOTTOM_RIGHT:
					m_turnBottomRight.draw(p_gc, drawRect);
					break;
				case Block::BT_TURN_BOTTOM_LEFT:
					m_turnBottomLeft.draw(p_gc, drawRect);
					break;
				case Block::BT_TURN_TOP_RIGHT:
					m_turnTopRight.draw(p_gc, drawRect);
					break;
				case Block::BT_TURN_TOP_LEFT:
					m_turnTopLeft.draw(p_gc, drawRect);
					break;
				case Block::BT_NONE:
					break;
			}
		}
	}


	for (std::vector<Car*>::iterator itor = m_cars.begin(); itor != m_cars.end(); ++itor) {
		(*itor)->draw(p_gc);
	}

}

void Level::loadFromFile(const CL_String& p_filename)
{
	try {
		CL_File file(p_filename, CL_File::open_existing);
		CL_String8 line;

		bool hasSize = false;
		int row = 0;

		while (true) {
			line = file.read_string_text("", "\n", false);
			file.seek(1, CL_File::seek_cur);

			if (line.empty()) {
				break;
			}

			std::vector<CL_TempString> parts = CL_StringHelp::split_text(line, " ", true);

			if (!hasSize) {
				m_width = CL_StringHelp::text_to_int(parts[0]);
				m_height = CL_StringHelp::text_to_int(parts[2]);

				m_blocks = new Block[m_width * m_height];

				hasSize = true;
			} else {
				for (int i = 0; i < m_width; ++i) {
					m_blocks[m_width * row + i] = Block(decodeBlock(parts[i]));
				}

				++row;
			}
		}

	//	CL_DataBuffer dataBuffer(file.get_size());
	//	file.read(buffer.get_data(), buffer.get_size());
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
		default:
			assert(0 && "unknown char");
	}

}

float Level::getResistance(float p_x, float p_y) {
	return 0.0f;
}
