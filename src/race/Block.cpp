/*
 * Block.cpp
 *
 *  Created on: 2009-09-10
 *      Author: chudy
 */

#define GROUND_RESIST 1.5f;

#include "Block.h"

#include <cstdlib>

Block::Block(BlockType p_type, int p_width) :
	m_type(p_type),
	m_resistanceMap(NULL),
	m_width(p_width)
{
}

Block::~Block() {
}


#ifdef CLIENT
void Block::draw(CL_GraphicContext& p_gc) {
	CL_Rectf drawRect(
			0, 0,
			CL_Sizef(m_width, m_width)
	);

	m_bgSprite.draw(p_gc, drawRect);

	if (!m_fgSprite.is_null()) {
		m_fgSprite.draw(p_gc, drawRect);

		if (!m_fgSprite2.is_null()) {
			m_fgSprite2.draw(p_gc, drawRect);
		}
	}
}

void Block::load(CL_GraphicContext& p_gc) {
	m_bgSprite = CL_Sprite(p_gc, "race/block", Stage::getResourceManager());

	CL_String8 fgSpriteName;
	CL_String8 fgSpriteName2;
	CL_String8 resistanceMapName;
	PixelTranslator *pixelTranslator;

	switch (m_type) {
		case Block::BT_STREET_VERT:
			fgSpriteName = "race/street_vert";
			resistanceMapName = "resources/res_map_vert.png";
			pixelTranslator = new PixelTranslator();
			break;
		case Block::BT_STREET_HORIZ:
			fgSpriteName = "race/street_horiz";
			resistanceMapName = "resources/res_map_vert.png";
			pixelTranslator = new PixelTranslator90();
			break;
		case Block::BT_TURN_BOTTOM_RIGHT:
			fgSpriteName = "race/turn_bottom_right";
			resistanceMapName = "resources/res_map_bottom_right.png";
			pixelTranslator = new PixelTranslator();
			break;
		case Block::BT_TURN_BOTTOM_LEFT:
			fgSpriteName = "race/turn_bottom_left";
			resistanceMapName = "resources/res_map_bottom_right.png";
			pixelTranslator = new PixelTranslator270();
			break;
		case Block::BT_TURN_TOP_RIGHT:
			fgSpriteName = "race/turn_top_right";
			resistanceMapName = "resources/res_map_bottom_right.png";
			pixelTranslator = new PixelTranslator90();
			break;
		case Block::BT_TURN_TOP_LEFT:
			fgSpriteName = "race/turn_top_left";
			resistanceMapName = "resources/res_map_bottom_right.png";
			pixelTranslator = new PixelTranslator180();
			break;
		case Block::BT_START_LINE:
			fgSpriteName = "race/street_vert";
			fgSpriteName2 = "race/start_line";
			resistanceMapName = "resources/res_map_vert.png";
			pixelTranslator = new PixelTranslator();
		case Block::BT_NONE:
			break;
	}

	if (fgSpriteName.size() > 0) {
		m_fgSprite = CL_Sprite(p_gc, fgSpriteName, Stage::getResourceManager());

		if (fgSpriteName2.size() > 0) {
			m_fgSprite2 = CL_Sprite(p_gc, fgSpriteName2, Stage::getResourceManager());
		}

		// build resistance map
		CL_PixelBuffer resistanceImage = CL_ImageProviderFactory::load(resistanceMapName);
		m_resistanceMap = new float[m_width * m_width];

		const int w = resistanceImage.get_width();
		const int h = resistanceImage.get_height();
		int x2, y2;
		int resIndex;
		CL_Color color;

		const float wRatio = m_width / (float) w;
		const float hRatio = m_width / (float) h;

		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				x2 = x;
				y2 = y;
				pixelTranslator->translate(x2, y2, w, h);

				color = resistanceImage.get_pixel(x2, y2);

				resIndex = (int) (y * hRatio) * (int) (w * wRatio) + x * wRatio;
//				CL_Console::write_line(CL_StringHelp::int_to_local8(resIndex));

				if (color.get_red() == 0) {
					m_resistanceMap[resIndex] = GROUND_RESIST;
				} else {
					m_resistanceMap[resIndex] = 0.0f;
				}
			}
		}

	}
}

#endif // CLIENT

float Block::getResistance(int x, int y) const {
	if (m_resistanceMap == NULL) {
		return GROUND_RESIST;
	}

	return m_resistanceMap[y * m_width + x];
}
