/** @file ldb_Entity.h
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
#ifndef LDB_ENTITY_H_
#define LDB_ENTITY_H_

#include <efc/Set>
#include <efc/Map>
#include <efc/List>
#include <efc/String>
#include <efc/SharedPointer>
#include <liquiddb/Database>


namespace LiquidDb {

class Entity
{
public:
    enum Type
    {
        Int,
        String,
        Date,
        Time,
        DateTime,
        Memo,
        Composite,

        TypesCount
    };

    struct Property;
    typedef Database::Id             Id;
    typedef ::EFC::Set<Id>           IdsSet;
    typedef ::EFC::List<Id>          IdsList;
    typedef ::EFC::Map<Id, Entity>   Parents;
    typedef ::EFC::Map<Id, Property> Properties;

public:
    Entity();
    Entity(Id id, Type type, const char *name, const char *title);
    ~Entity();

    bool operator<(const Entity &other) const { return m_implementation < other.m_implementation; }
    bool operator==(const Entity &other) const { return m_implementation == other.m_implementation; }
    bool operator!=(const Entity &other) const { return m_implementation != other.m_implementation; }

    bool isValid() { return m_implementation != NULL; }

    Id id() const;
    Type type() const;
    const ::EFC::String &name() const;
    const Parents &parents() const;
    const Properties &properties() const;

protected:
    class Implementation;
    typedef ::EFC::SharedPointer<Implementation> Holder;

protected:
    Entity(Holder &holder);

private:
    friend class Storage;
    Holder m_implementation;
};


struct Entity::Property
{
    Property()
    {}
    Property(const Entity &entity, const ::EFC::String &name) :
        entity(entity),
        name(name)
    {}

    bool operator==(const Property &other) const { return entity == other.entity; }

    Entity entity;
    ::EFC::String name;
};

}

#endif /* LDB_ENTITY_H_ */
