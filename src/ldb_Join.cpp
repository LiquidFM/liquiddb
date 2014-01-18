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
#include "ldb_Join.h"
#include "ldb_conversions_p.h"

#include <brolly/assert.h>


namespace LiquidDb {

Join::Join(const Field &field1, Operator op, const Field &field2, Kind kind) :
	m_fields({ field1, field2 }),
	m_op(op),
	m_kind(kind)
{
	ASSERT(m_fields[0].table != NULL);
	ASSERT(m_fields[0].column != NULL);
	ASSERT(m_fields[1].table != NULL);
	ASSERT(m_fields[1].column != NULL);
}

Join::~Join()
{}

int Join::build(char *buffer, size_t size) const
{
	return snprintf(buffer, size, " %s %s ON %s.%s %s %s.%s",
								  kindToString(m_kind),
								  m_fields[0].table->name(),
								  m_fields[0].table->name(),
								  m_fields[0].column->name,
								  operatorToString(m_op),
								  m_fields[1].table->name(),
								  m_fields[1].column->name);
}

const char *Join::kindToString(Kind kind)
{
	switch (kind)
	{
		case LeftJoin:
		default:
			return "LEFT JOIN";
	}
}

}
