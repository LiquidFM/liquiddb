/** @internal @file ldb_Entity.cpp
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
#include "ldb_Entity_p.h"


namespace LiquidDb {

Entity::Entity()
{}

Entity::Entity(Id id, Type type, const ::EFC::String &name, const ::EFC::String &title) :
    m_implementation(new (std::nothrow) Implementation(id, type, name, title))
{}

Entity::~Entity()
{}

Entity::Id Entity::id() const
{
    return m_implementation->id();
}

Entity::Type Entity::type() const
{
    return m_implementation->type();
}

const ::EFC::String &Entity::name() const
{
    return m_implementation->name();
}

const EntityTitle &Entity::title() const
{
    return m_implementation->title();
}

const Entity::Parents &Entity::parents() const
{
    return m_implementation->parents();
}

const Entity::Properties &Entity::properties() const
{
    return m_implementation->properties();
}

Entity::Entity(Holder &holder) :
    m_implementation(std::move(holder))
{}

}
