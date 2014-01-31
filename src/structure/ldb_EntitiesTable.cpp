/** @internal @file ldb_EntitiesTable.cpp
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
#include "ldb_EntitiesTable.h"

#include <cstddef>


namespace LiquidDb {

EntitiesTable::EntitiesTable() :
    m_columns({
        { Column::BigInt,   "ID"    },
        { Column::SmallInt, "TYPE"  },
        { Column::Text,     "NAME"  },
        { Column::Text,     "TITLE" }
    })
{}

EntitiesTable::~EntitiesTable()
{}

const char *EntitiesTable::name() const
{
    return "ENTITIES";
}

const Table::Column *EntitiesTable::primaryKeyColumn() const
{
    return &m_columns[Id];
}

EntitiesTable::Column::Id EntitiesTable::columnsCount() const
{
    return ColumnsCount;
}

const Table::Column *EntitiesTable::column(Column::Id i) const
{
    return &m_columns[i];
}

EntitiesTable::Column::Id EntitiesTable::indexesCount() const
{
    return 0;
}

const Table::Index *EntitiesTable::index(Column::Id i) const
{
    return NULL;
}

}
