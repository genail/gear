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

#include "TrackSegment.h"

#include "common.h"

namespace Race
{

class TrackSegmentImpl
{
	public:

		const std::vector<CL_Pointf> m_triPoints;

		const std::vector<CL_Pointf> m_midPoints;

		const std::vector<TrackSegment::PointPair> m_pairs;

		CL_Rectf m_bounds;


		TrackSegmentImpl(
				const std::vector<CL_Pointf> &p_triPoints,
				const std::vector<CL_Pointf> &p_midPoints,
				const std::vector<TrackSegment::PointPair> &p_pairs
		) :
			m_triPoints(p_triPoints),
			m_midPoints(p_midPoints),
			m_pairs(p_pairs)
		{ /* empty */ }


		void calculateBounds();
};

TrackSegment::TrackSegment(
		const std::vector<CL_Pointf> &p_triPoints,
		const std::vector<CL_Pointf> &p_midPoints,
		const std::vector<PointPair> &p_pairs
) :
	m_impl(new TrackSegmentImpl(p_triPoints, p_midPoints, p_pairs))
{
	m_impl->calculateBounds();
}

TrackSegment::~TrackSegment()
{
	// empty
}

void TrackSegmentImpl::calculateBounds()
{
	bool first = true;

	foreach (const CL_Pointf &p, m_triPoints) {

		if (!first) {
			if (p.x < m_bounds.left) {
				m_bounds.left = p.x;
			} else if (p.x > m_bounds.right) {
				m_bounds.right = p.x;
			}

			if (p.y < m_bounds.top) {
				m_bounds.top = p.y;
			} else if (p.y > m_bounds.bottom) {
				m_bounds.bottom = p.y;
			}
		} else {
			m_bounds.left = p.x;
			m_bounds.right = p.x;
			m_bounds.top = p.y;
			m_bounds.bottom = p.y;

			first = false;
		}
	}
}

const CL_Rectf &TrackSegment::getBounds() const
{
	return m_impl->m_bounds;
}

const std::vector<CL_Pointf> &TrackSegment::getMidPoints() const
{
	return m_impl->m_midPoints;
}

const std::vector<CL_Pointf> &TrackSegment::getTrianglePoints() const
{
	return m_impl->m_triPoints;
}

const std::vector<TrackSegment::PointPair> &TrackSegment::getPointPairs() const
{
	return m_impl->m_pairs;
}

}
