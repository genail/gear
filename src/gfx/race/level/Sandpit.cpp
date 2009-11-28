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

Sandpit::Sandpit() :
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

}

void Sandpit::addCircle(const CL_Point &p_center, float p_radius)
{
	Circle circle;
	circle.m_center = p_center;
	circle.m_radius = p_radius;

	m_circles.push_back(circle);
}

void Sandpit::build()
{
	CL_Rect bounds = calculateCircleBounds();

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

	foreach (const Circle &circle, m_circles) {

		// here y points to bottom
		circleBounds.left = circle.m_center.x - circle.m_radius;
		circleBounds.right = circle.m_center.x + circle.m_radius;
		circleBounds.top = circle.m_center.y - circle.m_radius;
		circleBounds.bottom = circle.m_center.y + circle.m_radius;


		int index;

		int tx = -p_totalBounds.left;
		int ty = -p_totalBounds.top;

		cl_log_event("debug", "tx = %1, ty = %2", tx, ty);

		for (int y = circleBounds.top; y <= circleBounds.bottom; ++y) {
			for (int x = circleBounds.left; x <= circleBounds.right; ++x) {
				CL_Point point(x, y);

				if (point.distance(circle.m_center) <= circle.m_radius) {

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

	foreach (const Circle &circle, m_circles) {

		l = circle.m_center.x - circle.m_radius;
		r = circle.m_center.x + circle.m_radius;
		t = circle.m_center.y - circle.m_radius;
		b = circle.m_center.y + circle.m_radius;

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
