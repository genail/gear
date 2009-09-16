/*
 * Block.cpp
 *
 *  Created on: 2009-09-10
 *      Author: chudy
 */

#include "Block.h"

#include "graphics/Stage.h"

Block::Block(BlockType p_type, int p_width) :
	m_type(p_type),
	m_width(p_width)
{
}

Block::~Block() {
}

void Block::draw(CL_GraphicContext& p_gc) {
	CL_Rectf drawRect(
			0, 0,
			CL_Sizef(m_width, m_width)
	);

	m_bgSprite.draw(p_gc, drawRect);

	if (!m_fgSprite.is_null()) {
		m_fgSprite.draw(p_gc, drawRect);
	}
}

void Block::load(CL_GraphicContext& p_gc) {
	m_bgSprite = CL_Sprite(p_gc, "race/block", Stage::getResourceManager());
	CL_String8 fgSpriteName;

	switch (m_type) {
		case Block::BT_STREET_VERT:
			fgSpriteName = "race/street_vert";
			break;
		case Block::BT_STREET_HORIZ:
			fgSpriteName = "race/street_horiz";
			break;
		case Block::BT_TURN_BOTTOM_RIGHT:
			fgSpriteName = "race/turn_bottom_right";
			break;
		case Block::BT_TURN_BOTTOM_LEFT:
			fgSpriteName = "race/turn_bottom_left";
			break;
		case Block::BT_TURN_TOP_RIGHT:
			fgSpriteName = "race/turn_top_right";
			break;
		case Block::BT_TURN_TOP_LEFT:
			fgSpriteName = "race/turn_top_left";
			break;
		case Block::BT_NONE:
			break;
	}

	if (fgSpriteName.size() > 0) {
		m_fgSprite = CL_Sprite(p_gc, fgSpriteName, Stage::getResourceManager());
	}
}
