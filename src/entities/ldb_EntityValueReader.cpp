/** @file ldb_EntityValueReader.cpp
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

#include "ldb_EntityValueReader.h"


namespace LiquidDb {

EntityValueReader::EntityValueReader() :
    m_afterLast(false),
    m_beforeFirst(false)
{}

EntityValueReader::EntityValueReader(const SelectEntity &query)
{}

EntityValue EntityValueReader::next() const
{
    return EntityValue();
}

void EntityValueReader::refresh()
{

}

void EntityValueReader::addValue(const EntityValue &value, const EntityValue &property)
{

}

void EntityValueReader::takeValue(const EntityValue &value, const EntityValue &property)
{

}

void EntityValueReader::updateValue(const EntityValue &value, const ::EFC::Variant &newValue)
{

}

void EntityValueReader::removeValue(const EntityValue &value, const EntityValue &property)
{

}

void EntityValueReader::removeValue(const EntityValue &value, const CompositeEntityValue::Values &values)
{

}

EntityValue EntityValueReader::createValue(const Entity &entity, EntityValue::Id id)
{
    return EntityValue();
}

EntityValue EntityValueReader::createValue(const Entity &entity, EntityValue::Id id, const ::EFC::Variant &value)
{
    return EntityValue();
}

EntityValue EntityValueReader::doNext() const
{
    return EntityValue();
}

EntityValue EntityValueReader::value(const Entity &entity, Entity::Id id, int column) const
{
    return EntityValue();
}

void EntityValueReader::property(const EntityValue &value, const Entity &property, int &column) const
{

}

void EntityValueReader::skip(const Entity &property, int &column) const
{

}

}
