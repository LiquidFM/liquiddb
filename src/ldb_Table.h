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
#ifndef LDB_TABLE_H_
#define LDB_TABLE_H_

#include <cstddef>
#include <platform/utils.h>


namespace LiquidDb {

class Table
{
	PLATFORM_MAKE_NONCOPYABLE(Table)
	PLATFORM_MAKE_NONMOVEABLE(Table)

public:
	struct Column
	{
		PLATFORM_MAKE_STACK_ONLY

	public:
		enum Type
		{
			Int,
			TinyInt,
			SmallInt,
			MediumInt,
			BigInt,
			Text,
			Real,
			Double,
			Float,
			Date,
			Time,
			DateTime,
			Boolean,
			Blob
		};

		typedef unsigned char Id;

	public:
		Column() :
			type(Int),
			name(NULL),
            size(0)
		{}

		Column(Type type, const char *name) :
			type(type),
			name(name),
		    size(0)
		{}

        Column(Type type, const char *name, size_t size) :
            type(type),
            name(name),
            size(size)
        {}

	public:
		Type type;
		const char *name;
		size_t size;
	};


	struct Index
	{
		PLATFORM_MAKE_STACK_ONLY

	public:
		Index() :
			column(NULL),
			name(NULL)
		{}

		Index(const Column *column, const char *name) :
			column(column),
			name(name)
		{}

	public:
		const Column *column;
		const char *name;
	};

public:
	Table();
	virtual ~Table();

	virtual const char *name() const = 0;
	virtual const Column *primaryKeyColumn() const = 0;

	virtual Column::Id columnsCount() const = 0;
	virtual const Column *column(Column::Id i) const = 0;

	virtual Column::Id indexesCount() const = 0;
	virtual const Index *index(Column::Id i) const = 0;
};

}

#endif /* LDB_TABLE_H_ */
