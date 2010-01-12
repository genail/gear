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

#include "Track.h"

#include <assert.h>

#include "common.h"
#include "logic/race/level/Checkpoint.h"
#include "logic/race/level/TrackPoint.h"

namespace Race {

class TrackImpl
{
	public:

		typedef std::vector<TrackPoint> TTrackPoints;

		TTrackPoints m_trackPoints;
};

Track::Track() :
		m_impl(new TrackImpl())
{
}

Track::~Track()
{
}

void Track::addPoint(
		const CL_Pointf &p_point,
		float p_radius,
		float p_shift,
		int p_index
)
{
	G_ASSERT(p_index >= 0);

	TrackPoint trackPoint(p_point, p_radius, p_shift);

	if (p_index >= static_cast<signed>(m_impl->m_trackPoints.size())) {
		m_impl->m_trackPoints.push_back(trackPoint);
	} else {
		TrackImpl::TTrackPoints::iterator itor = m_impl->m_trackPoints.begin();
		itor += p_index;

		m_impl->m_trackPoints.insert(itor, trackPoint);
	}
}

TrackPoint &Track::getPoint(int p_index)
{
	return m_impl->m_trackPoints[p_index];
}

const TrackPoint &Track::getPoint(int p_index) const
{
	return m_impl->m_trackPoints[p_index];
}

int Track::getPointCount() const
{
	return m_impl->m_trackPoints.size();
}

void Track::clear()
{
	m_impl->m_trackPoints.clear();
}

} // namespace
