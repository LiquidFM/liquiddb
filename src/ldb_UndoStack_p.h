/** @file ldb_UndoStack_p.h
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

#ifndef LDB_UNDOSTACK_P_H_
#define LDB_UNDOSTACK_P_H_

#include "ldb_UndoStack.h"
#include "entities/ldb_Entity_p.h"
#include "entities/ldb_EntityValue_p.h"

#include <efc/Map>


/* FIXME: */
WARN_UNUSED_RETURN_OFF


namespace LiquidDb {

class UndoStack::Command
{
public:
    typedef ::EFC::Map<Entity::Id, Entity> Entities;

public:
    virtual ~Command()
    {}

    virtual void undo(Entities &entities) = 0;
};


class UndoUpdateValue : public UndoStack::Command
{
public:
    UndoUpdateValue(const EntityValue &value, const ::EFC::Variant &newValue) :
        m_value(value)
    {
        m_oldValue = std::move(EntityValue::updateValue(value, newValue));
    }

    virtual void undo(Entities &entities)
    {
        EntityValue::updateValue(m_value, m_oldValue);
    }

private:
    EntityValue m_value;
    ::EFC::Variant m_oldValue;
};


class UndoRenameProperty : public UndoStack::Command
{
public:
    UndoRenameProperty(const Entity &entity, const Entity &property, const char *name) :
        m_entity(entity),
        m_property(property)
    {
        m_name = std::move(m_entity.m_implementation->rename(m_property, name));
    }

    virtual void undo(Entities &entities)
    {
       m_entity.m_implementation->rename(m_property, std::move(m_name));
    }

private:
    Entity m_entity;
    Entity m_property;
    ::EFC::String m_name;
};


class UndoRemoveValue : public UndoStack::Command
{
public:
    UndoRemoveValue(const EntityValue &entityValue, const EntityValue &propertyValue) :
        m_entityValue(entityValue),
        m_propertyValue(propertyValue)
    {
        EntityValue::takeValue(m_entityValue, m_propertyValue);
    }

    virtual void undo(Entities &entities)
    {
        EntityValue::addValue(m_entityValue, m_propertyValue);
    }

private:
    EntityValue m_entityValue;
    EntityValue m_propertyValue;
};


class UndoRemoveProperty : public UndoStack::Command
{
public:
    UndoRemoveProperty(const Entity &entity, const Entity &property) :
        m_entity(entity),
        m_property(property)
    {
        m_name = std::move(m_entity.m_implementation->remove(m_property));
        m_property.m_implementation->removeParent(m_entity);
    }

    virtual void undo(Entities &entities)
    {
        m_entity.m_implementation->add(m_property, std::move(m_name));
        m_property.m_implementation->addParent(m_entity);
    }

private:
    Entity m_entity;
    Entity m_property;
    ::EFC::String m_name;
};


class UndoRemoveEntity : public UndoStack::Command
{
public:
    UndoRemoveEntity(const Entity &entity, Entities &entities) :
        m_entity(entity)
    {
        for (Entity::Parents::const_iterator i = m_entity.parents().begin(), end = m_entity.parents().end(); i != end; ++i)
            m_names[(*i).second.id()] = std::move((*i).second.m_implementation->remove(m_entity));

        for (Entity::Properties::const_iterator i = m_entity.properties().begin(), end = m_entity.properties().end(); i != end; ++i)
            (*i).second.entity.m_implementation->removeParent(m_entity);

        entities.erase(m_entity.id());
    }

    virtual void undo(Entities &entities)
    {
        entities[m_entity.id()] = m_entity;

        for (Entity::Parents::const_iterator i = m_entity.parents().begin(), end = m_entity.parents().end(); i != end; ++i)
            (*i).second.m_implementation->add(m_entity, std::move(m_names[(*i).second.id()]));

        for (Entity::Properties::const_iterator i = m_entity.properties().begin(), end = m_entity.properties().end(); i != end; ++i)
            (*i).second.entity.m_implementation->addParent(m_entity);
    }

private:
    Entity m_entity;
    UndoStack::Names m_names;
};


class UndoAddValue : public UndoStack::Command
{
public:
    UndoAddValue(const EntityValue &entityValue, const EntityValue &propertyValue) :
        m_entityValue(entityValue)
    {
        EntityValue::addValue(m_entityValue, propertyValue);
        m_propertyValues.push_back(propertyValue);
    }

    UndoAddValue(const EntityValue &entityValue, const EntityValue::List &propertyValues) :
        m_entityValue(entityValue),
        m_propertyValues(propertyValues)
    {}

    virtual void undo(Entities &entities)
    {
        EntityValue::removeValue(m_entityValue, m_propertyValues);
    }

private:
    EntityValue m_entityValue;
    EntityValue::List m_propertyValues;
};


class UndoAddProperty : public UndoStack::Command
{
public:
    UndoAddProperty(const Entity &entity, const Entity &property, const char *name) :
        m_entity(entity),
        m_property(property)
    {
        m_entity.m_implementation->add(property, name);
        m_property.m_implementation->addParent(entity);
    }

    virtual void undo(Entities &entities)
    {
        m_property.m_implementation->removeParent(m_entity);
        m_entity.m_implementation->remove(m_property);
    }

private:
    Entity m_entity;
    Entity m_property;
};


class UndoAddEntity : public UndoStack::Command
{
public:
    UndoAddEntity(Entity::Id id, Entity::Type type, const char *name, const char *title, Entities &entities) :
        m_entity(id, type, name, title)
    {
        entities[m_entity.id()] = m_entity;
    }

    virtual void undo(Entities &entities)
    {
        entities.erase(m_entity.id());

        for (Entity::Properties::const_iterator i = m_entity.properties().begin(), end = m_entity.properties().end(); i != end; ++i)
            (*i).second.entity.m_implementation->removeParent(m_entity);
    }

    const Entity &entity() const { return m_entity; }

private:
    Entity m_entity;
};

}

#endif /* LDB_UNDOSTACK_P_H_ */
