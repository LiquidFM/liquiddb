/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2012 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */
#include "ldb_Database.h"
#include "ldb_conversions_p.h"


namespace LiquidDb {

Database::Database() :
	m_db(NULL),
    m_transaction(0),
	m_error(SQLITE_OK)
{}

#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
Database::Database(Database &&other) :
	m_db(other.m_db),
    m_transaction(0),
	m_error(other.m_error)
{
	other.m_db = NULL;
	other.m_error = SQLITE_OK;
}

Database &Database::operator=(Database &&other)
{
	sqlite3_close(m_db);
	m_db = other.m_db;
    m_transaction = other.m_transaction;
	m_error = other.m_error;
	other.m_db = NULL;
	other.m_error = SQLITE_OK;

	return *this;
}
#endif

Database::~Database()
{
	sqlite3_close(m_db);
}

bool Database::open(const ::EFC::String &filename)
{
	ASSERT(m_db == NULL);
	return sqlite3_open(filename.c_str(), &m_db) == SQLITE_OK;
}

bool Database::exsitst(const Table &table) const
{
	ASSERT(table.name() != NULL);
	char buffer[BufferSize];
	bool res = false;

	int len = snprintf(buffer, BufferSize, "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table.name());

	if (LIKELY(len > 0))
	{
		sqlite3_stmt *statement;

		if (sqlite3_prepare_v2(m_db, buffer, len, &statement, NULL) == SQLITE_OK)
		{
			res = (sqlite3_step(statement) == SQLITE_ROW);
			m_error = sqlite3_finalize(statement);
		}
	}

	return res;
}

bool Database::create(const Table &table)
{
	ASSERT(table.name() != NULL);
	ASSERT(table.primaryKeyColumn() != NULL);
	ASSERT(table.primaryKeyColumn()->type == Table::Column::BigInt);
    char buffer[BufferSize];

    m_error = SQLITE_OK;
    int len = snprintf(buffer, BufferSize, "CREATE TABLE %s (%s INTEGER PRIMARY KEY ASC AUTOINCREMENT", table.name(), table.primaryKeyColumn()->name);

    if (LIKELY(len > 0))
    {
        sqlite3_stmt *statement;
        int size = len;

        for (int i = 0, count = table.columnsCount(); i < count; ++i)
        {
            if (table.column(i) == table.primaryKeyColumn())
                continue;

            if (UNLIKELY(BufferSize <= size))
            {
                m_error = SQLITE_NOMEM;
                return false;
            }

            len = snprintf(buffer + size, (BufferSize) - size, ", %s %s", table.column(i)->name, typeToString(table.column(i)->type));

            if (LIKELY(len > 0))
                size += len;
            else
            {
                m_error = SQLITE_NOMEM;
                return false;
            }
        }

        len = snprintf(buffer + size, (BufferSize) - size, ")");

        if (LIKELY(len > 0))
        {
            size += len;

            if (sqlite3_prepare_v2(m_db, buffer, size, &statement, NULL) == SQLITE_OK)
            {
                sqlite3_step(statement);
                m_error = sqlite3_finalize(statement);

                if (m_error != SQLITE_OK)
                    return false;
            }
            else
            {
                m_error = sqlite3_errcode(m_db);
                return false;
            }
        }
        else
        {
            m_error = SQLITE_NOMEM;
            return false;
        }

        for (int i = 0, count = table.indexesCount(); i < count; ++i)
        {
            len = snprintf(buffer, (BufferSize), "CREATE INDEX %s ON %s(%s)", table.index(i)->name, table.name(), table.index(i)->column->name);

            if (LIKELY(len > 0))
            {
                if (sqlite3_prepare_v2(m_db, buffer, len, &statement, NULL) == SQLITE_OK)
                {
                    sqlite3_step(statement);
                    m_error = sqlite3_finalize(statement);

                    if (m_error != SQLITE_OK)
                        return false;
                }
                else
                {
                    m_error = sqlite3_errcode(m_db);
                    return false;
                }
            }
            else
            {
                m_error = SQLITE_NOMEM;
                return false;
            }
        }

        return true;
    }
    else
    {
        m_error = SQLITE_NOMEM;
    }

	return false;
}

bool Database::remove(const Table &table)
{
    ASSERT(m_db != NULL);
    char buffer[BufferSize];
    int res = snprintf(buffer, BufferSize, "DROP TABLE %s", table.name());

    if (LIKELY(res > 0))
    {
        sqlite3_stmt *statement;

        if (sqlite3_prepare_v2(m_db, buffer, res, &statement, NULL) == SQLITE_OK)
        {
            sqlite3_step(statement);
            return (m_error = sqlite3_finalize(statement)) == SQLITE_OK;
        }
        else
        {
            m_error = sqlite3_errcode(m_db);
        }
    }

    return false;
}

bool Database::perform(const Update &query)
{
    return performQuery(query);
}

bool Database::perform(const Insert &query, Id &id)
{
	ASSERT(m_db != NULL);
	char buffer[BufferSize];
	int res = query.build(buffer, BufferSize);

	if (LIKELY(res > 0))
	{
		sqlite3_stmt *statement;

		if (sqlite3_prepare_v2(m_db, buffer, res, &statement, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(statement) == SQLITE_DONE)
				id = sqlite3_last_insert_rowid(m_db);
			else
				id = -1;

			return (m_error = sqlite3_finalize(statement)) == SQLITE_OK;
		}
		else
		{
			m_error = sqlite3_errcode(m_db);
		}
	}

	return false;
}

bool Database::perform(const Select &query, DataSet &data)
{
	ASSERT(m_db != NULL);
	char buffer[BufferSize];
	int res = query.build(buffer, BufferSize);

	if (LIKELY(res > 0))
	{
		sqlite3_stmt *statement;

		if (sqlite3_prepare_v2(m_db, buffer, res, &statement, NULL) == SQLITE_OK)
		{
			return data.initialize(statement, query.m_fields);
		}
		else
		{
			m_error = sqlite3_errcode(m_db);
		}
	}

	return false;
}

bool Database::perform(const Delete &query)
{
    return performQuery(query);
}

bool Database::transaction()
{
	ASSERT(m_db != NULL);
	sqlite3_stmt *statement;
	bool res = false;

	if (m_transaction)
	{
	    enum { BUfferSize = 32 };
	    char buffer[BUfferSize];
	    int len = snprintf(buffer, BUfferSize, "SAVEPOINT '%d'", m_transaction + 1);

	    if (LIKELY(len > 0))
            if (sqlite3_prepare_v2(m_db, buffer, len, &statement, NULL) == SQLITE_OK)
            {
                res = (sqlite3_step(statement) == SQLITE_DONE);
                m_error = sqlite3_finalize(statement);
            }
            else
            {
                m_error = sqlite3_errcode(m_db);
            }
	}
	else
        if (sqlite3_prepare_v2(m_db, "BEGIN TRANSACTION", sizeof("BEGIN TRANSACTION") - 1, &statement, NULL) == SQLITE_OK)
        {
            res = (sqlite3_step(statement) == SQLITE_DONE);
            m_error = sqlite3_finalize(statement);
        }
        else
        {
            m_error = sqlite3_errcode(m_db);
        }

    if (res)
        ++m_transaction;

    return res;
}

bool Database::commit()
{
	ASSERT(m_db != NULL);
	sqlite3_stmt *statement;
	bool res = false;

    if (m_transaction > 1)
    {
        enum { BUfferSize = 32 };
        char buffer[BUfferSize];
        int len = snprintf(buffer, BUfferSize, "RELEASE '%d'", m_transaction);

        if (LIKELY(len > 0))
            if (sqlite3_prepare_v2(m_db, buffer, len, &statement, NULL) == SQLITE_OK)
            {
                res = (sqlite3_step(statement) == SQLITE_DONE);
                m_error = sqlite3_finalize(statement);
            }
            else
            {
                m_error = sqlite3_errcode(m_db);
            }
    }
    else
        if (sqlite3_prepare_v2(m_db, "COMMIT", sizeof("COMMIT") - 1, &statement, NULL) == SQLITE_OK)
        {
            res = (sqlite3_step(statement) == SQLITE_DONE);
            m_error = sqlite3_finalize(statement);
        }
        else
        {
            m_error = sqlite3_errcode(m_db);
        }

    if (res)
        --m_transaction;

    return res;
}

void Database::rollback()
{
	ASSERT(m_db != NULL);
	sqlite3_stmt *statement;
    bool res = false;

    if (m_transaction > 1)
    {
        enum { BUfferSize = 32 };
        char buffer[BUfferSize];
        int len = snprintf(buffer, BUfferSize, "ROLLBACK TO '%d'", m_transaction);

        if (LIKELY(len > 0))
            if (sqlite3_prepare_v2(m_db, buffer, len, &statement, NULL) == SQLITE_OK)
            {
                res = (sqlite3_step(statement) == SQLITE_DONE);
                m_error = sqlite3_finalize(statement);
            }
            else
            {
                m_error = sqlite3_errcode(m_db);
            }
    }
    else
        if (sqlite3_prepare_v2(m_db, "ROLLBACK", sizeof("ROLLBACK") - 1, &statement, NULL) == SQLITE_OK)
        {
            res = (sqlite3_step(statement) == SQLITE_DONE);
            m_error = sqlite3_finalize(statement);
        }
        else
        {
            m_error = sqlite3_errcode(m_db);
        }

    if (res)
        --m_transaction;
}

void Database::close()
{
	ASSERT(m_db != NULL);
	sqlite3_close(m_db);
	m_db = NULL;
}

bool Database::performQuery(const Query &query)
{
    ASSERT(m_db != NULL);
    char buffer[BufferSize];
    int res = query.build(buffer, BufferSize);

    if (LIKELY(res > 0))
    {
        sqlite3_stmt *statement;

        if (sqlite3_prepare_v2(m_db, buffer, res, &statement, NULL) == SQLITE_OK)
        {
            sqlite3_step(statement);
            return (m_error = sqlite3_finalize(statement)) == SQLITE_OK;
        }
        else
        {
            m_error = sqlite3_errcode(m_db);
        }
    }

    return false;
}

}
