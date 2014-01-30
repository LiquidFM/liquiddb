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
#include "ldb_Constraint.h"
#include "ldb_conversions_p.h"


namespace LiquidDb {

Constraint::Constraint()
{}

Constraint::~Constraint()
{}


ConstConstraint::ConstConstraint(const Field &field, Operator op, const void *value) :
	m_field(field),
	m_op(op),
	m_value(value)
{}

int ConstConstraint::build(char *buffer, size_t size) const
{
    int len = snprintf(buffer, size, "(%s.%s %s ", m_field.table->name(), m_field.column->name, operatorToString(m_op));

    if (LIKELY(len > 0))
    {
        int res = printValue(buffer + len, size - len, m_field.column, m_value, 0);

        if (LIKELY(res > 0))
            len += res;
        else
            return -1;

        res = snprintf(buffer + len, size - len, ")");

        if (LIKELY(res > 0))
            return len + res;
    }

	return -1;
}


SetConstraint::SetConstraint(const Field &field, RangeOperator op, const Entity::IdsList &ids) :
    m_field(field),
    m_op(op),
    m_idsList(ids)
{}

int SetConstraint::build(char *buffer, size_t size) const
{
    int len = snprintf(buffer, size, "(%s.%s %s (", m_field.table->name(), m_field.column->name, operatorToString(m_op));

    if (LIKELY(len > 0))
    {
        Entity::IdsList::const_iterator i = m_idsList.begin();
        Entity::IdsList::const_iterator end = m_idsList.end();

        int res = printValue(buffer + len, size - len, m_field.column, &(*i), 0);

        if (LIKELY(res > 0))
            len += res;
        else
            return -1;

        for (++i; i != end; ++i)
        {
            res = snprintf(buffer + len, size - len, ", ");

            if (LIKELY(res > 0))
                len += res;
            else
                return -1;

            res = printValue(buffer + len, size - len, m_field.column, &(*i), 0);

            if (LIKELY(res > 0))
                len += res;
            else
                return -1;
        }

        res = snprintf(buffer + len, size - len, "))");

        if (LIKELY(res > 0))
            return len + res;
    }

    return -1;
}


FieldConstraint::FieldConstraint(const Field &field1, Operator op, const Field &field2) :
	m_fields({ field1, field2 }),
	m_op(op)
{}

int FieldConstraint::build(char *buffer, size_t size) const
{
	return snprintf(buffer, size, "(%s.%s %s %s.%s)", m_fields[0].table->name(), m_fields[0].column->name, operatorToString(m_op), m_fields[1].table->name(), m_fields[1].column->name);
}


GroupConstraint::GroupConstraint(Type op) :
	m_op(op)
{}

int GroupConstraint::build(char *buffer, size_t size) const
{
	int len = 0;

	if (!m_constraints.empty())
	{
		int res1 = 0;
		int res2 = 0;
		Container::const_iterator i = m_constraints.begin();
		Container::const_iterator end = m_constraints.end();

		res1 = snprintf(buffer + len, size - len, "(");

		if (LIKELY(res1 > 0))
			len += res1;
		else
			return -1;

		res1 = (*i)->build(buffer + len, size - len);

		if (LIKELY(res1 > 0))
			len += res1;
		else
			return -1;

		for (++i; i != end; ++i)
		{
			res1 = snprintf(buffer + len, size - len, " %s ", operatorToString(m_op));

			if (LIKELY(res1 > 0))
			{
				res2 = (*i)->build(buffer + len + res1, size - len - res1);

				if (LIKELY(res2 > 0))
					len += res1 + res2;
				else
					return -1;
			}
			else
				return -1;
		}

		res1 = snprintf(buffer + len, size - len, ")");

		if (LIKELY(res1 > 0))
			len += res1;
		else
			return -1;
	}

	return len;
}

const char *GroupConstraint::operatorToString(Type op)
{
	switch (op)
	{
		case And:
			return "AND";

		case Or:
			return "OR";

		default:
			return "";
	}
}

}
