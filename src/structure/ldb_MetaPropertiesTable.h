/** @file ldb_MetaPropertiesTable.h
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

#ifndef LDB_METAPROPERTIESTABLE_H_
#define LDB_METAPROPERTIESTABLE_H_

#include <liquiddb/Table>


namespace LiquidDb {

class MetaPropertiesTable : public Table
{
    PLATFORM_MAKE_NONCOPYABLE(MetaPropertiesTable)
    PLATFORM_MAKE_NONMOVEABLE(MetaPropertiesTable)

public:
    enum Columns
    {
        Id,
        EntityId,
        Value,

        ColumnsCount
    };

public:
    MetaPropertiesTable();
    virtual ~MetaPropertiesTable();

    virtual const char *name() const;
    virtual const Column *primaryKeyColumn() const;

    virtual Column::Id columnsCount() const;
    virtual const Column *column(Column::Id i) const;

    virtual Column::Id indexesCount() const;
    virtual const Index *index(Column::Id i) const;

private:
    Column m_columns[ColumnsCount];
};

}

#endif /* LDB_METAPROPERTIESTABLE_H_ */
