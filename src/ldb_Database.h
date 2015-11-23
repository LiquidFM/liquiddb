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
#ifndef LDB_DATABASE_H_
#define LDB_DATABASE_H_

#include <cstdint>
#include <platform/utils.h>


typedef struct sqlite3 sqlite3;


namespace LiquidDb {

class Table;
class Query;
class Insert;
class Select;
class Update;
class Delete;
class DataSet;


class PLATFORM_MAKE_PUBLIC Database
{
	PLATFORM_MAKE_NONCOPYABLE(Database)

public:
    typedef int64_t Id;
    enum { InvalidId = -1 };

public:
	Database();

#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
	Database(Database &&other);
	Database &operator=(Database &&other);
#endif

	~Database();

	int lastError() const { return m_error; }

	bool open(const char *filename);
	bool exsitst(const Table &table) const;
	bool create(const Table &table);
    bool remove(const Table &table);
	bool perform(const Update &query);
	bool perform(const Insert &query, Id &id);
	bool perform(const Select &query, DataSet &data) const;
    bool perform(const Delete &query);
	bool transaction();
	bool commit();
	void rollback();
	void close();

    int version() const;
    void setVersion(int value);

private:
    bool performQuery(const Query &query);

private:
	sqlite3 *m_db;
	int m_transaction;
	mutable int m_error;
};

}

#endif /* LDB_DATABASE_H_ */
