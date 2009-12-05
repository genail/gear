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

#include <vector>
#include <ClanLib/core.h>

namespace Race {

class Checkpoint;

class Track {

	public:

		Track();

		virtual ~Track();


		void addCheckpointAtPosition(const CL_Pointf &p_position);

		unsigned getCheckpointCount() const;

		const Checkpoint *getCheckpoint(unsigned p_index) const;

		const Checkpoint *getFirst() const;

		void clear();

		/** Closes the track.  */
		void close();


		const Checkpoint *check(const CL_Pointf &p_position, const Checkpoint *p_lastCheckPoint, bool *p_movingForward, bool *p_newLap);

	private:

		/** Registered checkpoints */
		typedef std::vector<Checkpoint*> TCheckpointVector;
		TCheckpointVector m_checkpoints;

		/** Closed track cannot get new checkpoints */
		bool m_closed;

		void getPrevAndNext(const Checkpoint *p_current, Checkpoint **p_before, Checkpoint **p_after);

};

}

