/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <boost/test/unit_test.hpp>

#include "ranking/RankingEntry.h"
#include "ranking/LocalRanking.h"

/*
 * Minimal testing facility:
 *
 * BOOST_CHECK( predicate )
 * BOOST_REQUIRE( predicate )
 * BOOST_ERROR( message )
 * BOOST_FAIL( message )
 *
 * Test tools:
 * http://www.boost.org/doc/libs/1_34_0/libs/test/doc/components/test_tools/index.html
 */

BOOST_AUTO_TEST_SUITE(LocalRankingTest)

const char *TEMPLATE = "gear_ranking_db_XXXXXX";
LocalRanking *ranking;

void init() {
	char *temp = new char[strlen(TEMPLATE)];
	strncpy(temp, TEMPLATE, strlen(TEMPLATE) + 1);

	mkstemp(temp);

	static LocalRanking r(temp);
	ranking = &r;

	delete[] temp;
}

BOOST_AUTO_TEST_CASE(ListEntries)
{
	init();

	BOOST_CHECK(ranking->getEntryCount() == 0);

	RankingEntry entry1, entry2;
	entry1.uid = "uid_1";
	entry1.name = "name_1";
	entry1.timeMs = 10;

	entry2.uid = "uid_2";
	entry2.name = "name_2";
	entry2.timeMs = 5;

	ranking->advanceEntry(entry1);
	ranking->advanceEntry(entry2);

	BOOST_CHECK(ranking->getEntryCount() == 2);

	RankingEntry dbEntry1, dbEntry2;
	dbEntry1 = ranking->getEntryAtPosition(1);
	dbEntry2 = ranking->getEntryAtPosition(2);

	BOOST_CHECK(dbEntry1.name == entry2.name);
	BOOST_CHECK(dbEntry2.name == entry1.name);

	BOOST_CHECK(dbEntry1.uid == entry2.uid);
	BOOST_CHECK(dbEntry2.uid == entry1.uid);

	BOOST_CHECK_EQUAL(dbEntry1.timeMs, entry2.timeMs);
	BOOST_CHECK_EQUAL(dbEntry2.timeMs, entry1.timeMs);
}

BOOST_AUTO_TEST_CASE(Advance)
{
	init();

	RankingEntry entry1, entry2;
	entry1.uid = "uid_1";
	entry1.name = "name_1";
	entry1.timeMs = 10;

	entry2.uid = "uid_2";
	entry2.name = "name_2";
	entry2.timeMs = 5;

	ranking->advanceEntry(entry1);
	ranking->advanceEntry(entry2);

	entry1.timeMs = 3;

	ranking->advanceEntry(entry1);


	BOOST_CHECK(ranking->getEntryCount() == 2);

	RankingEntry dbEntry1, dbEntry2;
	dbEntry1 = ranking->getEntryAtPosition(1);
	dbEntry2 = ranking->getEntryAtPosition(2);

	BOOST_CHECK(dbEntry1.name == entry1.name);
	BOOST_CHECK(dbEntry2.name == entry2.name);

	BOOST_CHECK(dbEntry1.uid == entry1.uid);
	BOOST_CHECK(dbEntry2.uid == entry2.uid);

	BOOST_CHECK_EQUAL(dbEntry1.timeMs, entry1.timeMs);
	BOOST_CHECK_EQUAL(dbEntry2.timeMs, entry2.timeMs);
}

BOOST_AUTO_TEST_CASE(Position)
{
	init();

	RankingEntry entry1, entry2;
	entry1.uid = "uid_1";
	entry1.name = "name_1";
	entry1.timeMs = 10;

	entry2.uid = "uid_2";
	entry2.name = "name_2";
	entry2.timeMs = 5;

	ranking->advanceEntry(entry1);
	ranking->advanceEntry(entry2);

	BOOST_CHECK_EQUAL(1, ranking->getEntryRankingPosition(ranking->findEntryIndex(entry2.uid)));
	BOOST_CHECK_EQUAL(2, ranking->getEntryRankingPosition(ranking->findEntryIndex(entry1.uid)));
}

BOOST_AUTO_TEST_SUITE_END()
