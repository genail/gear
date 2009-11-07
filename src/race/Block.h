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

#ifndef RACE_BLOCK_H_
#define RACE_BLOCK_H_

#include "boost/utility.hpp"

namespace Race {

class Block : public boost::noncopyable
{
	public:

		enum BlockType {
			BT_GRASS,
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

		float getResistance(int x, int y) const;

		BlockType getType() const { return m_type; }

	private:
		/** Subtype of this block */
		BlockType m_type;

		/** Resistance map */
		float *m_resistanceMap;

		/** Width of block in pixels */
		int m_width;

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

} // namespace

#endif /* RACE_BLOCK_H_ */
