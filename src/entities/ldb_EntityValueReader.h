/** @file ldb_EntityValueReader.h
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

#ifndef LDB_ENTITYVALUEREADER_H_
#define LDB_ENTITYVALUEREADER_H_

#include <efc/Variant>
#include <liquiddb/Query>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class EntityValueReader
{
public:
    EntityValueReader();
    EntityValueReader(const Entity &query);

//    bool isValid() const { return m_context.isValid(); }
//    const Entity &entity() const { return m_context.entity(); }

    EntityValue next() const;
    bool eof() const { return m_afterLast; }
    bool bof() const { return m_beforeFirst; }
    void close() { m_afterLast = true; }
    void refresh();

private:
    EntityValue doNext() const;
    EntityValue value(const Entity &entity, Entity::Id id, int column) const;
    void property(const EntityValue &value, const Entity &property, int &column) const;
    void skip(const Entity &property, int &column) const;

private:
    mutable bool m_afterLast;
    mutable bool m_beforeFirst;
};

}

#endif /* LDB_ENTITYVALUEREADER_H_ */
