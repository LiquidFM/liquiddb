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
#include "ldb_Query.h"
#include "ldb_conversions_p.h"
#include "ldb_Join.h"
#include "ldb_Constraint.h"

#include "structure/ldb_EntitiesTable.h"
#include "structure/ldb_PropertiesTable.h"
#include "structure/ldb_EntityTable.h"
#include "structure/ldb_PropertyTable.h"
#include "entities/ldb_Entity.h"

#include <efc/List>
#include <efc/ScopedPointer>


namespace {
    using namespace EFC;
    using namespace LiquidDb;

    class JoinProperty
    {
    public:
        JoinProperty(const EntityTable &entityTable, const Entity &entity, const Entity &property) :
            m_propertyTable(entity, property),
            m_fieldId1(entityTable, EntityTable::Id),
            m_entityValueId(m_propertyTable, PropertyTable::EntityValueId),
            m_join1(m_fieldId1, Join::Equal, m_entityValueId),

            m_entityPropertyTable(property),
            m_propertyValueId(m_propertyTable, PropertyTable::PropertyValueId),
            m_fieldId2(m_entityPropertyTable, EntityTable::Id),
            m_join2(m_propertyValueId, Join::Equal, m_fieldId2)
        {}

        const Join &join1() const { return m_join1; }
        const Join &join2() const { return m_join2; }
        const EntityTable &propertyTable() const { return m_entityPropertyTable; }

    private:
        PropertyTable m_propertyTable;
        Field m_fieldId1;
        Field m_entityValueId;
        Join m_join1;

        EntityTable m_entityPropertyTable;
        Field m_propertyValueId;
        Field m_fieldId2;
        Join m_join2;
    };

    typedef List<ScopedPointer<JoinProperty>> PropertiesList;
}


namespace LiquidDb {

Query::Query()
{}

Query::~Query()
{}

void Query::join(const Join &link)
{
	m_joins.push_back(&link);
}

void Query::where(const Constraint &constraint)
{
	m_constraints.push_back(&constraint);
}

void Query::where(const Constraints &constraints)
{
    m_constraints = constraints;
}

int Query::build(char *buffer, size_t size) const
{
    int res;
    int len = 0;

    for (Joins::const_iterator i = m_joins.begin(), end = m_joins.end(); i != end; ++i)
    {
        res = (*i)->build(buffer + len, size - len);

        if (LIKELY(res > 0))
            len += res;
        else
            return -1;
    }

    if (!m_constraints.empty())
    {
        res = snprintf(buffer + len, size - len, " WHERE ");

        if (LIKELY(res > 0))
            len += res;
        else
            return -1;

        for (Constraints::const_iterator i = m_constraints.begin(), end = m_constraints.end(); i != end; ++i)
        {
            res = (*i)->build(buffer + len, size - len);

            if (LIKELY(res > 0))
                len += res;
            else
                return -1;
        }
    }

    return len;
}


Select::Select(const Table &from) :
	m_from(&from)
{}

Select::~Select()
{}

void Select::select(const Table &table)
{
	for (unsigned char i = 0, size = table.columnsCount(); i < size; ++i)
		m_fields.push_back({ table, i });
}

void Select::select(const Table &table, unsigned char column)
{
	m_fields.push_back({ table, column });
}

int Select::build(char *buffer, size_t size) const
{
	int len = 0;

	if (!m_fields.empty())
	{
		int res;

		{
			Fields::const_iterator i = m_fields.begin();
			Fields::const_iterator end = m_fields.end();

			res = snprintf(buffer + len, size - len, "SELECT %s.%s", (*i).table->name(), (*i).column->name);

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

			for (++i; i != end; ++i)
			{
				res = snprintf(buffer + len, size - len, ", %s.%s", (*i).table->name(), (*i).column->name);

				if (LIKELY(res > 0))
					len += res;
				else
					return -1;
			}
		}

		res = snprintf(buffer + len, size - len, " FROM %s", m_from->name());

		if (LIKELY(res > 0))
			len += res;
		else
			return -1;

		res = Query::build(buffer + len, size - len);

        if (LIKELY(res >= 0))
            len += res;
        else
            return -1;
	}

	return len;
}


Insert::Insert(const Table &into) :
	m_into(&into)
{}

Insert::~Insert()
{}

void Insert::insert(const Table::Column *column, const void *value)
{
	m_values.push_back({ column, value, 0 });
}

void Insert::insert(const Table::Column *column, const void *value, size_t size)
{
	m_values.push_back({ column, value, size });
}

int Insert::build(char *buffer, size_t size) const
{
	int res = snprintf(buffer, size, "INSERT INTO %s", m_into->name());

	if (LIKELY(res > 0))
	{
		int len = res;

		if (!m_values.empty())
		{
			Values::const_iterator i = m_values.begin();
			Values::const_iterator end = m_values.end();

			res = snprintf(buffer + len, size - len, " (%s", (*i).column->name);

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

			for (++i; i != end; ++i)
			{
				res = snprintf(buffer + len, size - len, ", %s", (*i).column->name);

				if (LIKELY(len > 0))
					len += res;
				else
					return -1;
			}

			res = snprintf(buffer + len, size - len, ") VALUES (");

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

			i = m_values.begin();
			res = printValue(buffer + len, size - len, (*i).column, (*i).value, (*i).size);

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

				res = printValue(buffer + len, size - len, (*i).column, (*i).value, (*i).size);

				if (LIKELY(len > 0))
					len += res;
				else
					return -1;
			}

			res = snprintf(buffer + len, size - len, ")");

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

	        res = Query::build(buffer + len, size - len);

	        if (LIKELY(res >= 0))
	            len += res;
	        else
	            return -1;
		}

        return len;
	}

