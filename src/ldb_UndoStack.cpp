/** @file ldb_UndoStack.cpp
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

#include "ldb_UndoStack_p.h"

#include <brolly/assert.h>


namespace LiquidDb {

UndoStack::UndoStack(Entities &entities) :
    m_entities(entities)
{}

UndoStack::~UndoStack()
{}

bool UndoStack::transaction()
{
    return m_stack.push_back(List());
}

void UndoStack::commit()
{

}

void UndoStack::rollback()
{

}

Entity UndoStack::undoAddEntity(Entity::Id id, Entity::Type type, const ::EFC::String &name, const ::EFC::String &title)
{
    ASSERT(!m_stack.empty());
    Holder holder(new (std::nothrow) UndoAddEntity(id, type, name, title, m_entities));

    if (LIKELY(holder != NULL))
        if (LIKELY(m_stack.back().push_back(holder) == true))
            return static_cast<UndoAddEntity *>(holder.get())->entity();

    return Entity();
}

bool UndoStack::undoRemoveEntity(const Entity &entity)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoRemoveEntity(entity, m_entities))));
}

bool UndoStack::undoAddProperty(const Entity &entity, const Entity &property, const ::EFC::String &name)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoAddProperty(entity, property, name))));
}

bool UndoStack::undoRenameProperty(const Entity &entity, const Entity &property, const ::EFC::String &name)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoRenameProperty(entity, property, name))));
}

bool UndoStack::undoRemoveProperty(const Entity &entity, const Entity &property)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoRemoveProperty(entity, property))));
}

bool UndoStack::undoAddValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoAddValue(entityValue, propertyValue))));
}

bool UndoStack::undoAddValue(const EntityValue &entityValue, const EntityValue::List &propertyValues)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoAddValue(entityValue, propertyValues))));
}

bool UndoStack::undoUpdateValue(const EntityValue &entityValue, const ::EFC::Variant &value)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoUpdateValue(entityValue, value))));
}

bool UndoStack::undoRemoveValue(const EntityValue &entityValue, const EntityValue &propertyValue)
{
    ASSERT(!m_stack.empty());
    return m_stack.back().push_back(std::move(Holder(new (std::nothrow) UndoRemoveValue(entityValue, propertyValue))));
}

}
