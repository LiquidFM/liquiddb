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

#include "../structure/ldb_EntityTable.h"
#include "../structure/ldb_PropertyTable.h"
#include "../ldb_Join.h"


namespace LiquidDb {

class EntityValueReader::Implementation
{
public:
    Implementation(const Entity &entity, DataSet &dataSet, PropertiesList &properties) :
        m_afterLast(false),
        m_beforeFirst(false),
        m_entity(entity),
        m_dataSet(std::move(dataSet)),
        m_properties(std::move(properties))
    {}

    const Entity &entity() const { return m_entity; }

    EntityValue next()
    {
        if (m_beforeFirst)
            if (m_dataSet.next())
            {
                m_beforeFirst = false;
                return doNext();
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
                return doNext();
    }

    bool eof() const { return m_afterLast; }
    bool bof() const { return m_beforeFirst; }
    void close() { m_afterLast = true; }

private:
    EntityValue doNext()
    {
        Entity::Id id;
        EntityValue value;
        DataSet::Columns::const_iterator column = m_dataSet.columns().begin();
        DataSet::Columns::const_iterator columnProperty;

        (*column).value(&id);

        if (m_entity.type() == Entity::Composite)
        {
            value = EntityValue::createValue(m_entity, id);

            if (UNLIKELY(value.isValid() == false))
                return EntityValue();

            for (Entity::Id id = value.id(), nextId = id; id == nextId; (*column).value(&nextId))
            {
                ++(columnProperty = column);

                for (Entity::Properties::const_iterator i = m_entity.properties().begin(), end = m_entity.properties().end(); i != end; ++i)
                    if (UNLIKELY(property(value, (*i).second.entity, columnProperty) == false))
                        return EntityValue();

                if (!m_dataSet.next())
                {
                    m_afterLast = true;
                    break;
                }
            }
        }
        else
        {
            value = Implementation::value(id, column);

            if (!m_dataSet.next())
                m_afterLast = true;
        }

        return value;
    }

    EntityValue value(Entity::Id id, DataSet::Columns::const_iterator column) const
    {
//        switch (m_entity.type())
//        {
//            case Database::Int:
//                return createValue(entity, id, contextValue<Database::Int>(m_context, column + 1));
//            case Database::String:
//                return createValue(entity, id, contextValue<Database::String>(m_context, column + 1));
//            case Database::Date:
//                return createValue(entity, id, contextValue<Database::Date>(m_context, column + 1));
//            case Database::Time:
//                return createValue(entity, id, contextValue<Database::Time>(m_context, column + 1));
//            case Database::DateTime:
//                return createValue(entity, id, contextValue<Database::DateTime>(m_context, column + 1));
//            case Database::Memo:
//                return createValue(entity, id, contextValue<Database::Memo>(m_context, column + 1));
//            case Database::Rating:
//                return createValue(entity, id, contextValue<Database::Rating>(m_context, column + 1));
//            case Database::Path:
//                return createValue(entity, id, contextValue<Database::Path>(m_context, column + 1));
//            default:
//                return EntityValue::Holder();
//        }
        return EntityValue();
    }

    bool property(const EntityValue &value, const Entity &property, DataSet::Columns::const_iterator &column) const
    {
        return false;
    }

    void skip(const Entity &property, int &column) const
    {

    }

private:
    mutable bool m_afterLast;
    mutable bool m_beforeFirst;
    Entity m_entity;
    DataSet m_dataSet;
    PropertiesList m_properties;
};


class EntityValueReader::Property
{
public:
    Property(const Table &entityTable, const Entity &entity, const Entity &property) :
        m_propertyTable(entity, property),
        m_entityValueId(m_propertyTable, PropertyTable::EntityValueId),
        m_fieldId1(entityTable, EntityTable::Id),
        m_join1(m_entityValueId, Join::Equal, m_fieldId1),

        m_entityPropertyTable(property),
        m_fieldId2(m_entityPropertyTable, EntityTable::Id),
        m_propertyValueId(m_propertyTable, PropertyTable::PropertyValueId),
        m_join2(m_fieldId2, Join::Equal, m_propertyValueId)
    {}

    const Join &join1() const { return m_join1; }
    const Join &join2() const { return m_join2; }
    const EntityTable &propertyTable() const { return m_entityPropertyTable; }

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

}

#endif /* LDB_ENTITYVALUEREADER_P_H_ */
