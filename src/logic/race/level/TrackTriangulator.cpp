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

#include "TrackTriangulator.h"

#include <map>
#include <vector>

#include "common.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackPoint.h"
#include "logic/race/level/TrackSegment.h"

namespace Race {

/** Resolution of curves (in degrees). Lower is better, but slower. */
const float CURVE_RESOLUTION = 6.0f;


class TrackTriangulatorImpl
{
	public:

		enum Side {
			S_LEFT,
			S_RIGHT
		};

		typedef std::map<int, TrackSegment> TIntSegmentMap;


		TIntSegmentMap m_intSegMap;


		static int clamp(int p_index, int p_size);


		CL_Vec2f helper(const Track &p_track, int p_index, Side p_side) const;

		float interpolate(float p_pos, float p_prev, float p_next);

		float lengthTotal(const std::vector<CL_Pointf> &p_points);

		std::vector<TrackPoint> toTrackPoints(
				const std::vector<CL_Pointf> &p_points,
				float p_prevRadius, float p_nextRadius,
				float p_prevShift, float p_nextShift
		);
};

template <typename T>
class LoopVector : public std::vector<T>
{
    public:

        T &operator[] (int p_index) {
            return std::vector<T>::operator[](norm(p_index));
        }

        const T &operator[] (int p_index) const {
            return std::vector<T>::operator[](norm(p_index));
        }

    private:

