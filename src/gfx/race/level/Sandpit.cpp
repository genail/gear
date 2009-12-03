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

#include "Sandpit.h"

#include <ClanLib/core.h>
#include <ClanLib/gl1.h>
#include <assert.h>

#include "common.h"

namespace Gfx {

Sandpit::Sandpit(const Race::Sandpit *p_logicSandpit) :
	m_logicSandpit(p_logicSandpit),
	m_built(false)
{
}

Sandpit::~Sandpit()
{
}

void Sandpit::setPosition(const CL_Pointf &p_position)
{
	m_position = p_position;
}

void Sandpit::draw(CL_GraphicContext &p_gc)
{
	assert(m_built);

	if (m_texture.is_null()) {
		// create texture
		m_texture = CL_SharedPtr<CL_Texture>(
				new CL_Texture(
						p_gc,
						m_pixelData->get_width(), m_pixelData->get_height()
				)
		);

		m_texture->set_image(*m_pixelData);
	}

	p_gc.set_texture(0, *m_texture);

	CL_Rectf drawRect(0, 0, m_pixelData->get_width(), m_pixelData->get_height());

	p_gc.push_translate(m_position.x, m_position.y);
	CL_Draw::texture(p_gc, drawRect, CL_Colorf::white);
	p_gc.pop_modelview();
}

void Sandpit::load(CL_GraphicContext &p_gc)
{
	build();
	Drawable::load(p_gc);
}

void Sandpit::build()
{
	CL_Rect bounds = calculateCircleBounds();

	setPosition(CL_Pointf(bounds.left, bounds.top));

	const float MAX_WIDTH = 512;
	const float MAX_HEIGHT = 512;

	// prepare image data
	const int width = bounds.get_width() + 1;
	const int height = bounds.get_height() + 1;

	assert(width <= MAX_WIDTH && width > 0);
	assert(height <= MAX_HEIGHT && height > 0);

	// prepare pixel data
	m_pixelData = CL_SharedPtr<CL_PixelBuffer>(new CL_PixelBuffer(width, height, width * 4, CL_PixelFormat::rgba8888));

	fillCircles(width, height, bounds);

	// unset the texture to create is at the next draw
	m_texture.disconnect();

	m_built = true;
}

void Sandpit::fillCircles(int p_width, int p_height, const CL_Rect& p_totalBounds)
{
	int strike = p_width;
	int *data = (int*) m_pixelData->get_data();

	// clear all
	memset(data, 0, strike * p_height);

	// fill circles
	CL_Rect circleBounds;

	const unsigned circleCount = m_logicSandpit->getCircleCount();

	for (unsigned i = 0; i < circleCount; ++i) {
		const Race::Sandpit::Circle &circle = m_logicSandpit->circleAt(i);

		// here y points to bottom
		circleBounds.left = circle.getCenter().x - circle.getRadius();
		circleBounds.right = circle.getCenter().x + circle.getRadius();
		circleBounds.top = circle.getCenter().y - circle.getRadius();
		circleBounds.bottom = circle.getCenter().y + circle.getRadius();


		int index;

		int tx = -p_totalBounds.left;
		int ty = -p_totalBounds.top;

		cl_log_event("debug", "tx = %1, ty = %2", tx, ty);

		for (int y = circleBounds.top; y <= circleBounds.bottom; ++y) {
			for (int x = circleBounds.left; x <= circleBounds.right; ++x) {
				CL_Point point(x, y);

				if (point.distance(circle.getCenter()) <= circle.getRadius()) {

					index = (y + ty) * strike + (x + tx);
					cl_log_event("debug", "index = %1", index);

					assert(index < p_width * p_height && index >= 0);
					data[index] = 0xFFFF00FF; // yellow
				}
			}
		}
	}
}

CL_Rect Sandpit::calculateCircleBounds()
{
	CL_Rect bounds;
	bool first = true;
	int l, t, r, b;

	const unsigned circleCount = m_logicSandpit->getCircleCount();

	for (unsigned i = 0; i < circleCount; ++i) {
		const Race::Sandpit::Circle &circle = m_logicSandpit->circleAt(i);

		l = circle.getCenter().x - circle.getRadius();
		r = circle.getCenter().x + circle.getRadius();
		t = circle.getCenter().y - circle.getRadius();
		b = circle.getCenter().y + circle.getRadius();

		if (first) {
			bounds.left = l;
			bounds.right = r;
			bounds.top = t;
			bounds.bottom = b;

			first = false;
		} else {
			if (l < bounds.left) {
				bounds.left = l;
			}

			if (r > bounds.right) {
				bounds.right = r;
			}

			if (t < bounds.top) {
				bounds.top = t;
			}

			if (b > bounds.bottom) {
				bounds.bottom = b;
			}
		}
	}

	return bounds;
}

}
