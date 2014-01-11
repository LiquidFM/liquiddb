/** @file ldb_EntityValue.h
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

#ifndef LDB_ENTITYVALUE_H_
#define LDB_ENTITYVALUE_H_

#include <efc/Map>
#include <efc/List>
#include <efc/Variant>
#include <efc/SharedPointer>
#include <liquiddb/Entity>


namespace LiquidDb {

class EntityValue
{
public:
    typedef Entity::Id Id;

public:
    EntityValue();
    ~EntityValue();

    bool operator<(const EntityValue &other) const { return m_implementation < other.m_implementation; }
    bool operator==(const EntityValue &other) const { return m_implementation == other.m_implementation; }
    bool operator!=(const EntityValue &other) const { return m_implementation != other.m_implementation; }

    const Entity &entity() const;

    Id id() const;
    ::EFC::Variant value() const;

protected:
    class Implementation;
    typedef ::EFC::SharedPointer<Implementation> Holder;

protected:
    EntityValue(Holder &holder);

private:
    Holder m_implementation;
};


class CompositeEntityValue : public EntityValue
{
public:
    typedef ::EFC::List<EntityValue> Values;

public:
    CompositeEntityValue();
    CompositeEntityValue(const EntityValue &value);

    Values values(const Entity &property) const;
    void resetValue();

private:
    class Implementation;
};


///* Meta-function "EntityValueType" */
//template <Database::EntityType EntityType> struct EntityValueType {};
//template <> struct EntityValueType<Database::Int>      { typedef int       type; };
//template <> struct EntityValueType<Database::String>   { typedef QString   type; };
//template <> struct EntityValueType<Database::Date>     { typedef QDate     type; };
//template <> struct EntityValueType<Database::Time>     { typedef QTime     type; };
//template <> struct EntityValueType<Database::DateTime> { typedef QDateTime type; };
//template <> struct EntityValueType<Database::Memo>     { typedef QString   type; };
//template <> struct EntityValueType<Database::Rating>   { typedef int       type; };
//template <> struct EntityValueType<Database::Path>     { typedef QString   type; };

}

#endif /* LDB_ENTITYVALUE_H_ */
