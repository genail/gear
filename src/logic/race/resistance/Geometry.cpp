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

#include "Geometry.h"

#include <assert.h>

#include "Circle.h"
#include "Rectangle.h"
#include "Primitive.h"

namespace RaceResistance {

Geometry::Geometry() :
	m_boundsSet(false)
{
}

Geometry::~Geometry()
{
	foreach (const Primitive *pr, m_primitives) {
		delete pr;
	}
}

const CL_Rectf &Geometry::getBounds() const
{
	assert(m_boundsSet && "bounds not set yet");
	return m_bounds;
}

void Geometry::addCircle(const CL_Circlef &p_circle)
{
	const Primitive *pr = new Circle(p_circle, Primitive::IT_ADD);
	m_primitives.push_back(pr);

	updateBounds(p_circle);
}

void Geometry::addRectangle(const CL_Rectf &p_rectangle)
{
	const Primitive *pr = new Rectangle(p_rectangle, Primitive::IT_ADD);
	m_primitives.push_back(pr);

	updateBounds(p_rectangle);
}

void Geometry::subtractCircle(const CL_Circlef &p_circle)
{
	const Primitive *pr = new Circle(p_circle, Primitive::IT_SUB);
	m_primitives.push_back(pr);

	updateBounds(p_circle);
}

void Geometry::subtractRect(const CL_Rectf &p_rectangle)
{
	const Primitive *pr = new Rectangle(p_rectangle, Primitive::IT_SUB);
	m_primitives.push_back(pr);

	updateBounds(p_rectangle);
}

void Geometry::andCircle(const CL_Circlef &p_circle)
{
	const Primitive *pr = new Circle(p_circle, Primitive::IT_AND);
	m_primitives.push_back(pr);

	updateBounds(p_circle);
}

void Geometry::andRect(const CL_Rectf &p_rectangle)
{
	const Primitive *pr = new Rectangle(p_rectangle, Primitive::IT_AND);
	m_primitives.push_back(pr);

	updateBounds(p_rectangle);
}

void Geometry::updateBounds(const CL_Circlef &p_circle)
{
	const float l = p_circle.position.x - p_circle.radius;
	const float t = p_circle.position.y - p_circle.radius;
	const float r = p_circle.position.x + p_circle.radius;
	const float b = p_circle.position.y + p_circle.radius;

	updateBounds(l, t, r, b);
}

void Geometry::updateBounds(const CL_Rectf &p_rect)
{
	updateBounds(p_rect.left, p_rect.top, p_rect.right, p_rect.bottom);
}

void Geometry::updateBounds(float p_l, float p_t, float p_r, float p_b)
{
	if (!m_boundsSet) {
		m_bounds.left = p_l;
		m_bounds.top = p_t;
		m_bounds.right = p_r;
		m_bounds.bottom = p_b;

		m_boundsSet = true;
	} else {
		if (p_l < m_bounds.left) {
			m_bounds.left = p_l;
		}

		if (p_t < m_bounds.top) {
			m_bounds.top = p_t;
		}

		if (p_r > m_bounds.right) {
			m_bounds.right = p_r;
		}

		if (p_b > m_bounds.bottom) {
			m_bounds.bottom = p_b;
		}
	}
}

bool Geometry::contains(const CL_Pointf &p_point) const
{
	bool result = false;

	foreach (const Primitive *pr, m_primitives) {
		switch (pr->getInsertionType()) {
			case Primitive::IT_ADD:
				if (result == false) {
					result = pr->contains(p_point);
				}
				break;
			case Primitive::IT_SUB:
				if (result == true) {
					result = !pr->contains(p_point);
				}
				break;
			case Primitive::IT_AND:
				result = result && pr->contains(p_point);

			default:
				G_ASSERT(0 && "unknown InsertionType");
		}
	}

	return result;
}

} // namespace
