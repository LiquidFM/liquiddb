/** @file ldb_PropertyTable.cpp
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

#include "ldb_PropertyTable.h"

#include <cstddef>


namespace LiquidDb {

PropertyTable::PropertyTable(Entity::Id id, Entity::Id propertyId) :
    m_columns({
        { Column::BigInt, "ID"                },
        { Column::BigInt, "ENTITY_VALUE_ID"   },
        { Column::BigInt, "PROPERTY_VALUE_ID" }
    })
{
    int res = snprintf(m_name, MaxLengthOfName, "ENTITY_%" PRId64 "_PROPERTY_%" PRId64, id, propertyId);

    if (UNLIKELY(res < 0))
        m_name[0] = 0;
}

PropertyTable::PropertyTable(const Entity &entity, const Entity &property) :
    m_columns({
        { Column::BigInt, "ID"                },
        { Column::BigInt, "ENTITY_VALUE_ID"   },
        { Column::BigInt, "PROPERTY_VALUE_ID" }
    })
{
    int res = snprintf(m_name, MaxLengthOfName, "ENTITY_%" PRId64 "_PROPERTY_%" PRId64, entity.id(), property.id());

    if (UNLIKELY(res < 0))
        m_name[0] = 0;
}

PropertyTable::~PropertyTable()
{}

const char *PropertyTable::name() const
{
    return m_name;
}

const Table::Column *PropertyTable::primaryKeyColumn() const
{
    return &m_columns[Id];
}

unsigned char PropertyTable::columnsCount() const
{
    return ColumnsCount;
}

const Table::Column *PropertyTable::column(unsigned char i) const
{
    return &m_columns[i];
}

unsigned char PropertyTable::indexesCount() const
{
    return 0;
}

const Table::Index *PropertyTable::index(unsigned char i) const
{
    return NULL;
}

}
