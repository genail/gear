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
			return;
		}

		++itor;
	} while (itor != m_entries.end());

	m_entries.push_back(Entry(p_racePlayer, p_time));
}

int ScoreTable::getEntriesCount() const
{
	return m_entries.size();
}

const ScoreTable::Entry &ScoreTable::getEntry(size_t index) const
{
	assert(index >= 0 && index < m_entries.size());

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

const RacePlayer* ScoreTable::getEntryPlayer(size_t index) const
{
	return getEntry(index).m_racePlayer;
}

unsigned ScoreTable::getEntryTime(size_t index) const
{
	return getEntry(index).m_time;
}

void ScoreTable::clear()
{
	m_entries.clear();
}
