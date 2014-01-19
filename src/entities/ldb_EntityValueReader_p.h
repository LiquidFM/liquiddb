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

    EntityValue next() const
    {
        return EntityValue();
    }

    bool eof() const { return m_afterLast; }
    bool bof() const { return m_beforeFirst; }
    void close() { m_afterLast = true; }

private:
    EntityValue doNext() const;
    EntityValue value(const Entity &entity, Entity::Id id, int column) const;
    void property(const EntityValue &value, const Entity &property, int &column) const;
    void skip(const Entity &property, int &column) const;

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
