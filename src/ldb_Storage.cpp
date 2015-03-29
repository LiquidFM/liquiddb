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
#include "ldb_Join.h"

#include "structure/ldb_EntitiesTable.h"
#include "structure/ldb_PropertiesTable.h"
#include "structure/ldb_EntityTable.h"
#include "structure/ldb_PropertyTable.h"

#include "entities/ldb_Entity_p.h"
#include "entities/ldb_EntityValueReader_p.h"

#include <efc/ScopedPointer>
#include <brolly/assert.h>


namespace {
    using namespace LiquidDb;

    inline Entity::IdsList setToList(const Entity::IdsSet &ids)
    {
        Entity::IdsList res;

        for (Entity::IdsSet::const_iterator i = ids.begin(), end = ids.end(); i != end; ++i)
            if (UNLIKELY(res.push_back(*i) == false))
                return Entity::IdsList();

        return res;
    }
}


namespace LiquidDb {

Storage::Storage(const ::EFC::String &fileName, bool create) :
    m_undoStack(m_entities)
{
    if (m_database.open(fileName))
        if (create)
        {
            EntitiesTable entitiesTable;

            if (m_database.create(entitiesTable))
            {
                PropertiesTable propertiesTable;

                if (m_database.create(propertiesTable))
                    m_database.setVersion(0);
            }
        }
        else
        {
            if (loadEntities())
                if (loadProperties())
                    loadMetaProperties();
        }
}

bool Storage::transaction()
{
    if (!m_database.transaction())
        return false;
    else
        if (UNLIKELY(m_undoStack.transaction() == false))
        {
            m_database.rollback();
            return false;
        }

    return true;
}

bool Storage::commit()
{
    if (m_database.commit())
    {
        m_undoStack.commit();
        return true;
    }

    return false;
}

void Storage::rollback()
{
    m_database.rollback();
    m_undoStack.rollback();
}

EntityValueReader Storage::entityValues(const Entity &entity) const
{
    EntityValueReader::ContextHolder context(new (std::nothrow) EntityValueReader::Context(entity));

    if (LIKELY(context != NULL))
    {
        DataSet data;
        Select query(context->entityTable());

        context->setupQuery(query);

        if (m_database.perform(query, data))
            return EntityValueReader(new (std::nothrow) EntityValueReader::Implementation(data, context));
    }

    return EntityValueReader();
}

EntityValueReader Storage::entityValues(const Entity &entity, const Constraint &constraint) const
{
    EntityValueReader::ContextHolder context(new (std::nothrow) EntityValueReader::Context(entity));

    if (LIKELY(context != NULL))
    {
        DataSet data;
        Select query(context->entityTable());

        context->setupQuery(query);
        query.where(constraint);

        if (m_database.perform(query, data))
            return EntityValueReader(new (std::nothrow) EntityValueReader::Implementation(data, context));
    }

    return EntityValueReader();
}

::EFC::Variant Storage::metaPropertyValue(const Entity &entity, unsigned char property) const
{
    ASSERT(property < EntitiesTable::MetaPropertiesCount);
    ::EFC::Variant res;
    DataSet dataSet;

    EntitiesTable entitiesTable;
    Select query(entitiesTable);
    query.select(EntitiesTable::ColumnsCount + property);

    Value idValue = entity.id();
    Field idField(entitiesTable, EntitiesTable::Id);
    ConstConstraint constraint(idField, Constraint::Equal, idValue);
    query.where(constraint);

    if (m_database.perform(query, dataSet) && dataSet.next())
    {
        (*dataSet.columns().begin()).value(idValue);
        res.deserialize(reinterpret_cast<const unsigned char *>(idValue.str()), idValue.size());
    }

    return res;
}

bool Storage::setMetaPropertyValue(const Entity &entity, unsigned char property, const ::EFC::Variant &value)
{
    ASSERT(property < EntitiesTable::MetaPropertiesCount);
    ASSERT(value.isValid());

    size_t size = value.serialize(NULL, 0);
    ::EFC::ScopedPointer<unsigned char> buffer(new (std::nothrow) unsigned char[size]);

    if (LIKELY(buffer != NULL))
    {
        EntitiesTable entitiesTable;
        Update query(entitiesTable);

        value.serialize(buffer.get(), size);

        Value val;
        val.str() = reinterpret_cast<const char *>(buffer.get());
        val.size() = size;
        query.update(EntitiesTable::ColumnsCount + property, val);

        Value idVal = entity.id();
        Field idField(entitiesTable, EntitiesTable::Id);
        ConstConstraint constraint(idField, Constraint::Equal, idVal);
        query.where(constraint);

        if (m_database.perform(query))
            return true;
    }

    return false;
}

Entity Storage::createEntity(Entity::Type type, const ::EFC::String &name, const ::EFC::String &title)
{
    Entity::Id id;
    Value typeVal = type;
    Value nameVal = name.c_str();
    Value titleVal = title.c_str();

    EntitiesTable entitiesTable;
    Insert query(entitiesTable);

    query.insert(EntitiesTable::Type, typeVal);
    query.insert(EntitiesTable::Name, nameVal);
    query.insert(EntitiesTable::Title, titleVal);

    if (m_database.perform(query, id))
    {
        EntityTable entityTable(id, type);

        if (m_database.create(entityTable))
            return m_undoStack.undoAddEntity(id, type, name, title);
    }

    return Entity();
}

bool Storage::removeEntity(const Entity &entity)
{
    Value id = entity.id();

    EntitiesTable entitiesTable;
    PropertiesTable propertiesTable;

    {
        Delete query(entitiesTable);

        Field entityId(entitiesTable, EntitiesTable::Id);
        ConstConstraint constraint(entityId, Constraint::Equal, id);

        query.where(constraint);

        if (!m_database.perform(query))
            return false;
    }

    {
        Delete query(propertiesTable);

        Field propertyId(propertiesTable, PropertiesTable::PropertyId);
        ConstConstraint constraint(propertyId, Constraint::Equal, id);

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
        return m_undoStack.undoRemoveEntity(entity);
    }

    return true;
}

bool Storage::addProperty(const Entity &entity, const Entity &property, const ::EFC::String &name)
{
    ASSERT(entity != property);
    ASSERT(entity.type() == Entity::Composite);
    ASSERT(entity.properties().find(property.id()) == entity.properties().end());

    if (!isThereCycles(entity, property))
    {
        Entity::Id id;
        Value entityId = entity.id();
        Value propertyId = property.id();
        Value nameVal = name.c_str();

        PropertiesTable propertiesTable;
        Insert query(propertiesTable);

        query.insert(PropertiesTable::EntityId, entityId);
        query.insert(PropertiesTable::PropertyId, propertyId);
        query.insert(PropertiesTable::Name, nameVal);

        if (m_database.perform(query, id))
        {
            PropertyTable propertyTable(entity, property);

            if (m_database.create(propertyTable))
                return m_undoStack.undoAddProperty(entity, property, name);
        }
    }

    return false;
}

bool Storage::renameProperty(const Entity &entity, const Entity &property, const ::EFC::String &name)
{
    ASSERT(entity.properties().find(property.id()) != entity.properties().end());

    Value nameVal = name.c_str();
    Value entityId = entity.id();
    Value propertyId = property.id();

    PropertiesTable propertiesTable;
    Update query(propertiesTable);

    query.update(PropertiesTable::Name, nameVal);

    Field entityIdField(propertiesTable, PropertiesTable::EntityId);
    ConstConstraint constraint1(entityIdField, Constraint::Equal, entityId);

    Field propertyIdField(propertiesTable, PropertiesTable::PropertyId);
    ConstConstraint constraint2(propertyIdField, Constraint::Equal, propertyId);

    GroupConstraint constraint(GroupConstraint::And);
    constraint.add(constraint1);
    constraint.add(constraint2);

    query.where(constraint);

    if (m_database.perform(query))
        return m_undoStack.undoRenameProperty(entity, property, name);

    return false;
}

bool Storage::removeProperty(const Entity &entity, const Entity &property)
{
    ASSERT(entity != property);
    ASSERT(entity.type() == Entity::Composite);
    ASSERT(entity.properties().find(property.id()) != entity.properties().end());

    Value entityId = entity.id();
    Value propertyId = property.id();

    PropertiesTable propertiesTable;
    Delete query(propertiesTable);

    Field entityIdField(propertiesTable, PropertiesTable::EntityId);
    ConstConstraint constraint1(entityIdField, Constraint::Equal, entityId);

    Field propertyIdField(propertiesTable, PropertiesTable::PropertyId);
    ConstConstraint constraint2(propertyIdField, Constraint::Equal, propertyId);

    GroupConstraint constraint(GroupConstraint::And);
    constraint.add(constraint1);
    constraint.add(constraint2);

    query.where(constraint);

    if (m_database.perform(query))
        return m_undoStack.undoRemoveProperty(entity, property);

    return false;
}

EntityValue Storage::addValue(const Entity &entity)
{
    ASSERT(entity.type() == Entity::Composite);

    Entity::Id id;

    EntityTable entityTable(entity);
    Insert query(entityTable);

    if (m_database.perform(query, id))
        return EntityValue::createValue(entity, id);

    return EntityValue();
}

bool Storage::addValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{
    ASSERT(entityValue.entity().type() == Entity::Composite);

    CompositeEntityValue compositeEntityValue(entityValue);
    const CompositeEntityValue::Values &values = compositeEntityValue.values(propertyValue.entity());

    if (values.find(propertyValue.id()) == values.end())
    {
        Entity::Id id;
        Value entityId = entityValue.id();
        Value propertyId = propertyValue.id();

        PropertyTable propertyTable(entityValue.entity(), propertyValue.entity());
        Insert query(propertyTable);

        query.insert(PropertyTable::EntityValueId, entityId);
        query.insert(PropertyTable::PropertyValueId, propertyId);

        if (m_database.perform(query, id))
            return m_undoStack.undoAddValue(entityValue, propertyValue);
    }

    return false;
}

bool Storage::addValue(const EntityValue &entityValue, const EntityValue::List &propertyValues)
{
    ASSERT(!propertyValues.empty());
    ASSERT(entityValue.entity().type() == Entity::Composite);

    Entity::Id id;
    Value entityId = entityValue.id();
    Value propertyId;

    CompositeEntityValue compositeEntityValue(entityValue);

    for (EntityValue::List::const_iterator i = propertyValues.begin(), end = propertyValues.end(); i != end; ++i)
    {
        if (entityValue.entity().properties().find(i->entity().id()) == entityValue.entity().properties().end())
            return false;

        const CompositeEntityValue::Values &values = compositeEntityValue.values(i->entity());

        if (values.find(i->id()) != values.end())
            return false;
    }

    for (EntityValue::List::const_iterator i = propertyValues.begin(), end = propertyValues.end(); i != end; ++i)
    {
        PropertyTable propertyTable(entityValue.entity(), i->entity());
        Insert query(propertyTable);

        propertyId = i->id();

        query.insert(PropertyTable::EntityValueId, entityId);
        query.insert(PropertyTable::PropertyValueId, propertyId);

        if (m_database.perform(query, id))
            EntityValue::addValue(entityValue, *i);
        else
        {
            for (--i; i != end; --i)
                EntityValue::removeValue(entityValue, *i);

            return false;
        }
    }

    if (UNLIKELY(m_undoStack.undoAddValue(entityValue, propertyValues) == false))
    {
        for (EntityValue::List::const_iterator i = propertyValues.begin(), end = propertyValues.end(); i != end; ++i)
            EntityValue::removeValue(entityValue, *i);

        return false;
    }

    return true;
}

EntityValue Storage::addValue(const Entity &entity, const ::EFC::Variant &value)
{
    ASSERT(entity.type() != Entity::Composite);

    Entity::Id id;

    EntityTable entityTable(entity);
    Insert query(entityTable);

    Value val;
    ::EFC::Variant tmpVal(value);

    if (set(val, entity, tmpVal))
    {
        query.insert(EntityTable::Value, val);

        if (m_database.perform(query, id))
           return EntityValue::createValue(entity, id, value);
    }

    return EntityValue();
}

bool Storage::updateValue(const EntityValue &value, const ::EFC::Variant &newValue)
{
    ASSERT(value.entity().type() != Entity::Composite);

    EntityTable entityTable(value.entity());
    Update query(entityTable);

    Value val;
    ::EFC::Variant tmpVal(newValue);

    if (set(val, value.entity(), tmpVal))
    {
        Value valueId = value.id();
        Field valueIdField(entityTable, EntityTable::Id);
        ConstConstraint constraint(valueIdField, Constraint::Equal, valueId);

        query.update(EntityTable::Value, val);
        query.where(constraint);

        if (m_database.perform(query))
            return m_undoStack.undoUpdateValue(value, newValue);
    }

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
    PropertyTable propertyTable(entityValue.entity(), propertyValue.entity());
    Delete query(propertyTable);

    Value entityId = entityValue.id();
    Field entityValueId(propertyTable, PropertyTable::EntityValueId);
    ConstConstraint constraint1(entityValueId, Constraint::Equal, entityId);

    Value propertyId = propertyValue.id();
    Field propertyValueId(propertyTable, PropertyTable::PropertyValueId);
    ConstConstraint constraint2(propertyValueId, Constraint::Equal, propertyId);

    GroupConstraint constraint(GroupConstraint::And);
    constraint.add(constraint1);
    constraint.add(constraint2);

    query.where(constraint);

    if (m_database.perform(query))
        return m_undoStack.undoRemoveValue(entityValue, propertyValue);

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
        Value id;
        Value type;
        Value nameValue[2];
        DataSet::Columns::const_iterator column;

        while (dataSet.next())
        {
            column = dataSet.columns().begin();

            (*column).value(id);
            (*(++column)).value(type);
            (*(++column)).value(nameValue[0]);
            (*(++column)).value(nameValue[1]);

            Entity entity(id, static_cast<Entity::Type>(type.i8()), nameValue[0].str(), nameValue[1].str());

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
    Value id;
    Value nameValue;
    DataSet dataSet;
    DataSet::Columns::const_iterator column;
    PropertiesTable propertiesTable;

    Field entityId(propertiesTable, PropertiesTable::EntityId);
    ConstConstraint constraint(entityId, Constraint::Equal, id);
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
                column = dataSet.columns().begin();

                (*(++(++column))).value(id);
                (*(++column)).value(nameValue);

                q = m_entities.find(id);

                if (q != end)
                {
                    (*i).second.m_implementation->add((*q).second, nameValue.str());
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
    Value idValue = id;
    EntityTable entityTable(entity);
    Delete query(entityTable);

    Field fieldId(entityTable, EntityTable::Id);
    ConstConstraint constraint(fieldId, Constraint::Equal, idValue);

    query.where(constraint);

    return m_database.perform(query);
}

bool Storage::removeEntityValues(const Entity &entity, const Entity::IdsList &ids)
{
    EntityTable entityTable(entity);
    Delete query(entityTable);

    Field id(entityTable, EntityTable::Id);
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
                PropertyTable propertyTable((*i).second, property);

                Select query(propertyTable);
                query.select(PropertyTable::PropertyValueId);

                Entity::IdsList tmp(setToList(ids));
                Field propertyValueId(propertyTable, PropertyTable::PropertyValueId);
                SetConstraint constraint(propertyValueId, Constraint::In, tmp);

                query.where(constraint);

                {
                    Value id;
                    DataSet dataSet;
                    DataSet::Columns::const_iterator column = dataSet.columns().begin();

                    if (!m_database.perform(query, dataSet))
                        return false;

                    while (dataSet.next())
                    {
                        (*column).value(id);
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
        PropertyTable propertyTable(entity, property);

        Select query(propertyTable);
        query.select(PropertyTable::PropertyValueId);

        Field entityValueId(propertyTable, PropertyTable::EntityValueId);
        SetConstraint constraint1(entityValueId, Constraint::NotIn, entityIds);

        Entity::IdsList tmp(setToList(propertyIds));
        Field propertyValueId(propertyTable, PropertyTable::PropertyValueId);
        SetConstraint constraint2(propertyValueId, Constraint::In, tmp);

        GroupConstraint constraint(GroupConstraint::And);
        constraint.add(constraint1);
        constraint.add(constraint2);

        query.where(constraint);

        {
            Value id;
            DataSet dataSet;

            if (!m_database.perform(query, dataSet))
                return false;

            DataSet::Columns::const_iterator column = dataSet.columns().begin();

            while (dataSet.next())
            {
                (*column).value(id);
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
        PropertyTable propertyTable((*i).second, entity);

        if (!m_database.remove(propertyTable))
            return false;
    }

    return true;
}

bool Storage::cleanupParentsValues(const Entity &entity, const Entity::IdsList &ids)
{
    for (Entity::Parents::const_iterator i = entity.parents().begin(), end = entity.parents().end(); i != end; ++i)
    {
        PropertyTable propertyTable((*i).second, entity);
        Delete query(propertyTable);

        Field propertyValueId(propertyTable, PropertyTable::PropertyValueId);
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
        PropertyTable propertyTable(entity, (*i).second.entity);

        if (!m_database.remove(propertyTable))
            return false;
    }

    return true;
}


bool Storage::cleanupPropertyValues(const Entity &entity, const Entity::IdsList &ids)
{
    Value id;
    Entity::IdsSet propertyIds;

    for (Entity::Properties::const_iterator i = entity.properties().begin(), end = entity.properties().end(); i != end; ++i)
    {
        PropertyTable propertyTable(entity, (*i).second.entity);

        Select query(propertyTable);
        query.select(PropertyTable::PropertyValueId);

        Field entityValueId(propertyTable, PropertyTable::EntityValueId);
        SetConstraint constraint(entityValueId, Constraint::In, ids);
        query.where(constraint);

        {
            DataSet dataSet;

            if (!m_database.perform(query, dataSet))
                return false;

            DataSet::Columns::const_iterator column = dataSet.columns().begin();
            propertyIds.clear();

            while (dataSet.next())
            {
                (*column).value(id);
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
    PropertyTable propertyTable(entity, property);

    Select query(propertyTable);
    query.select(PropertyTable::PropertyValueId);

    {
        Value id;
        DataSet dataSet;

        if (!m_database.perform(query, dataSet))
            return false;

        DataSet::Columns::const_iterator column = dataSet.columns().begin();

        while (dataSet.next())
        {
            (*column).value(id);
            ids.insert(id);
        }

        if (removeOverlappingIds(entity, property, ids))
            return removeValue(property, setToList(ids));
    }

    return false;
}

}
