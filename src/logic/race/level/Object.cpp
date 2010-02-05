/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#include "Object.h"

namespace Race
{

class ObjectImpl
{
	public:

		mutable CL_CollisionOutline m_outline;

		std::vector<CL_Pointf> *m_pts;


		ObjectImpl(const CL_Pointf p_points[], int p_count)
		{
			CL_Contour contour;
			m_pts = &contour.get_points();

			for (int i = 0; i < p_count; ++i) {
				m_pts->push_back(p_points[i]);
			}

			m_outline.get_contours().push_back(contour);
			m_outline.set_inside_test(true);
			m_outline.calculate_radius();
			m_outline.calculate_sub_circles();
			m_outline.enable_collision_info(true, false, true);
		}

};

const std::vector<CL_CollidingContours> EMPTY_CONTOURS;

Object::Object(const CL_Pointf p_points[], int p_count) :
	m_impl(new ObjectImpl(p_points, p_count))
{
	// empty
}

Object::~Object()
{
	// empty
}

const std::vector<CL_CollidingContours> &Object::collide(
		const CL_CollisionOutline &p_outline
) const
{
	if (m_impl->m_outline.collide(p_outline)) {
		return m_impl->m_outline.get_collision_info();
	} else {
		return EMPTY_CONTOURS;
	}
}

const CL_CollisionOutline &Object::getCollisionOutline() const
{
	return m_impl->m_outline;
}

const CL_Pointf &Object::getPoint(int p_idx) const
{
	G_ASSERT(p_idx >=0 && p_idx <= getPointCount());
	return (*m_impl->m_pts)[p_idx];
}

int Object::getPointCount() const
{
	return static_cast<signed>(
			m_impl->m_pts->size()
	);
}

}
