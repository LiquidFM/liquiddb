/** @file ldb_EntityTable.cpp
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

#include "ldb_EntityTable.h"

#include <brolly/assert.h>
#include <platform/utils.h>
#include <cstddef>


namespace LiquidDb {

static const Table::Column::Type types[Entity::TypesCount] =
{
    Table::Column::Int,
    Table::Column::Text,
    Table::Column::Date,
    Table::Column::Time,
    Table::Column::DateTime,
    Table::Column::Text,
    Table::Column::Int
};


EntityTable::EntityTable(const Entity &entity) :
    m_type(entity.type()),
    m_columns({
        { Column::BigInt, "ID"    },
        { types[m_type],  "VALUE" }
    })
{
    ASSERT(types[Entity::Int] == Table::Column::Int);
    ASSERT(types[Entity::String] == Table::Column::Text);
    ASSERT(types[Entity::Date] == Table::Column::Date);
    ASSERT(types[Entity::Time] == Table::Column::Time);
    ASSERT(types[Entity::DateTime] == Table::Column::DateTime);
    ASSERT(types[Entity::Memo] == Table::Column::Text);
    ASSERT(types[Entity::Composite] == Table::Column::Int);

    int res = snprintf(m_name, MaxLengthOfName, "ENTITY_%" PRId64, entity.id());

    if (UNLIKELY(res < 0))
        m_name[0] = 0;
}

EntityTable::EntityTable(Entity::Id id, Entity::Type type) :
    m_type(type),
    m_columns({
        { Column::BigInt, "ID"    },
        { types[m_type],  "VALUE" }
    })
{
    ASSERT(types[Entity::Int] == Table::Column::Int);
    ASSERT(types[Entity::String] == Table::Column::Text);
    ASSERT(types[Entity::Date] == Table::Column::Date);
    ASSERT(types[Entity::Time] == Table::Column::Time);
    ASSERT(types[Entity::DateTime] == Table::Column::DateTime);
    ASSERT(types[Entity::Memo] == Table::Column::Text);
    ASSERT(types[Entity::Composite] == Table::Column::Int);

    int res = snprintf(m_name, MaxLengthOfName, "ENTITY_%" PRId64, id);

    if (UNLIKELY(res < 0))
        m_name[0] = 0;
}

EntityTable::~EntityTable()
{}

const char *EntityTable::name() const
{
    return m_name;
}

const Table::Column *EntityTable::primaryKeyColumn() const
{
    return &m_columns[Id];
}

unsigned char EntityTable::columnsCount() const
{
    return m_type == Entity::Composite ? ColumnsCount - 1 : ColumnsCount;
}

const Table::Column *EntityTable::column(unsigned char i) const
{
    return &m_columns[i];
}

unsigned char EntityTable::indexesCount() const
{
    return 0;
}

const Table::Index *EntityTable::index(unsigned char i) const
{
    return NULL;
}

}
