/*
 * ScoreTable.cpp
 *
 *  Created on: 2009-10-07
 *      Author: chudy
 */

#include "ScoreTable.h"

#include <assert.h>

#include "common.h"

ScoreTable::ScoreTable()
{

}

ScoreTable::~ScoreTable()
{

}

void ScoreTable::add(const RacePlayer *p_racePlayer, unsigned p_time)
{

	// push score in sorted order (lowest time first)
	std::list<Entry>::iterator itor(m_entries.begin());

	do {
		if (itor->m_time > p_time) {
			m_entries.insert(itor, Entry(p_racePlayer, p_time));
			break;
		}

		++itor;
	} while (itor != m_entries.end());
}

int ScoreTable::getEntriesCount() const
{
	return m_entries.size();
}

const ScoreTable::Entry &ScoreTable::getEntry(int index) const
{
	assert(index >= 0 && index < m_entries.size());

	int i = 0;
	foreach (const Entry &entry, m_entries) {
		if (i == index) {
			return entry;
		}

		++i;
	}
}

const RacePlayer* ScoreTable::getEntryPlayer(int index) const
{
	return getEntry(index).m_racePlayer;
}

unsigned ScoreTable::getEntryTime(int index) const
{
	return getEntry(index).m_time;
}
