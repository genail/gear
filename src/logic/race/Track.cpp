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
#include "Checkpoint.h"

namespace Race {

Track::Track() :
	m_closed(false)
{
}

Track::~Track()
{
	clear();
}

void Track::addCheckpointAtPosition(const CL_Pointf &p_position)
{
	assert(!m_closed);

	const int id = m_checkpoints.size() + 1;
	m_checkpoints.push_back(new Checkpoint(id, p_position));
}

unsigned Track::getCheckpointCount() const
{
	return m_checkpoints.size();
}

const Checkpoint *Track::getCheckpoint(unsigned p_index) const
{
	assert(p_index < m_checkpoints.size());
	assert(m_closed);

	return m_checkpoints[p_index];
}

const Checkpoint *Track::getFirst() const
{
	assert(m_checkpoints.size() > 0);
	assert(m_closed);

	return m_checkpoints[0];
}

void Track::clear()
{
	foreach(Checkpoint *checkpoint, m_checkpoints) {
		delete checkpoint;
	}

	m_checkpoints.clear();
	m_closed = false;
}

const Checkpoint *Track::check(const CL_Pointf &p_position, const Checkpoint *p_lastCheckPoint, bool *p_movingForward, bool *p_newLap) const
{
	assert(m_closed);

	// get before and after checkpoint
	Checkpoint *prev, *next;
	getPrevAndNext(p_lastCheckPoint, &prev, &next);

	// calculate distances
	const float prevDistance = p_position.distance(prev->getPosition());
	const float currentDistance = p_position.distance(p_lastCheckPoint->getPosition());
	const float nextDistance = p_position.distance(next->getPosition());

	*p_newLap = false;
	*p_movingForward = true;
	Checkpoint *result;

	if (nextDistance < currentDistance && nextDistance < prevDistance) {
		// moving forward

		if (next == m_checkpoints[0]) {
			// made a lap
			*p_newLap = true;
		}

		result = next;
	} else if (prevDistance < currentDistance && prevDistance < nextDistance) {
		// moving backwards
		*p_movingForward = false;

		result = prev;
	} else {
		// moving forward without a change
		result = const_cast<Checkpoint*>(p_lastCheckPoint);
	}

	return result;
}

void Track::getPrevAndNext(const Checkpoint *p_current, Checkpoint **p_prev, Checkpoint **p_next) const
{
	assert(m_checkpoints.size() >= 3);
	assert(m_closed);

	Checkpoint *current;
	Checkpoint *prev = NULL;
	Checkpoint *next;
	bool found = false;

	TCheckpointVector::const_iterator itor = m_checkpoints.begin();

	for (;itor != m_checkpoints.end(); ++itor) {
		current = *itor;

		if (current == p_current) {

			// if current is first, then previous will be the last
			if (prev == NULL) {
				prev = *(--m_checkpoints.end());
			}

			// get after
			++itor;

			// if next is the end, then it will be the first
			if (itor != m_checkpoints.end()) {
				next = *itor;
			} else {
				next = *m_checkpoints.begin();
			}

			found = true;
			break;
		} else {
			prev = current;
		}
	}

	assert(found);

	// assign pointers
	*p_prev = prev;
	*p_next = next;
}

void Track::close()
{
	// set progress to every checkpoint
	const unsigned size = m_checkpoints.size();

	for (unsigned i = 0; i < size - 1; ++i) {
		m_checkpoints[i]->m_progress = i / (float) (size - 1);
	}

	m_checkpoints[size - 1]->m_progress = 1.0f;

	// set track closed
	m_closed = true;
}

} // namespace