	return -1;
}


Update::Update(const Table &table) :
	m_table(&table)
{}

Update::~Update()
{}

void Update::update(const Table::Column *column, const void *value)
{
	m_values.push_back({ column, value, 0 });
}

void Update::update(const Table::Column *column, const void *value, size_t size)
{
	m_values.push_back({ column, value, size });
}

int Update::build(char *buffer, size_t size) const
{
	int res = snprintf(buffer, size, "UPDATE %s SET ", m_table->name());

	if (LIKELY(res > 0))
	{
		int len = res;

		if (!m_values.empty())
		{
			Values::const_iterator i = m_values.begin();
			Values::const_iterator end = m_values.end();

			res = snprintf(buffer + len, size - len, "%s = ", (*i).column->name);

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

			res = printValue(buffer + len, size - len, (*i).column, (*i).value, (*i).size);

			if (LIKELY(res > 0))
				len += res;
			else
				return -1;

			for (++i; i != end; ++i)
			{
				res = snprintf(buffer + len, size - len, ", %s = ", (*i).column->name);

				if (LIKELY(res > 0))
					len += res;
				else
					return -1;

				res = printValue(buffer + len, size - len, (*i).column, (*i).value, (*i).size);

				if (LIKELY(res > 0))
					len += res;
				else
					return -1;
			}

	        res = Query::build(buffer + len, size - len);

	        if (LIKELY(res >= 0))
	            len += res;
	        else
	            return -1;

			return len;
		}
	}

	return -1;
}


Delete::Delete(const Table &from) :
    m_from(&from)
{}

Delete::~Delete()
{}

int Delete::build(char *buffer, size_t size) const
{
    int len = 0;
    int res = snprintf(buffer, size, "DELETE FROM %s", m_from->name());

    if (LIKELY(res > 0))
        len += res;
    else
        return -1;

    res = Query::build(buffer + len, size - len);

    if (LIKELY(res >= 0))
        len += res;
    else
        return -1;

    return len;
}


SelectEntity::SelectEntity(const Entity &entity) :
    m_entity(entity)
{}

SelectEntity::~SelectEntity()
{}

void SelectEntity::where(const EntityConstraint &constraint)
{
    Query::where(constraint);
}

int SelectEntity::build(char *buffer, size_t size) const
{
    EntityTable entityTable(m_entity);
    Select query(entityTable);

    query.select(entityTable);
    query.where(m_constraints);

    if (m_entity.type() != Entity::Composite)
        return query.build(buffer, size);
    else
    {
        PropertiesList list;

        for (Entity::Properties::const_iterator i = m_entity.properties().begin(), end = m_entity.properties().end(); i != end; ++i)
        {
            ::EFC::ScopedPointer<JoinProperty> property(new (std::nothrow) JoinProperty(entityTable, m_entity, (*i).second.entity));

            if (LIKELY(property != NULL))
            {
                query.join(property->join1());
                query.join(property->join2());
                query.select(property->propertyTable());

                if (UNLIKELY(list.push_back(std::move(property))) == false)
                    return -1;
            }
        }

        return query.build(buffer, size);
    }

    return -1;
}

}
