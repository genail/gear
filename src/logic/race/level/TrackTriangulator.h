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

#pragma once

#include <ClanLib/core.h>

namespace Race
{

class Track;
class TrackSegment;

class TrackTriangulatorImpl;

/**
 * Track triangulator.
 * <p>
 * This class takes track object and generates triangle representation of
 * track. After calculation the data is stored in triangulator memory.
 * Stored data can be updated by passing modified track and segment index
 * to triangulate() method.
 */
class TrackTriangulator
{

	public:

		TrackTriangulator();

		virtual ~TrackTriangulator();

		/** Removes all triangulation data. */
		void clear();

		/** @return First left side point from selected segment */
		const CL_Pointf &getFirstLeftPoint(int p_segIndex) const;

		/** @return First right side point from selected segment */
		const CL_Pointf &getFirstRightPoint(int p_segIndex) const;

		/** @return Last left side point from selected segment */
		const CL_Pointf &getLastLeftPoint(int p_segIndex) const;

		/** @return Last right side point from selected segment */
		const CL_Pointf &getLastRightPoint(int p_segIndex) const;

		/**
		 * Provides guide vector for selected point. It is available only
		 * after segment triangulation, so don't try to retrieve it earlier.
		 * <p>
		 * It always points to forward direction of the track.
		 *
		 * @return Guide vector for selected point.
		 */
		const CL_Vec2f &getGuide(int p_pointIndex) const;

		const TrackSegment &getSegment(int p_segIndex) const;

		/**
		 * Triangulates whole track (if p_segment is -1) or
		 * single segment by replacing old triangulate data
		 * with new one.
		 * <p>
		 * When track size has changed is strongly recommended to triangulate
		 * whole track again.
		 *
		 * @param p_track The track.
		 * @param p_segment Segment index.
		 */
		void triangulate(const Track &p_track, int p_segment = -1);


	private:

		CL_SharedPtr<TrackTriangulatorImpl> m_impl;
};

}

