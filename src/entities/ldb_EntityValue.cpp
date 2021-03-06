/** @file ldb_EntityValue.cpp
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

#include "ldb_EntityValue_p.h"


namespace LiquidDb {

EntityValue::EntityValue()
{}

EntityValue::~EntityValue()
{}

const Entity &EntityValue::entity() const
{
    return m_implementation->entity();
}

EntityValue::Id EntityValue::id() const
{
    return m_implementation->id();
}

const ::EFC::Variant &EntityValue::value() const
{
    return m_implementation->value();
}

const char *EntityValue::nullString()
{
    static const char nullString[] = "<NULL>";
    return nullString;
}

void EntityValue::addValue(const EntityValue &value, const EntityValue &property)
{
    static_cast<CompositeEntityValue::Implementation *>(value.m_implementation.get())->add(property);
}

void EntityValue::takeValue(const EntityValue &value, const EntityValue &property)
{
    static_cast<CompositeEntityValue::Implementation *>(value.m_implementation.get())->remove(property);
}

::EFC::Variant EntityValue::updateValue(const EntityValue &value, const ::EFC::Variant &newValue)
{
    return static_cast<EntityValue::Implementation *>(value.m_implementation.get())->setValue(newValue);
}

void EntityValue::removeValue(const EntityValue &value, const EntityValue &property)
{
    static_cast<CompositeEntityValue::Implementation *>(value.m_implementation.get())->remove(property);
}

void EntityValue::removeValue(const EntityValue &value, const List &values)
{
    static_cast<CompositeEntityValue::Implementation *>(value.m_implementation.get())->remove(values);
}

EntityValue EntityValue::createSimpleValue(const Entity &entity, EntityValue::Id id, const ::EFC::Variant &value)
{
    return EntityValue(new (std::nothrow) Implementation(entity, id, value));
}

EntityValue EntityValue::createCompositeValue(const Entity &entity, EntityValue::Id id)
{
    return EntityValue(new (std::nothrow) CompositeEntityValue::Implementation(entity, id));
}

EntityValue EntityValue::createCompositeValue(const Entity &entity, EntityValue::Id id, const ::EFC::Variant &value)
{
    return EntityValue(new (std::nothrow) CompositeEntityValue::Implementation(entity, id, value));
}

EntityValue::EntityValue(Holder &holder) :
    m_implementation(std::move(holder))
{}

EntityValue::EntityValue(Implementation *implementation) :
    m_implementation(implementation)
{}

CompositeEntityValue::CompositeEntityValue()
{}

CompositeEntityValue::CompositeEntityValue(const EntityValue &value) :
    EntityValue(value)
{}

const CompositeEntityValue::Values &CompositeEntityValue::values(const Entity &property) const
{
    return static_cast<CompositeEntityValue::Implementation *>(m_implementation.get())->values(property);
}

void CompositeEntityValue::resetValue()
{
    return static_cast<CompositeEntityValue::Implementation *>(m_implementation.get())->resetValue();
}

}
