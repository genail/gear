/*
 * Block.h
 *
 *  Created on: 2009-09-10
 *      Author: chudy
 */

#ifndef BLOCK_H_
#define BLOCK_H_

class Block {
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
		Block(BlockType p_type);
		virtual ~Block();

		BlockType getType() { return m_type; }

	private:
		BlockType m_type;
};

#endif /* BLOCK_H_ */
