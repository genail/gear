/*
 * ScoreTable.h
 *
 *  Created on: 2009-10-07
 *      Author: chudy
 */

#ifndef SCORETABLE_H_
#define SCORETABLE_H_

#include <list>

#include "race/RacePlayer.h"

class ScoreTable {

		struct Entry {
				const RacePlayer *m_racePlayer;
				const unsigned m_time;

				Entry(const RacePlayer *p_racePlayer, unsigned p_time) :
					m_racePlayer(p_racePlayer),
					m_time(p_time)
					{}
		};

	public:

		ScoreTable();

		virtual ~ScoreTable();

		void add(const RacePlayer *p_racePlayer, unsigned p_time);

		void clear();

		int getEntriesCount() const;

		const RacePlayer* getEntryPlayer(size_t index) const;

		unsigned getEntryTime(size_t index) const;

	private:

		std::list<Entry> m_entries;

		const Entry &getEntry(size_t index) const;


};

#endif /* SCORETABLE_H_ */
