/** @file ldb_EntityValueReader_p.h
 *  @brief TODO: Put description here.
 *
 * TODO: Put description here.
 *
 * @copyright
 * This file is part of liquiddb.
 * @n@n
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#ifndef LDB_ENTITYVALUEREADER_P_H_
#define LDB_ENTITYVALUEREADER_P_H_

#include "ldb_EntityValueReader.h"
#include "ldb_EntityValue_p.h"
#include "ldb_Entity_p.h"

#include "../structure/ldb_EntityTable.h"
#include "../structure/ldb_PropertyTable.h"
#include "../ldb_Join.h"

#include <efc/List>
#include <efc/ScopedPointer>
#include <brolly/assert.h>


namespace LiquidDb {

class EntityValueReader::Property
{
public:
    Property()
    {}
    virtual ~Property()
    {}

    virtual bool setupQuery(Select &query) = 0;
};


class EntityValueReader::SimpleProperty : public EntityValueReader::Property
{
public:
    SimpleProperty(const Table &entityTable, const Entity &entity, const Entity &property) :
        m_propertyTable(entity, property),
        m_entityValueId(m_propertyTable, PropertyTable::EntityValueId),
        m_fieldId1(entityTable, EntityTable::Id),
        m_join1(m_entityValueId, Join::Equal, m_fieldId1),

        m_entityPropertyTable(property),
        m_fieldId2(m_entityPropertyTable, EntityTable::Id),
        m_propertyValueId(m_propertyTable, PropertyTable::PropertyValueId),
        m_join2(m_fieldId2, Join::Equal, m_propertyValueId)
    {}

    virtual bool setupQuery(Select &query)
    {
        query.join(m_join1);
        query.join(m_join2);
        query.select(m_entityPropertyTable);
        return true;
    }

protected:
    const EntityTable &entityTable() const { return m_entityPropertyTable; }

private:
    PropertyTable m_propertyTable;
    Field m_entityValueId;
    Field m_fieldId1;
    Join m_join1;

    EntityTable m_entityPropertyTable;
    Field m_fieldId2;
    Field m_propertyValueId;
    Join m_join2;
};


class EntityValueReader::CompositeProperty : public EntityValueReader::SimpleProperty
{
public:
    CompositeProperty(const Table &entityTable, const Entity &entity, const Entity &property) :
        SimpleProperty(entityTable, entity, property),
        m_entity(property)
    {}

    virtual bool setupQuery(Select &query)
    {
        PropertyHolder property;

        SimpleProperty::setupQuery(query);

        for (auto i : m_entity.properties())
        {
            if (i.second.entity.type() == Entity::Composite)
                property.reset(new (std::nothrow) CompositeProperty(entityTable(), m_entity, i.second.entity));
            else
                property.reset(new (std::nothrow) SimpleProperty(entityTable(), m_entity, i.second.entity));

            if (UNLIKELY(property == NULL))
                return false;

            if (UNLIKELY(property->setupQuery(query) == false))
                return false;

            if (UNLIKELY(m_properties.push_back(std::move(property))) == false)
                return false;
        }

        return true;
    }

private:
    Entity m_entity;
    PropertiesList m_properties;
};


class EntityValueReader::Context
{
public:
    Context(const Entity &entity) :
        m_entity(entity),
        m_entityTable(m_entity)
    {}

    const Entity &entity() const { return m_entity; }
    const EntityTable &entityTable() const { return m_entityTable; }

    bool setupQuery(Select &query)
    {
        PropertyHolder property;

        query.select(m_entityTable);

        for (auto i : m_entity.properties())
        {
            if (i.second.entity.type() == Entity::Composite)
                property.reset(new (std::nothrow) CompositeProperty(m_entityTable, m_entity, i.second.entity));
            else
                property.reset(new (std::nothrow) SimpleProperty(m_entityTable, m_entity, i.second.entity));

            if (UNLIKELY(property == NULL))
                return false;

            if (UNLIKELY(property->setupQuery(query) == false))
                return false;

            if (UNLIKELY(m_properties.push_back(std::move(property))) == false)
                return false;
        }

        return true;
    }

private:
    Entity m_entity;
    EntityTable m_entityTable;
    PropertiesList m_properties;
};


class EntityValueReader::Implementation
{
public:
    Implementation(DataSet &dataSet, ContextHolder &context) :
        m_afterLast(false),
        m_beforeFirst(true),
        m_dataSet(std::move(dataSet)),
        m_context(std::move(context))
    {
        ASSERT(m_context != NULL);
        ASSERT(m_dataSet.isValid());
    }

    bool isValid() const { return m_dataSet.isValid(); }
    const Entity &entity() const { return m_context->entity(); }

    EntityValue next()
    {
        if (m_beforeFirst)
            if (m_dataSet.next())
            {
                m_beforeFirst = false;
                return doNext(m_context->entity());
            }
            else
            {
                m_beforeFirst = false;
                m_afterLast = true;
                return EntityValue();
            }
        else
            if (m_afterLast)
                return EntityValue();
            else
                return doNext(m_context->entity());
    }

    bool eof() const { return m_afterLast; }
    bool bof() const { return m_beforeFirst; }
    void refresh()
    {
        m_dataSet.reset();
        m_afterLast = m_dataSet.lastError() != 0;
        m_beforeFirst = !m_afterLast;
    }
    void close() { m_afterLast = true; }

private:
    EntityValue doNext(const Entity &entity)
    {
        Value id;
        EntityValue value;
        DataSet::Columns::const_iterator column = m_dataSet.columns().begin();
        DataSet::Columns::const_iterator columnProperty;

        (*column).value(id);

        if (entity.type() == Entity::Composite)
        {
            value = EntityValue::createValue(entity, id);

            if (UNLIKELY(value.isValid() == false))
                return EntityValue();

            for (Entity::Id currentId = id, nextId = id; currentId == nextId; nextId = (*column).value(id))
            {
                ++(columnProperty = column);

                for (Entity::Properties::const_iterator i = entity.properties().begin(), end = entity.properties().end(); i != end; ++i)
                    property(value, (*i).second.entity, columnProperty);

                if (!m_dataSet.next())
                {
                    m_afterLast = true;
                    break;
                }
            }
        }
        else
        {
            value = Implementation::value(entity, id, ++column);

            if (!m_dataSet.next())
                m_afterLast = true;
        }

        return value;
    }

    EntityValue value(const Entity &entity, Entity::Id id, const DataSet::Columns::const_iterator &column) const
    {
        Value val;
        ::EFC::Variant value;

        (*column).value(val);
        get(val, entity, value);

        return EntityValue::createValue(entity, id, value);
    }

    void property(const EntityValue &value, const Entity &property, DataSet::Columns::const_iterator &column) const
    {
        if ((*column).isNull())
            skip(property, column);
        else
        {
            Value id;
            EntityValue lastValue;

            (*column).value(id);
            lastValue = static_cast<CompositeEntityValue::Implementation *>(value.m_implementation.get())->value(property, id);

            if (lastValue.isValid())
            {
                if (property.type() != Entity::Composite)
                    (++(++column));
                else
                {
                    ++column;

                    for (Entity::Properties::const_iterator i = property.properties().begin(), end = property.properties().end(); i != end; ++i)
                        Implementation::property(lastValue, (*i).second.entity, column);
                }

                return;
            }

            if (property.type() == Entity::Composite)
            {
                EntityValue localValue(EntityValue::createValue(property, id));
                ++column;

                for (Entity::Properties::const_iterator i = property.properties().begin(), end = property.properties().end(); i != end; ++i)
                    Implementation::property(localValue, (*i).second.entity, column);

                EntityValue::addValue(value, localValue);
            }
            else
            {
                ++column;
                EntityValue::addValue(value, Implementation::value(property, id, column));
                ++column;
            }
        }
    }

    void skip(const Entity &property, DataSet::Columns::const_iterator &column) const
    {
        if (property.type() != Entity::Composite)
            (++(++column));
        else
        {
            ++column;

            for (Entity::Properties::const_iterator i = property.properties().begin(), end = property.properties().end(); i != end; ++i)
                skip((*i).second.entity, column);
        }
    }

private:
    mutable bool m_afterLast;
    mutable bool m_beforeFirst;
    DataSet m_dataSet;
    ContextHolder m_context;
};

}

#endif /* LDB_ENTITYVALUEREADER_P_H_ */
