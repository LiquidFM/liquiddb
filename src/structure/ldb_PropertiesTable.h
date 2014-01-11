/** @file ldb_PropertiesTable.h
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
#ifndef LDB_PROPERTIESTABLE_H_
#define LDB_PROPERTIESTABLE_H_

#include <liquiddb/Table>


namespace LiquidDb {

class PropertiesTable : public Table
{
    PLATFORM_MAKE_NONCOPYABLE(PropertiesTable)
    PLATFORM_MAKE_NONMOVEABLE(PropertiesTable)

public:
    enum Columns
    {
        Id,
        EntityId,
        PropertyId,
        Name,

        ColumnsCount
    };

public:
    PropertiesTable();
    virtual ~PropertiesTable();

    virtual const char *name() const;
    virtual const Column *primaryKeyColumn() const;

    virtual unsigned char columnsCount() const;
    virtual const Column *column(unsigned char i) const;

    virtual unsigned char indexesCount() const;
    virtual const Index *index(unsigned char i) const;

private:
    Column m_columns[ColumnsCount];
};

}

#endif /* LDB_PROPERTIESTABLE_H_ */
