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

#include "Progress.h"

#include <map>

#include "common.h"
#include "logic/race/Car.h"
#include "logic/race/level/Checkpoint.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackTriangulator.h"
#include "logic/race/level/TrackSegment.h"

namespace Race
{

struct ProgressInfo
{
	// current lap
	int m_lapNum;

	// farthest accepted checkpoint on track
	Checkpoint m_cp;
};

class ProgressImpl
{
	public:

		typedef std::map<const Car*, ProgressInfo> TCarProgressMap;
		typedef std::pair<const Car*, ProgressInfo> TCarProgressPair;

		typedef std::vector<Checkpoint> TCheckpointList;
		typedef std::vector<int> TCheckpointDistances;


		// fields

		/** Cars to progress mapping */
		TCarProgressMap m_cars;

		/** All checkpoints ordered from first to last */
		TCheckpointList m_chkpts;

		/**
		 * Checkpoint distances. 0 is distance between 0 and 1, last between
		 * last checkpoint and 0. Distances held in world units.
		 */
		TCheckpointDistances m_dists;

		const Level &m_level;

		/** Initialized flag */
		bool m_initd;


		// methods

		ProgressImpl(const Level &p_level) :
			m_level(p_level),
			m_initd(false)
		{ /* empty */ }

		~ProgressImpl()
		{
			destroy();
		}


		const Checkpoint &closestCheckpoint(const CL_Pointf &p_pos) const;

		void destroy();

		float distPow(const CL_Pointf &p_a, const CL_Pointf &p_b) const
		{
			const float a = p_b.x - p_a.x;
			const float b = p_b.y - p_a.y;
			return a * a + b * b;
		}

		int distance(const Checkpoint &p_from, const Checkpoint &p_to) const;
};

Progress::Progress(const Level &p_level) :
	m_impl(new ProgressImpl(p_level))
{
	// empty
}

Progress::~Progress()
{
	// empty
}

void Progress::addCar(const Car &p_car)
{
	G_ASSERT(m_impl->m_initd);

	const ProgressInfo pinfo = {0, m_impl->m_chkpts[0]};
	m_impl->m_cars[&p_car] = pinfo;
}

void Progress::initialize()
{
	if (m_impl->m_initd) {
		return;
	}

	G_ASSERT(m_impl->m_level.isLoaded() && "level must be loaded first");

	// minimal checkpoint distance
	static const int MIN_DISTANCE = 50;


	// load checkpoints
	const Track &track = m_impl->m_level.getTrack();
	const TrackTriangulator &triang = m_impl->m_level.getTrackTriangulator();
	const int segCount = track.getPointCount();

	int chkPtIdx = 0;
	CL_Pointf prevPos;
	float dist;

	for (int i = 0; i < segCount; ++i) {
		const TrackSegment &seg = triang.getSegment(i);

		// from mid points of track
		const std::vector<CL_Pointf> &midPts = seg.getMidPoints();
		const int midCount = midPts.size();

		for (int j = 0; j < midCount; ++j) {

			if (chkPtIdx != 0) {

				dist = prevPos.distance(midPts[j]);

				// skip point if distance is to low
				if (dist < MIN_DISTANCE) {
					continue;
				}

				// save the distance
				m_impl->m_dists.push_back(static_cast<int>(ceil(dist)));
			}

			m_impl->m_chkpts.push_back(Checkpoint(chkPtIdx++, midPts[j]));
			prevPos = m_impl->m_chkpts.back().getPosition();
		}
	}

	G_ASSERT(m_impl->m_chkpts.size() > 0 && "no checkpoints loaded");

	// insert distance betwwen first and last
	dist = m_impl->m_chkpts.back().getPosition().distance(
			m_impl->m_chkpts.front().getPosition()
	);

	m_impl->m_dists.push_back(dist);

	// mark initialized
	m_impl->m_initd = true;

}

void Progress::destroy()
{
	m_impl->destroy();
}

void ProgressImpl::destroy()
{
	if (!m_initd) {
		return;
	}

	m_chkpts.clear();
	m_dists.clear();
	m_cars.clear();

	m_initd = false;
}

void Progress::removeCar(const Car &p_car)
{
	G_ASSERT(m_impl->m_initd);

	ProgressImpl::TCarProgressMap::iterator itor =
			m_impl->m_cars.find(&p_car);

	G_ASSERT(itor != m_impl->m_cars.end());

	m_impl->m_cars.erase(itor);
}

void Progress::update()
{
	static const int FAR_LIMIT = 400;

	// localize closest checkpoint for each car
	ProgressImpl::TCarProgressPair pair;

	foreach (pair, m_impl->m_cars) {
		const Checkpoint &nextCp =
				m_impl->closestCheckpoint(pair.first->getPosition());

		ProgressInfo &info = m_impl->m_cars[pair.first];

		if (nextCp.getIndex() == info.m_cp.getIndex() + 1) {
			// accept if this is next checkpoint
			info.m_cp = nextCp;
		} else {
			// if this is much further then accept only if its position
			// is close enough
			if (m_impl->distance(info.m_cp, nextCp) <= FAR_LIMIT) {

				if (nextCp.getIndex() < info.m_cp.getIndex()) {
					// this means a new lap
					++info.m_lapNum;
				}

				info.m_cp = nextCp;
			}
		}

	}
}

const Checkpoint &ProgressImpl::closestCheckpoint(const CL_Pointf &p_pos) const
{
	G_ASSERT(m_chkpts.size() > 0);

	float dist, ndist;
	const Checkpoint *tcp;
	bool first = true;

	foreach (const Checkpoint &cp, m_chkpts) {

		ndist = distPow(cp.getPosition(), p_pos);

		if (!first) {
			if (ndist < dist) {
				dist = ndist;
				tcp = &cp;
			}
		} else {
			dist = ndist;
			tcp = &cp;
			first = false;
		}
	}

	return *tcp;
}

int ProgressImpl::distance(
		const Checkpoint &p_from,
		const Checkpoint &p_to
) const
{
	const int fromIdx = p_from.getIndex();
	const int toIdx = p_to.getIndex();

	int sum = 0;
	const int distsCount = m_dists.size();

	for (int i = fromIdx; i != toIdx;) {

		sum += m_dists[i];

		if (i + 1 < distsCount) {
			++i;
		} else {
			i = 0;
		}
	}

	return sum;
}

int Progress::getLapNumber(const Car &p_car) const
{
	G_ASSERT(m_impl->m_initd);

	ProgressImpl::TCarProgressMap::const_iterator itor =
			m_impl->m_cars.find(&p_car);

	G_ASSERT(itor != m_impl->m_cars.end());

	return itor->second.m_lapNum;
}

const Checkpoint &Progress::getCheckpoint(const Car &p_car) const
{
	G_ASSERT(m_impl->m_initd);

	ProgressImpl::TCarProgressMap::const_iterator itor =
			m_impl->m_cars.find(&p_car);

	G_ASSERT(itor != m_impl->m_cars.end());


	return itor->second.m_cp;
}

const Checkpoint &Progress::getCheckpoint(int p_idx) const
{
	G_ASSERT(m_impl->m_initd);
	G_ASSERT(p_idx >= 0 && p_idx < static_cast<signed>(m_impl->m_chkpts.size()));

	return m_impl->m_chkpts[p_idx];
}

int Progress::getCheckpointCount() const
{
	G_ASSERT(m_impl->m_initd);
	return static_cast<signed>(m_impl->m_chkpts.size());
}

} // namespace