        int norm(int p_index) const {
        	return TrackTriangulatorImpl::clamp(
        			p_index,
        			static_cast<signed> (std::vector<T>::size())
        	);
        }
};

TrackTriangulator::TrackTriangulator()
{
}

TrackTriangulator::~TrackTriangulator()
{
}

int TrackTriangulatorImpl::clamp(int p_index, int p_size)
{
	if (p_index >= 0 && p_index < p_size) {
		return p_index;
	}

	p_index = p_index % p_size;

	if (p_index < 0) {
		p_index += p_size;
	}

	return p_index;
}

CL_Vec2f TrackTriangulatorImpl::helper(const Track &p_track, int p_index, TrackTriangulatorImpl::Side p_side) const
{
	const int trackSize = p_track.getPointCount();
	const CL_Vec2f &currPoint = p_track.getPoint(p_index).getPosition();


	const int nextIndex = p_side == S_RIGHT ?
			clamp(p_index + 1, trackSize) :
			clamp(p_index - 1, trackSize);

	const int prevIndex = p_side == S_RIGHT ?
			clamp(p_index - 1, trackSize) :
			clamp(p_index + 1, trackSize);


	const CL_Vec2f &nextPoint = p_track.getPoint(nextIndex).getPosition();
	const CL_Vec2f &prevPoint = p_track.getPoint(prevIndex).getPosition();

	const float dist = currPoint.distance(nextPoint);

	const CL_Vec2f nextVec = nextPoint - currPoint;
	const CL_Vec2f prevVec = prevPoint - currPoint;


	const CL_Vec2f invPrevVec = prevVec * -1;
	CL_Vec2f middleNextVec = (nextVec + invPrevVec) / 2;

	middleNextVec.normalize();
	middleNextVec *= dist * 0.3f;

	return middleNextVec;
}

float TrackTriangulatorImpl::lengthTotal(const std::vector<CL_Pointf> &p_points)
{
	float len = 0.0f;
	const int size = static_cast<signed>(p_points.size());

	for (int i = 1; i < size; ++i) {

		const CL_Pointf &prev = p_points[i - 1];
		const CL_Pointf &next = p_points[i];

		const CL_Vec2f vec(next - prev);
		len += vec.length();
	}

	return len;
}

float TrackTriangulatorImpl::interpolate(float p_pos, float p_prev, float p_next)
{
	G_ASSERT(p_pos >= 0.0f && p_pos <= 1.0f);
	G_ASSERT(p_prev >= -1.0f && p_prev <= 1.0f);
	G_ASSERT(p_next >= -1.0f && p_next <= 1.0f);

	p_pos = (p_pos * p_pos) * (3.0f - (2.0f * p_pos));
	return (p_prev + ((p_next - p_prev) * p_pos));
}

std::vector<TrackPoint> TrackTriangulatorImpl::toTrackPoints(
		const std::vector<CL_Pointf> &p_points,
		float p_prevRadius, float p_nextRadius,
		float p_prevShift, float p_nextShift
)
{
	std::vector<TrackPoint> trackPoints;

	const float length = lengthTotal(p_points);
	const int size = static_cast<signed>(p_points.size());

	float pos = 0.0f;

	for (int i = 1; i < size; ++i) {
		const CL_Pointf &prev = p_points[i - 1];
		const CL_Pointf &next = p_points[i];

		const float normPos = pos / length;

		// add track point with proper radius and shift interpolation
		trackPoints.push_back(
				TrackPoint(
						prev,
						interpolate(normPos, p_prevRadius, p_nextRadius),
						interpolate(normPos, p_prevShift, p_nextShift)
				)
		);

		// update position
		const CL_Vec2f vec(next - prev);
		pos += vec.length();
	}

	return trackPoints;
}

void TrackTriangulator::triangulate(const Track &p_track, int p_segment)
{
	const int pointCount = p_track.getPointCount();
	G_ASSERT(p_segment >= 0 && p_segment <= pointCount);

	if (p_segment == -1) {

		for (int i = 0; i < pointCount; ++i) {
			triangulate(p_track, i);
		}

	} else {

		CL_BezierCurve curve;
		const int prevIdx = p_segment;
		const int nextIdx = m_impl->clamp(p_segment + 1, pointCount);

		const TrackPoint &prev = p_track.getPoint(prevIdx);
		const TrackPoint &next = p_track.getPoint(nextIdx);


		const CL_Vec2f prevHelper = m_impl->helper(
				p_track,
				prevIdx,
				TrackTriangulatorImpl::S_RIGHT
		);

		const CL_Vec2f nextHelper = m_impl->helper(
				p_track,
				nextIdx,
				TrackTriangulatorImpl::S_LEFT
		);


		curve.add_control_point(prev.getPosition());
		curve.add_control_point(prev.getPosition() + prevHelper);
		curve.add_control_point(next.getPosition() + nextHelper);
		curve.add_control_point(next.getPosition());

		std::vector<CL_Pointf> curvePoints = curve.generate_curve_points(CURVE_RESOLUTION);

		// track points knows thier radius and shift (interpolated values)
		std::vector<TrackPoint> trackPoints = m_impl->toTrackPoints(
				curvePoints,
				prev.getRadius(), next.getRadius(),
				prev.getShift(), next.getShift()
		);

		const int curveSize = static_cast<signed>(curvePoints.size());
		std::vector<CL_Pointf> triPoints;

		CL_Pointf lastLeftPoint, lastRightPoint;
		bool first = true;

		for (int i = 1; i < curveSize; ++i) {
			const TrackPoint &tprev = trackPoints[i - 1];
			const TrackPoint &tnext = trackPoints[i];

			const CL_Pointf prevPoint = tprev.getPosition();
			const CL_Pointf nextPoint = tnext.getPosition();

			CL_Vec2f tvec = nextPoint - prevPoint;
			tvec.normalize();
			tvec *= tprev.getRadius();

			// calculate left and right wing
			CL_Vec2f leftVec(tvec.y, -tvec.x); // due to inverted Y the left side is actually a right side
			CL_Vec2f rightVec(-leftVec.x, -leftVec.y);

			leftVec += leftVec * (tprev.getShift() * -1);
			rightVec += rightVec * tprev.getShift();

			if (!first) {

				const CL_Pointf leftPoint = prevPoint + leftVec;
				const CL_Pointf rightPoint = prevPoint + rightVec;

				// got 4 points, can make two triangles
				triPoints.push_back(lastLeftPoint);
				triPoints.push_back(lastRightPoint);
				triPoints.push_back(rightPoint);

				triPoints.push_back(lastLeftPoint);
				triPoints.push_back(rightPoint);
				triPoints.push_back(leftPoint);

			} else {
				first = false;
			}

			lastLeftPoint = prevPoint + leftVec;
			lastRightPoint = prevPoint + rightVec;
		}

		// update triangle points in map
		m_impl->m_intSegMap[p_segment] = triPoints;

	}

}

} // namespace
