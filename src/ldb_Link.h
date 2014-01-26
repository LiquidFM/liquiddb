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
#ifndef LDB_LINK_H_
#define LDB_LINK_H_

#include <cstddef>


namespace LiquidDb {

class Link
{
public:
	enum Operator
	{
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,
		Like
	};

    enum RangeOperator
    {
        In,
        NotIn
    };

public:
	Link();
	virtual ~Link();

	virtual int build(char *buffer, size_t size) const = 0;

	static const char *operatorToString(Operator op);
    static const char *operatorToString(RangeOperator op);
};

}

#endif /* LDB_LINK_H_ */
