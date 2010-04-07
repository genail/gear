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

#include "LocalRanking.h"

#include <ClanLib/database.h>
#include <ClanLib/sqlite.h>

#include "common/gassert.h"
#include "common/loglevels.h"
#include "ranking/RankingEntry.h"


const int CURRENT_SCHEMA_VERSION = 1;


const CL_String META_TABLE_DEF = CL_String()
		+ "CREATE TABLE meta ( "
		+ "id INTEGER PRIMARY KEY ASC"
		+ ", name TEXT"
		+ ", value INTEGER"
		+ " )";

const CL_String RANKING_TABLE_DEF = CL_String()
		+ "CREATE TABLE ranking ( "
		+ "id INTEGER PRIMARY KEY ASC AUTOINCREMENT"
		+ ", uid TEXT"
		+ ", name TEXT"
		+ ", time INTEGER"
		+ " )";

const CL_String RANKING_INDEX_UID = CL_String()
		+ "CREATE UNIQUE INDEX ranking_uid_index "
		+ "ON ranking (uid)";

const CL_String RANKING_INDEX_TIME = CL_String()
		+ "CREATE UNIQUE INDEX ranking_time_index "
		+ "ON ranking (time)";

class LocalRankingImpl
{
	public:

		CL_SqliteConnection m_connection;


		LocalRankingImpl(const CL_String &p_dbFile);

		void prepareSchema();

		int tryGettingVersion();

		void buildSchema();

		void buildMetaTable();

		void buildRankingTable();

		void createIndexes();

		void insertSchemaVersion();


		void insertEntry(const RankingEntry &p_entry);

		void moveEntry(const RankingEntry &p_newEntry, int p_oldIndex);

		int getEntryTime(int p_index);

		RankingEntry getEntryAtIndex(int p_index);
};

LocalRanking::LocalRanking(const CL_String &p_dbFile) :
		m_impl(new LocalRankingImpl(p_dbFile))
{
	// empty
}

LocalRankingImpl::LocalRankingImpl(const CL_String &p_dbFile) :
		m_connection(p_dbFile)
{
	prepareSchema();
}

void LocalRankingImpl::prepareSchema()
{
	int version = 0;

	try {
		version = tryGettingVersion();
	} catch (CL_Exception &e) {
		buildSchema();
	}

	if (version != 0) {
		if (version != CURRENT_SCHEMA_VERSION) {
			throw CL_Exception("ranking version mismatch");
		}
	}
}

int LocalRankingImpl::tryGettingVersion()
{
	CL_DBCommand cmd = m_connection.create_command("SELECT value FROM meta WHERE name=?1");
	cmd.set_input_parameter_string(1, "version");
	return m_connection.execute_scalar_int(cmd);
}

void LocalRankingImpl::buildSchema()
{
	buildMetaTable();
	buildRankingTable();
	insertSchemaVersion();
}

void LocalRankingImpl::buildMetaTable()
{
	CL_DBCommand cmd = m_connection.create_command(META_TABLE_DEF);
	m_connection.execute_non_query(cmd);
}

void LocalRankingImpl::buildRankingTable()
{
	CL_DBCommand cmd = m_connection.create_command(RANKING_TABLE_DEF);
	m_connection.execute_non_query(cmd);
}

void LocalRankingImpl::createIndexes()
{
	CL_DBCommand cmd;

	cmd = m_connection.create_command(RANKING_INDEX_UID);
	m_connection.execute_non_query(cmd);

	cmd = m_connection.create_command(RANKING_INDEX_TIME);
	m_connection.execute_non_query(cmd);
}

void LocalRankingImpl::insertSchemaVersion()
{
	CL_DBCommand cmd =
			m_connection.create_command("INSERT INTO meta (name, value) VALUES (?1, ?2)");

	cmd.set_input_parameter_string(1, "version");
	cmd.set_input_parameter_int(2, CURRENT_SCHEMA_VERSION);

	m_connection.execute_non_query(cmd);
}

LocalRanking::~LocalRanking()
{
	// empty
}

void LocalRanking::advanceEntry(const RankingEntry &p_entry)
{
	const int index = findEntryIndex(p_entry.uid);

	if (index == -1) {
		m_impl->insertEntry(p_entry);
	} else {
		m_impl->moveEntry(p_entry, index);
	}
}

