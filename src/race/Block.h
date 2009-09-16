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
			BT_TURN_TOP_LEFT
		};

		Block() {}
		Block(BlockType p_type, int p_width);
		virtual ~Block();

		virtual void draw(CL_GraphicContext& p_gc);
		virtual void load(CL_GraphicContext& p_gc);

		BlockType getType() { return m_type; }

	private:
		/** Subtype of this block */
		BlockType m_type;

		/** Resistance map */
		float* m_resistanceMap;

		/** Width of block side */
		int m_width;

		/** Background sprite */
		CL_Sprite m_bgSprite;

		/** Foreground sprite */
		CL_Sprite m_fgSprite;

};

#endif /* BLOCK_H_ */
