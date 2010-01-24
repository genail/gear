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

#include "ScoreTable.h"

#include "common.h"
#include "common/Player.h"

ScoreTable::ScoreTable()
{

}

ScoreTable::~ScoreTable()
{

}

void ScoreTable::add(const Player *p_player, unsigned p_time)
{

	// push score in sorted order (lowest time first)
	std::list<Entry>::iterator itor(m_entries.begin());

	do {
		if (itor->m_time > p_time) {
			m_entries.insert(itor, Entry(p_player, p_time));
			return;
		}

		++itor;
	} while (itor != m_entries.end());

	m_entries.push_back(Entry(p_player, p_time));
}

int ScoreTable::getEntriesCount() const
{
	return m_entries.size();
}

const ScoreTable::Entry &ScoreTable::getEntry(size_t index) const
{
	G_ASSERT(index < m_entries.size());

	size_t i = 0;
	foreach (const Entry &entry, m_entries) {
		if (i == index) {
			return entry;
		}

		++i;
	}

	assert(0 && "not supposed to be here");
	return *m_entries.begin();
}

const Player* ScoreTable::getEntryPlayer(size_t index) const
{
	return getEntry(index).m_player;
}

unsigned ScoreTable::getEntryTime(size_t index) const
{
	return getEntry(index).m_time;
}

void ScoreTable::clear()
{
	m_entries.clear();
}
