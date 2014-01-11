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
#ifndef LDB_DATASET_H_
#define LDB_DATASET_H_

#include <efc/Map>
#include <liquiddb/Query>


typedef struct sqlite3_stmt sqlite3_stmt;


namespace LiquidDb {

class Database;


class DataSet
{
public:
	DataSet();
	~DataSet();

	bool isValid() const;
	int lastError() const { return m_error; }
	bool next();

	void columnValue(const Table::Column *column, void *value) const;
	void columnValue(const Table::Column *column, const void **value, size_t &size) const;

protected:
	friend class Database;
	bool initialize(sqlite3_stmt *statement, const Query::Fields &fields);

private:
	void columnValueInternal(const Table::Column *column, void **value, size_t &size) const;

private:
	typedef ::EFC::Map<const Table::Column *, int> Map;

private:
	int m_error;
	sqlite3_stmt *m_statement;
	Map m_fields;
};

}

#endif /* LDB_DATASET_H_ */
