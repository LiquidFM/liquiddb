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

#include <platform/platform.h>
#include <liquiddb/Query>


typedef struct sqlite3_stmt sqlite3_stmt;


namespace LiquidDb {

class Database;


class DataSet
{
public:
    class Columns
    {
    public:
        struct const_iterator
        {
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef std::ptrdiff_t                  difference_type;
            struct value_type
            {
                const Table::Column *column() const { return (*iterator).column; }
                bool isNull() const { return dataSet->columnIsNull(pos); }
                void value(void *value) const { dataSet->columnValue(pos, column()->type, value); }
                void value(const void **value, size_t &size) const { dataSet->columnValue(pos, column()->type, value, size); }

                unsigned char pos;
                const DataSet *dataSet;
                Query::Fields::const_iterator iterator;
            };
            typedef const value_type *              pointer;
            typedef const value_type &              reference;

            const_iterator();
            const_iterator(const DataSet &dataSet, const Query::Fields::const_iterator &iterator);

            reference operator*() const { return m_value; }
            pointer operator->() const { return &m_value; }
            const_iterator &operator++() { ++m_value.pos; ++m_value.iterator; return *this; }
            const_iterator operator++(int) { return ++const_iterator(*this); }
            const_iterator &operator--() { --m_value.pos; --m_value.iterator; return *this; }
            const_iterator operator--(int) { return --const_iterator(*this); }
            bool operator==(const const_iterator &x) const { return m_value.iterator == x.m_value.iterator; }
            bool operator!=(const const_iterator &x) const { return m_value.iterator != x.m_value.iterator; }

        private:
            value_type m_value;
        };

    public:
        Columns() :
            m_dataSet(NULL)
        {}

#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
        Columns(const DataSet &dataSet, Columns &&other) :
            m_dataSet(&dataSet),
            m_fields(std::move(other.m_fields))
        {}
#endif

        Columns(const DataSet &dataSet, const Query::Fields &fields) :
            m_dataSet(&dataSet),
            m_fields(fields)
        {}

        const_iterator begin() const { return const_iterator(*m_dataSet, m_fields.begin()); }
        const_iterator end() const { return const_iterator(*m_dataSet, m_fields.end()); }

    private:
        const DataSet *m_dataSet;
        Query::Fields m_fields;
    };

public:
	DataSet();
#if PLATFORM_COMPILER_SUPPORTS(MOVE_SEMANTIC)
    DataSet(DataSet &&other);
#endif
	~DataSet();

	bool isValid() const;
	int lastError() const { return m_error; }
	bool next();

	const Columns &columns() const { return m_columns; }

protected:
	friend class Database;
	bool initialize(sqlite3_stmt *statement, const Query::Fields &fields);

private:
	friend class Columns::const_iterator::value_type;
    bool columnIsNull(unsigned char column) const;
    void columnValue(unsigned char column, Table::Column::Type type, void *value) const;
    void columnValue(unsigned char column, Table::Column::Type type, const void **value, size_t &size) const;
	void columnValueInternal(unsigned char column, Table::Column::Type type, void **value, size_t &size) const;

private:
	int m_error;
	sqlite3_stmt *m_statement;
	Columns m_columns;
};

}

#endif /* LDB_DATASET_H_ */
