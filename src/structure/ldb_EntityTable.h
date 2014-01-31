/** @file ldb_EntityTable.h
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

#ifndef LDB_ENTITYTABLE_H_
#define LDB_ENTITYTABLE_H_

#include <liquiddb/Table>
#include <liquiddb/Entity>


namespace LiquidDb {

class EntityTable : public Table
{
    PLATFORM_MAKE_NONCOPYABLE(EntityTable)
    PLATFORM_MAKE_NONMOVEABLE(EntityTable)

public:
    enum Columns
    {
        Id,
        Value,

        ColumnsCount
    };

    enum
    {
        MaxLengthOfName = 32
    };

public:
    EntityTable(const Entity &entity);
    EntityTable(Entity::Id id, Entity::Type type);
    virtual ~EntityTable();

    virtual const char *name() const;
    virtual const Column *primaryKeyColumn() const;

    virtual Column::Id columnsCount() const;
    virtual const Column *column(Column::Id i) const;

    virtual Column::Id indexesCount() const;
    virtual const Index *index(Column::Id i) const;

private:
    Entity::Type m_type;
    char m_name[MaxLengthOfName];
    Column m_columns[ColumnsCount];
};

}

#endif /* LDB_ENTITYTABLE_H_ */
