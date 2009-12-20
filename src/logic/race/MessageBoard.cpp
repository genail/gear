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

#include "MessageBoard.h"

#include <limits>
#include <assert.h>

#include "common.h"

namespace Race {

/** What is the most number of messages that can be kept */
const unsigned UPPER_LIMIT = 100;

/** To what count value message board will be cleaned */
const unsigned LOWER_LIMIT = 30;

struct Comparator {
	bool operator() (unsigned s1, unsigned s2) const
	{
		return s1 > s2; // reverse result will sort in reverse order
	}
};

MessageBoard::MessageBoard() :
	m_id(0)
{
}

MessageBoard::~MessageBoard()
{
}

const CL_String &MessageBoard::getMessageString(int p_id)
{
	assert(m_messageMap.find(p_id) != m_messageMap.end());
	return m_messageMap[p_id].m_msg;
}

unsigned MessageBoard::getMessageCreationTime(int p_id)
{
	assert(m_messageMap.find(p_id) != m_messageMap.end());
	return m_messageMap[p_id].m_creationTime;
}

std::vector<int> MessageBoard::getMessageIdsYoungerThat(unsigned p_ageMs, unsigned p_limit)
{
	std::vector<int> result;
	const unsigned now = CL_System::get_time();

	TMessageMapPair pair;
	foreach (pair, m_messageMap) {
		if (now - pair.second.m_creationTime < p_ageMs) {
			result.push_back(pair.first);
		}
	}

	return result;
}

int MessageBoard::addMessage(const CL_String &p_message)
{
	const int id = nextId();

	Message msg = { p_message, CL_System::get_time() };
	m_messageMap[id] = msg;

	checkLimits();

	return id;
}

int MessageBoard::nextId()
{
	bool overflow = false;

	do {
		if (m_id != std::numeric_limits<int>::max()) {
			++m_id;

			if (!overflow) {
				overflow = true;
			} else {
				assert(0 && "Lack of id's");
			}
		} else {
			m_id = 1;
		}

	} while (m_messageMap.find(m_id) == m_messageMap.end());

	return m_id;
}

void MessageBoard::checkLimits()
{
	if (m_messageMap.size() > UPPER_LIMIT) {
		// remove oldest messages to reach the LOWER_LIMIT

		// create a creation time => id map to find oldest ones
		typedef std::multimap<unsigned, int, Comparator> TTimeIdMap;
		typedef std::pair<unsigned, int> TTimeIdPair;
		typedef TTimeIdMap::iterator TTimeIdMapItor;

		TTimeIdMap timeIdMap;

		TMessageMapPair pair;
		foreach (pair, m_messageMap) {
			timeIdMap.insert(std::pair<unsigned, int>(pair.second.m_creationTime, pair.first));
		}

		// kepp only the youngest
		unsigned i = 1;
		for (TTimeIdMapItor it = timeIdMap.begin(); it != timeIdMap.end(); ++it, ++i) {
			if (i > LOWER_LIMIT) {
				m_messageMap.erase(it->second);
			}
		}
	}
}


}
