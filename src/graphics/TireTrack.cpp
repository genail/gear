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

#include "TireTrack.h"

namespace Gfx {

TireTrack::TireTrack()
{
}

TireTrack::~TireTrack()
{
}

void TireTrack::draw(CL_GraphicContext &p_gc)
{
	CL_Pen pen;
	pen.set_line_width(3);
	p_gc.set_pen(pen);

	// TODO: how to make blend?
	CL_Draw::line(p_gc, m_fromPoint, m_toPoint, CL_Colorf(0.0f, 0.0f, 0.0f, m_fromAlpha));
}

void TireTrack::load(CL_GraphicContext &p_gc)
{
	// nothing to load here
	Drawable::load(p_gc);
}

void TireTrack::setFromPoint(const CL_Pointf &p_from, float p_alpha)
{
	m_fromPoint = p_from;
	m_fromAlpha = p_alpha;
}

void TireTrack::setToPoint(const CL_Pointf &p_to, float p_alpha)
{
	m_toPoint = p_to;
	m_toAlpha = p_alpha;
}

} // namespace
