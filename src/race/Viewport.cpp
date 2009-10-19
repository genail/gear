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


#include "graphics/Stage.h"
#include "race/Viewport.h"

Viewport::Viewport() :
	m_x(0),
	m_y(0),
	m_width(Stage::getWidth()),
	m_height(Stage::getHeight()),
	m_attachPoint(NULL),
	m_scale(2.0f)
{
}

Viewport::~Viewport() {
}

void Viewport::prepareGC(CL_GraphicContext &p_gc) {
	p_gc.push_modelview();

	if (m_attachPoint != NULL) {
		const int stageWidth = Stage::getWidth();
		const int stageHeight = Stage::getHeight();

		m_x = m_attachPoint->x - stageWidth / 2 / m_scale;
		m_y = m_attachPoint->y - stageHeight / 2 / m_scale;
		m_width = stageWidth / m_scale;
		m_height = stageHeight / m_scale;
	}

	const float horizScale = p_gc.get_width() / m_width;
	const float vertScale = p_gc.get_height() / m_height;

	p_gc.mult_scale(horizScale, vertScale);

	p_gc.mult_translate(-m_x, -m_y);


}

void Viewport::finalizeGC(CL_GraphicContext &p_gc) {
	p_gc.pop_modelview();
}
