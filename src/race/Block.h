/*
 * Block.h
 *
 *  Created on: 2009-09-10
 *      Author: chudy
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include "graphics/Drawable.h"

class Block : public Drawable {
	public:

		enum BlockType {
			BT_NONE,
			BT_STREET_HORIZ,
			BT_STREET_VERT,
			BT_TURN_BOTTOM_RIGHT,
			BT_TURN_BOTTOM_LEFT,
			BT_TURN_TOP_RIGHT,
			BT_TURN_TOP_LEFT,
			BT_START_LINE
		};

		Block() {}
		Block(BlockType p_type, int p_width);
		virtual ~Block();

		virtual void draw(CL_GraphicContext& p_gc);
		virtual void load(CL_GraphicContext& p_gc);

		float getResistance(int x, int y) const;

		BlockType getType() const { return m_type; }

	private:
		/** Subtype of this block */
		BlockType m_type;

		/** Resistance map */
		float* m_resistanceMap;

		/** Width of block side */
		int m_width;

		/** Background sprite */
		CL_Sprite m_bgSprite;

		/** Primary Foreground sprite */
		CL_Sprite m_fgSprite;

		/** Secondary Foreground sprite */
		CL_Sprite m_fgSprite2;

};

class PixelTranslator {
	public:
		virtual void translate(int &p_x, int &p_y, int p_width, int p_height) {
		}
};

class PixelTranslator270 : public PixelTranslator {
	public:
		virtual void translate(int &p_x, int &p_y, int p_width, int p_height) {
			int tmp = p_x;
			p_x = p_y;
			p_y = p_width - tmp - 1;
		}
};

class PixelTranslator180 : public PixelTranslator {
	public:
		virtual void translate(int &p_x, int &p_y, int p_width, int p_height) {
			p_x = p_width - p_x - 1;
			p_y = p_height - p_y - 1;
		}
};

class PixelTranslator90 : public PixelTranslator {
	public:
		virtual void translate(int &p_x, int &p_y, int p_width, int p_height) {
			int tmp = p_x;
			p_x = p_height - p_y - 1;
			p_y = tmp;
		}
};

#endif /* BLOCK_H_ */
