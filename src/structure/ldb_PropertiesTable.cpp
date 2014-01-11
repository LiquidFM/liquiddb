/** @internal @file ldb_PropertiesTable.cpp
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
#include "ldb_PropertiesTable.h"


namespace LiquidDb {

PropertiesTable::PropertiesTable() :
    m_columns({
        { Column::BigInt, "ID"          },
        { Column::BigInt, "ENTITY_ID"   },
        { Column::BigInt, "PROPERTY_ID" },
        { Column::Text,   "NAME"        }
    })
{}

PropertiesTable::~PropertiesTable()
{}

const char *PropertiesTable::name() const
{
    return "PROPERTIES";
}

const Table::Column *PropertiesTable::primaryKeyColumn() const
{
    return &m_columns[Id];
}

unsigned char PropertiesTable::columnsCount() const
{
    return ColumnsCount;
}

const Table::Column *PropertiesTable::column(unsigned char i) const
{
    return &m_columns[i];
}

unsigned char PropertiesTable::indexesCount() const
{
    return 0;
}

const Table::Index *PropertiesTable::index(unsigned char i) const
{
    return NULL;
}

}
