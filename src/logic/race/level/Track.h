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

#include <limits>
#include <vector>
#include <ClanLib/core.h>

namespace Race {

class Checkpoint;
class TrackPoint;

class TrackImpl;

class Track {

	public:

		Track();

		virtual ~Track();


		void addPoint(
				const CL_Pointf &p_point,
				float p_radius,
				float p_shift,
				int p_index = std::numeric_limits<int>::max()
		);

		const TrackPoint &getPoint(int p_index) const;

		int getPointCount() const;


//		void addCheckpointAtPosition(const CL_Pointf &p_position);

		void clear();

//		const Checkpoint *check(const CL_Pointf &p_position, const Checkpoint *p_lastCheckPoint, bool *p_movingForward, bool *p_newLap) const;

	private:

		CL_SharedPtr<TrackImpl> m_impl;

//		/** Registered checkpoints */
//		typedef std::vector<Checkpoint*> TCheckpointVector;
//		TCheckpointVector m_checkpoints;
//
//		void getPrevAndNext(const Checkpoint *p_current, Checkpoint **p_before, Checkpoint **p_after) const;

};

}
