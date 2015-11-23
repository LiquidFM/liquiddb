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
#ifndef LDB_FIELD_H_
#define LDB_FIELD_H_

#include <liquiddb/Table>


namespace LiquidDb {

struct Field
{
	PLATFORM_MAKE_STACK_ONLY

public:
	Field() :
	    expr(NULL),
        alias(NULL),
		table(NULL),
		column(NULL)
	{}

    Field(const char *expr, const char *alias) :
        expr(expr),
        alias(alias),
        table(NULL),
        column(NULL)
    {}

    Field(const Table &table, Table::Column::Id column) :
        expr(NULL),
        alias(NULL),
		table(&table),
		column(table.column(column))
	{}

public:
	const char *expr;
    const char *alias;
	const Table *table;
	const Table::Column *column;
};

}

#endif /* LDB_FIELD_H_ */
