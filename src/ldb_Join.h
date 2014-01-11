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
#ifndef LDB_JOIN_H_
#define LDB_JOIN_H_

#include <liquiddb/Link>
#include <liquiddb/Field>


namespace LiquidDb {

class Join : public Link
{
public:
	enum Kind
	{
		LeftJoin
	};

public:
	Join(const Field &field1, Operator op, const Field &field2, Kind kind = LeftJoin);
	virtual ~Join();

	virtual int build(char *buffer, size_t size) const;

protected:
	static const char *kindToString(Kind kind);

private:
	Field m_fields[2];
	Operator m_op;
	Kind m_kind;
};

}

#endif /* LDB_JOIN_H_ */
