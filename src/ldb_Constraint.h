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
#ifndef LDB_CONSTRAINT_H_
#define LDB_CONSTRAINT_H_

#include <efc/StaticArray>
#include <liquiddb/Field>
#include <liquiddb/Link>
#include <liquiddb/Entity>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class Constraint : public Link
{
public:
	Constraint();
	virtual ~Constraint();
};


class ConstConstraint : public Constraint
{
public:
	ConstConstraint(const Field &field, Operator op, const void *value);

	virtual int build(char *buffer, size_t size) const;

private:
	Field m_field;
	Operator m_op;
	const void *m_value;
};


class SetConstraint : public Constraint
{
public:
    SetConstraint(const Field &field, RangeOperator op, const Entity::IdsList &ids);

    virtual int build(char *buffer, size_t size) const;

private:
    Field m_field;
    RangeOperator m_op;
    const Entity::IdsList &m_idsList;
};


class FieldConstraint : public Constraint
{
public:
	FieldConstraint(const Field &field1, Operator op, const Field &field2);

	virtual int build(char *buffer, size_t size) const;

private:
	Field m_fields[2];
	Operator m_op;
};


class GroupConstraint : public Constraint
{
public:
	enum Type
	{
		And,
		Or
	};

public:
	GroupConstraint(Type op = And);

    Type type() const { return m_op; }
    void setType(Type type) { m_op = type; }

    void add(const Constraint &constraint) { m_constraints.push_back(&constraint); }

	virtual int build(char *buffer, size_t size) const;

	static const char *operatorToString(Type op);

private:
    enum
    {
        MaxConstraints = 16
    };

    typedef ::EFC::StaticArray<const Constraint *, MaxConstraints> Container;

private:
	Type m_op;
	Container m_constraints;
};

}

#endif /* LDB_CONSTRAINT_H_ */
