/** @internal @file ldb_Storage.cpp
 *  @brief TODO: Put description here.
 *
 * TODO: Put description here.
 *
 * @copyright
 * This file is part of liquiddb.
 * @n@n
 * Copyright (C) 2011-2013 Dmitriy Vilkov, <dav.daemon@gmail.com>
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
#include "ldb_Storage.h"
#include "ldb_Query.h"
#include "ldb_DataSet.h"
#include "ldb_Constraint.h"

#include "structure/ldb_EntitiesTable.h"
#include "structure/ldb_PropertiesTable.h"
#include "structure/ldb_MetaPropertiesTable.h"
#include "structure/ldb_EntityTable.h"
#include "structure/ldb_PropertyTable.h"

#include "entities/ldb_Entity_p.h"
#include "entities/ldb_EntityValueReader.h"


namespace {
    using namespace LiquidDb;

    struct BinaryValue
    {
        size_t size;
        const char *value;
    };

    inline Entity::IdsList setToList(const Entity::IdsSet &ids)
    {
        Entity::IdsList res;

        for (Entity::IdsSet::const_iterator i = ids.begin(), end = ids.end(); i != end; ++i)
            res.push_back(*i);

        return res;
    }
}


namespace LiquidDb {

Storage::Storage(const char *fileName, bool create)
{
    if (m_database.open(fileName))
        if (create)
        {
            EntitiesTable entitiesTable;

            if (m_database.create(entitiesTable))
            {
                PropertiesTable propertiesTable;

                if (m_database.create(propertiesTable))
                {
                    MetaPropertiesTable metaPropertiesTable;
                    m_database.create(metaPropertiesTable);
                }
            }
        }
        else
        {
            if (loadEntities())
                if (loadProperties())
                    loadMetaProperties();
        }
}

EntityValueReader Storage::perform(const SelectEntity &query)
{
    return EntityValueReader();
}

Entity Storage::createEntity(Entity::Type type, const char *name, const char *title)
{
    Entity::Id id;
    EntitiesTable entitiesTable;
    Insert query(entitiesTable);

    query.insert(entitiesTable.column(EntitiesTable::Type), &type);
    query.insert(entitiesTable.column(EntitiesTable::Name), name);
    query.insert(entitiesTable.column(EntitiesTable::Title), title);

    if (m_database.perform(query, id))
    {
        EntityTable entityTable(id, type);

        if (m_database.create(entityTable))
            return Entity(id, type, name, title);
    }

    return Entity();
}

bool Storage::removeEntity(const Entity &entity)
{
    Entity::Id id = entity.id();
    EntitiesTable entitiesTable;
    PropertiesTable propertiesTable;

    {
        Delete query(entitiesTable);

        Field entityId(entitiesTable, entitiesTable.column(EntitiesTable::Id));
        ConstConstraint constraint(entityId, Constraint::Equal, &id);

        query.where(constraint);

        if (!m_database.perform(query))
            return false;
    }

    {
        Delete query(propertiesTable);

        Field propertyId(propertiesTable, propertiesTable.column(PropertiesTable::PropertyId));
        ConstConstraint constraint(propertyId, Constraint::Equal, &id);

        query.where(constraint);

        if (!m_database.perform(query))
            return false;
    }

    {
        EntityTable entityTable(entity);

        if (!m_database.remove(entityTable))
            return false;
    }

    if (cleanupParentsValues(entity) &&
        (entity.type() != Entity::Composite || cleanupPropertyValues(entity)))
    {
        for (Entity::Parents::const_iterator i = entity.parents().begin(), end = entity.parents().end(); i != end; ++i)
            (*i).second.m_implementation->remove(entity);

        for (Entity::Properties::const_iterator i = entity.properties().begin(), end = entity.properties().end(); i != end; ++i)
            (*i).second.entity.m_implementation->removeParent(entity);

        m_entities.erase(entity.id());
    }

    return true;
}

bool Storage::addProperty(const Entity &entity, const Entity &property, const char *name)
{
    if (entity != property &&
        entity.type() == Entity::Composite &&
        entity.properties().find(property.id()) == entity.properties().end() &&
        !isThereCycles(entity, property))
    {
        Entity::Id id;
        Entity::Id entityId = entity.id();
        Entity::Id propertyId = property.id();

        PropertiesTable propertiesTable;
        Insert query(propertiesTable);

        query.insert(propertiesTable.column(PropertiesTable::EntityId), &entityId);
        query.insert(propertiesTable.column(PropertiesTable::PropertyId), &propertyId);
        query.insert(propertiesTable.column(PropertiesTable::Name), name);

        if (m_database.perform(query, id))
        {
            entity.m_implementation->add(property, name);
            property.m_implementation->addParent(entity);

            return true;
        }
    }

    return false;
}

bool Storage::renameProperty(const Entity &entity, const Entity &property, const char *name)
{
    return false;
}

bool Storage::removeProperty(const Entity &entity, const Entity &property)
{
    return false;
}

EntityValue Storage::addValue(const Entity &entity)
{
    return EntityValue();
}

bool Storage::addValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{
    return false;
}

bool Storage::addValue(const EntityValue &entityValue, const CompositeEntityValue::Values &propertyValues)
{
    return false;
}

EntityValue Storage::addValue(const Entity &entity, const ::EFC::Variant &value)
{
    return EntityValue();
}

bool Storage::updateValue(const EntityValue &value, const ::EFC::Variant &newValue)
{
    return false;
}

bool Storage::removeValue(const Entity &entity, const Entity::IdsList &ids)
{
    if (ids.empty())
        return true;
    else
        if (cleanupParentsValues(entity, ids))
            if (entity.type() != Entity::Composite)
                return removeEntityValues(entity, ids);
            else
                if (cleanupPropertyValues(entity, ids))
                    return removeEntityValues(entity, ids);

    return false;
}

bool Storage::removeValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{
    return false;
}

bool Storage::loadEntities()
{
    EntitiesTable entitiesTable;
    Select query(entitiesTable);
    DataSet dataSet;

    query.select(entitiesTable);

    if (m_database.perform(query, dataSet))
    {
        while (dataSet.next())
        {
            Entity::Id id;
            Entity::Type type;
            BinaryValue nameValue[2];

            dataSet.columnValue(entitiesTable.column(EntitiesTable::Id), &id);
            dataSet.columnValue(entitiesTable.column(EntitiesTable::Type), &type);
            dataSet.columnValue(entitiesTable.column(EntitiesTable::Name), reinterpret_cast<const void **>(&nameValue[0].value), nameValue[0].size);
            dataSet.columnValue(entitiesTable.column(EntitiesTable::Title), reinterpret_cast<const void **>(&nameValue[1].value), nameValue[1].size);

            Entity entity(id, type, nameValue[0].value, nameValue[1].value);

            if (LIKELY(entity.isValid() == true))
                m_entities.insert(Entities::value_type(id, std::move(entity)));
            else
                return false;

        }

        return true;
    }

    return false;
}

bool Storage::loadProperties()
{
    Entity::Id id;
    DataSet dataSet;
    PropertiesTable propertiesTable;
    BinaryValue nameValue;

    Field entityId(propertiesTable, propertiesTable.column(PropertiesTable::EntityId));
    ConstConstraint constraint(entityId, Constraint::Equal, &id);
    Select query(propertiesTable);

    query.select(propertiesTable);
    query.where(constraint);

    for (Entities::const_iterator q, i = m_entities.begin(), end = m_entities.end(); i != end; ++i)
    {
        id = (*i).first;

        if (m_database.perform(query, dataSet))
        {
            while (dataSet.next())
            {
                dataSet.columnValue(propertiesTable.column(PropertiesTable::PropertyId), &id);
                dataSet.columnValue(propertiesTable.column(PropertiesTable::Name), reinterpret_cast<const void **>(&nameValue.value), nameValue.size);

                q = m_entities.find(id);

                if (q != end)
                {
                    (*i).second.m_implementation->add((*q).second, nameValue.value);
                    (*q).second.m_implementation->addParent((*i).second);
                }
                else
                    return false;
            }
        }
        else
            return false;
    }

    return true;
}

bool Storage::loadMetaProperties()
{
    return true;
}

bool Storage::isThereCycles(const Entity &entity, const Entity &property) const
{
    for (Entity::Parents::const_iterator i = entity.parents().begin(), end = entity.parents().end(); i != end; ++i)
        if ((*i).second.id() == property.id())
            return true;
        else
            return isThereCycles((*i).second, property);

    return false;
}

bool Storage::removeEntityValue(const Entity &entity, Entity::Id id)
{
    EntityTable entityTable(entity);
    Delete query(entityTable);

    Field fieldId(entityTable, entityTable.column(EntityTable::Id));
    ConstConstraint constraint(fieldId, Constraint::Equal, &id);

    query.where(constraint);

    return m_database.perform(query);
}

bool Storage::removeEntityValues(const Entity &entity, const Entity::IdsList &ids)
{
    EntityTable entityTable(entity);
    Delete query(entityTable);

    Field id(entityTable, entityTable.column(EntityTable::Id));
    SetConstraint constraint(id, Constraint::In, ids);

    query.where(constraint);

    return m_database.perform(query);
}

bool Storage::removeOverlappingIds(const Entity &entity, const Entity &property, Entity::IdsSet &ids)
{
    if (!ids.empty())
        for (Entity::Parents::const_iterator i = property.parents().begin(), end = property.parents().end(); i != end; ++i)
            if ((*i).second != entity)
            {
                PropertyTable propertyTable((*i).second.id(), property.id());

                Select query(propertyTable);
                query.select(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));

                Field propertyValueId(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));
                SetConstraint constraint(propertyValueId, Constraint::In, setToList(ids));

                query.where(constraint);

                {
                    Entity::Id id;
                    DataSet dataSet;

                    if (!m_database.perform(query, dataSet))
                        return false;

                    while (dataSet.next())
                    {
                        dataSet.columnValue(propertyTable.column(PropertyTable::PropertyValueId), &id);
                        ids.erase(id);
                    }
                }

                if (ids.empty())
                    break;
            }

    return true;
}

bool Storage::removeSelfOverlappingIds(const Entity &entity, const Entity::IdsList &entityIds, const Entity &property, Entity::IdsSet &propertyIds)
{
    if (!propertyIds.empty())
    {
        PropertyTable propertyTable(entity.id(), property.id());

        Select query(propertyTable);
        query.select(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));

        Field entityValueId(propertyTable, propertyTable.column(PropertyTable::EntityValueId));
        SetConstraint constraint1(entityValueId, Constraint::NotIn, entityIds);

        Field propertyValueId(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));
        SetConstraint constraint2(propertyValueId, Constraint::In, setToList(propertyIds));

        GroupConstraint constraint(GroupConstraint::And);
        constraint.add(constraint1);
        constraint.add(constraint2);

        query.where(constraint);

        {
            Entity::Id id;
            DataSet dataSet;

            if (!m_database.perform(query, dataSet))
                return false;

            while (dataSet.next())
            {
                dataSet.columnValue(propertyTable.column(PropertyTable::PropertyValueId), &id);
                propertyIds.erase(id);
            }
        }
    }

    return true;
}

bool Storage::cleanupParentsValues(const Entity &entity)
{
    for (Entity::Parents::const_iterator i = entity.parents().begin(), end = entity.parents().end(); i != end; ++i)
    {
        PropertyTable propertyTable((*i).second.id(), entity.id());

        if (!m_database.remove(propertyTable))
            return false;
    }

    return true;
}

bool Storage::cleanupParentsValues(const Entity &entity, const Entity::IdsList &ids)
{
    for (Entity::Parents::const_iterator i = entity.parents().begin(), end = entity.parents().end(); i != end; ++i)
    {
        PropertyTable propertyTable((*i).second.id(), entity.id());
        Delete query(propertyTable);

        Field propertyValueId(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));
        SetConstraint constraint(propertyValueId, Constraint::In, ids);

        query.where(constraint);

        if (!m_database.perform(query))
            return false;
    }

    return true;
}

bool Storage::cleanupPropertyValues(const Entity &entity)
{
    for (Entity::Properties::const_iterator i = entity.properties().begin(), end = entity.properties().end(); i != end; ++i)
    {
        PropertyTable propertyTable(entity.id(), (*i).second.entity.id());

        if (!m_database.remove(propertyTable))
            return false;
    }

    return true;
}


bool Storage::cleanupPropertyValues(const Entity &entity, const Entity::IdsList &ids)
{
    Entity::Id id;
    Entity::IdsSet propertyIds;

    for (Entity::Properties::const_iterator i = entity.properties().begin(), end = entity.properties().end(); i != end; ++i)
    {
        PropertyTable propertyTable(entity.id(), (*i).second.entity.id());

        Select query(propertyTable);
        query.select(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));

        Field entityValueId(propertyTable, propertyTable.column(PropertyTable::EntityValueId));
        SetConstraint constraint(entityValueId, Constraint::In, ids);
        query.where(constraint);

        {
            DataSet dataSet;

            if (!m_database.perform(query, dataSet))
                return false;

            propertyIds.clear();

            while (dataSet.next())
            {
                dataSet.columnValue(propertyTable.column(PropertyTable::PropertyValueId), &id);
                propertyIds.insert(id);
            }
        }

        if (!removeSelfOverlappingIds(entity, ids, (*i).second.entity, propertyIds) ||
            !removeOverlappingIds(entity, (*i).second.entity, propertyIds) ||
            !removeValue((*i).second.entity, setToList(propertyIds)))
            return false;
    }

    return true;
}

bool Storage::cleanupPropertyValues(const Entity &entity, const Entity &property)
{
    Entity::IdsSet ids;
    PropertyTable propertyTable(entity.id(), property.id());

    Select query(propertyTable);
    query.select(propertyTable, propertyTable.column(PropertyTable::PropertyValueId));

    {
        Entity::Id id;
        DataSet dataSet;

        if (!m_database.perform(query, dataSet))
            return false;

        while (dataSet.next())
        {
            dataSet.columnValue(propertyTable.column(PropertyTable::PropertyValueId), &id);
            ids.insert(id);
        }

        if (removeOverlappingIds(entity, property, ids))
            return removeValue(property, setToList(ids));
    }

    return false;
}

}
