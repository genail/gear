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

#include "GroundBlock.h"

#include <assert.h>

#include <graphics/Stage.h>

namespace Gfx {

GroundBlock::GroundBlock(Common::GroundBlockType p_type) :
	m_type(p_type)
{
}

GroundBlock::~GroundBlock()
{
}

void GroundBlock::draw(CL_GraphicContext &p_gc)
{
	CL_Rect rect(
			m_position.x, m_position.y,
			m_position.x + 200, m_position.y + 200
	);
	m_sprite.draw(p_gc, rect);
}

void GroundBlock::load(CL_GraphicContext &p_gc)
{
	CL_String spriteName;

//	cl_log_event("debug", "block type: %1", m_type);

	switch (m_type) {
		case Common::BT_GRASS:
			spriteName = "race/block";
			break;
		case Common::BT_STREET_VERT:
			spriteName = "race/street_vert";
			break;
		case Common::BT_STREET_HORIZ:
			spriteName = "race/street_horiz";
			break;
		case Common::BT_TURN_BOTTOM_RIGHT:
			spriteName = "race/turn_bottom_right";
			break;
		case Common::BT_TURN_BOTTOM_LEFT:
			spriteName = "race/turn_bottom_left";
			break;
		case Common::BT_TURN_TOP_RIGHT:
			spriteName = "race/turn_top_right";
			break;
		case Common::BT_TURN_TOP_LEFT:
			spriteName = "race/turn_top_left";
			break;
		case Common::BT_START_LINE:
			spriteName = "race/street_vert";
			break;
		default:
			assert(0 && "unknown block type");
	}

	m_sprite = CL_Sprite(p_gc, spriteName, Gfx::Stage::getResourceManager());

}

} // namespace
