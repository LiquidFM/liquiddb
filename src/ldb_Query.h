/** @file ldb_Query.h
 *  @brief TODO: Put description here.
 *
 * TODO: Put description here.
 *
 * @copyright
 * This file is part of liquiddb.
 * @n@n
 * Copyright (C) 2011-2013 Dmitriy Vilkov, <dav.daemon@gmail.com>
 * @n@n
 * liquiddb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @n@n
 * liquiddb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * @n@n
 * You should have received a copy of the GNU General Public License
 * along with liquiddb. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LDB_QUERY_H_
#define LDB_QUERY_H_

#include <efc/StaticArray>
#include <liquiddb/Field>


namespace LiquidDb {

class Join;
class Constraint;
class Database;
class Entity;
class Value;


class Query
{
public:
    enum
    {
        MaxFields = 16
    };

    typedef ::EFC::StaticArray<Field, MaxFields> Fields;
    typedef ::EFC::StaticArray<const Join *, MaxFields> Joins;
    typedef ::EFC::StaticArray<const Constraint *, MaxFields> Constraints;

	struct Val
	{
		const Table::Column *column;
		const Value *value;
	};

    typedef ::EFC::StaticArray<Val, MaxFields> Values;

public:
	Query();
	virtual ~Query();

	void join(const Join &link);
	void where(const Constraint &constraint);
    void where(const Constraints &constraints);
    void orderby(const char *alias);
    void orderby(const Table &table, Table::Column::Id column);

	virtual int build(char *buffer, size_t size) const;

protected:
	Joins m_joins;
	Constraints m_constraints;
	Field m_orderby;
};


class Select : public Query
{
public:
	Select(const Table &from);
	virtual ~Select();

	void select(const Table &table);
    void select(Table::Column::Id column);
    void select(const char *expr, const char *alias = NULL);
	void select(const Table &table, Table::Column::Id column);
	virtual int build(char *buffer, size_t size) const;

private:
	friend class Database;
	Fields m_fields;

private:
	const Table *m_from;
};


class Insert : public Query
{
public:
	Insert(const Table &into);
	virtual ~Insert();

	void insert(Table::Column::Id column, Value &value);
	virtual int build(char *buffer, size_t size) const;

private:
	Values m_values;
	const Table *m_into;
};


class Update : public Query
{
public:
	Update(const Table &table);
	virtual ~Update();

	void update(Table::Column::Id column, Value &value);
	virtual int build(char *buffer, size_t size) const;

private:
	Values m_values;
	const Table *m_table;
};


class Delete : public Query
{
public:
    Delete(const Table &from);
    virtual ~Delete();

    virtual int build(char *buffer, size_t size) const;

private:
    const Table *m_from;
};

}

#endif /* LDB_QUERY_H_ */