void LocalRankingImpl::insertEntry(const RankingEntry &p_entry)
{
	static const CL_String INSERT_STATEMENT = CL_String("")
			+ "INSERT INTO ranking "
			+ "( uid, name, time ) "
			+ "VALUES "
			+ "(?1, ?2, ?3)";

	CL_DBCommand cmd = m_connection.create_command(INSERT_STATEMENT);

	cmd.set_input_parameter_string(1, p_entry.uid);
	cmd.set_input_parameter_string(2, p_entry.name);
	cmd.set_input_parameter_int(3, p_entry.timeMs);

	m_connection.execute_non_query(cmd);
}

void LocalRankingImpl::moveEntry(const RankingEntry &p_newEntry, int p_oldIndex)
{
	static const CL_String UPDATE_STATEMENT = CL_String("")
			+ "UPDATE ranking"
			+ " SET time=?1, name=?2"
			+ " WHERE id=?3";

	CL_DBCommand cmd = m_connection.create_command(UPDATE_STATEMENT);

	cmd.set_input_parameter_int(1, p_newEntry.timeMs);
	cmd.set_input_parameter_string(2, p_newEntry.name);
	cmd.set_input_parameter_int(3, p_oldIndex);

	m_connection.execute_non_query(cmd);
}

int LocalRanking::findEntryIndex(const CL_String &p_uid)
{
	static const CL_String FIND_STATEMENT = CL_String("")
			+ "SELECT id FROM ranking "
			+ "WHERE uid=?1";

	CL_DBCommand cmd = m_impl->m_connection.create_command(FIND_STATEMENT);
	cmd.set_input_parameter_string(1, p_uid);

	try {
		return m_impl->m_connection.execute_scalar_int(cmd);
	} catch (...) {
		return -1;
	}
}

RankingEntry LocalRanking::getEntryAtPosition(int p_position)
{
	static const CL_String SELECT_STATEMENT = CL_String("")
			+ "SELECT uid, name, time FROM ranking "
			+ "ORDER BY time "
			+ "LIMIT 1 OFFSET ?1";

	G_ASSERT(p_position >= 1);

	RankingEntry rankingEntry;

	CL_DBCommand cmd = m_impl->m_connection.create_command(SELECT_STATEMENT);
	cmd.set_input_parameter_int(1, p_position - 1);

	CL_DBReader reader = m_impl->m_connection.execute_reader(cmd);
	if (reader.retrieve_row()) {
		rankingEntry.uid = reader.get_column_string(0);
		rankingEntry.name = reader.get_column_string(1);
		rankingEntry.timeMs = reader.get_column_int(2);
	} else {
		throw CL_Exception("failed to retrieve index");
	}

	reader.close();
	return rankingEntry;
}

RankingEntry LocalRanking::getEntryAtIndex(int p_index)
{
	return m_impl->getEntryAtIndex(p_index);
}

RankingEntry LocalRankingImpl::getEntryAtIndex(int p_index)
{
	static const CL_String SELECT_STATEMENT = CL_String("")
			+ "SELECT uid, name, time FROM ranking "
			+ "WHERE id=?1";

	CL_DBCommand cmd = m_connection.create_command(SELECT_STATEMENT);
	cmd.set_input_parameter_int(1, p_index);

	RankingEntry rankingEntry;
	CL_DBReader reader = m_connection.execute_reader(cmd);
	if (reader.retrieve_row()) {
		rankingEntry.uid = reader.get_column_string(0);
		rankingEntry.name = reader.get_column_string(1);
		rankingEntry.timeMs = reader.get_column_int(2);
	} else {
		throw CL_Exception("failed to retrieve index");
	}

	reader.close();
	return rankingEntry;
}

int LocalRanking::getEntryRankingPosition(int p_index)
{
	const int time = m_impl->getEntryTime(p_index);

	static const CL_String SELECT_STATEMENT = CL_String("")
			+ "SELECT count(*) FROM ranking "
			+ "WHERE time <= ?1";

	CL_DBCommand cmd = m_impl->m_connection.create_command(SELECT_STATEMENT);
	cmd.set_input_parameter_int(1, time);

	return m_impl->m_connection.execute_scalar_int(cmd);
}

int LocalRankingImpl::getEntryTime(int p_index)
{
	const RankingEntry entry = getEntryAtIndex(p_index);
	return entry.timeMs;
}

int LocalRanking::getEntryCount()
{
	static const CL_String COUNT_STATEMENT = "SELECT count(*) FROM ranking";
	CL_DBCommand cmd = m_impl->m_connection.create_command(COUNT_STATEMENT);

	return m_impl->m_connection.execute_scalar_int(cmd);
}
