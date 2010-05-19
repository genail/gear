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

#include "TrackPoint.h"

#include "clanlib/core/math.h"

namespace Race {

class TrackPointImpl
{
	public:

		CL_Pointf m_position;

		float m_radius;

		float m_shift;

		TrackPointImpl(const CL_Pointf &p_position, float p_radius, float p_shift) :
			m_position(p_position),
			m_radius(p_radius),
			m_shift(p_shift)
		{}
};

TrackPoint::TrackPoint(const CL_Pointf &p_position, float p_radius, float p_shift) :
		m_impl(new TrackPointImpl(p_position, p_radius, p_shift))
{
}

TrackPoint::~TrackPoint()
{
}

const CL_Pointf &TrackPoint::getPosition() const
{
	return m_impl->m_position;
}

float TrackPoint::getRadius() const
{
	return m_impl->m_radius;
}

float TrackPoint::getShift() const
{
	return m_impl->m_shift;
}

void TrackPoint::setPosition(const CL_Pointf &p_pos)
{
	m_impl->m_position = p_pos;
}

void TrackPoint::setRadius(float p_radius)
{
	m_impl->m_radius = p_radius;
}

void TrackPoint::setShift(float p_shift)
{
	m_impl->m_shift = p_shift;
}

}
