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
#include "ldb_Link.h"

#include "ldb_conversions_p.h"


namespace LiquidDb {

Link::Link()
{}

Link::~Link()
{}

const char *Link::operatorToString(Operator op)
{
	switch (op)
	{
		case Less:
			return "<";

		case LessEqual:
			return "<=";

		case Greater:
			return ">";

		case GreaterEqual:
			return ">=";

		case Equal:
			return "=";

		case NotEqual:
			return "<>";

		case Like:
			return "LIKE";

        default:
			return "";
	}
}

const char *Link::operatorToString(RangeOperator op)
{
    switch (op)
    {
        case In:
            return "IN";

        case NotIn:
            return "NOT IN";

        default:
            return "";
    }
}

}
