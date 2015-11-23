/** @file ldb_Entity_p.h
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
#ifndef LDB_ENTITY_P_H_
#define LDB_ENTITY_P_H_

#include <efc/Map>
#include <efc/String>
#include <efc/Variant>
#include <efc/ScopedPointer>
#include "ldb_Entity.h"
#include "ldb_EntityTitle.h"
#include "../structure/ldb_EntitiesTable.h"
#include "../ldb_Value.h"


namespace {
    using namespace LiquidDb;

    static inline bool set(Value &rawValue, const Entity &entity, ::EFC::Variant &value)
    {
        switch (entity.type())
        {
            case Entity::Int:
                rawValue = value.asInt32();
                return true;

            case Entity::String:
            case Entity::Memo:
            {
                char *buf = new (std::nothrow) char[value.size() * 2];

                if (LIKELY(buf != NULL))
                {
                    size_t size = value.size() + 1;
                    memcpy(buf, value.asString(), size);

                    for (size_t i = 0; i < size; ++i)
                        if (buf[i] == '\'')
                        {
                            memmove(buf + i + 1, buf + i, size - i);
                            ++size;
                            ++i;
                        }

                    if (size > value.size() + 1)
                    {
                        value = ::EFC::Variant(buf);

                        if (UNLIKELY(value.isValid() == false))
                        {
                            delete [] buf;
                            return false;
                        }
                    }

                    rawValue = value.asString();
                    delete [] buf;
                    return true;
                }

                return false;
            }

            case Entity::Date:
            case Entity::Time:
            case Entity::DateTime:
                rawValue = value.asUint64();
                return true;

            default:
                return false;
        }
    }

    static inline void get(const Value &rawValue, const Entity &entity, ::EFC::Variant &value)
    {
        switch (entity.type())
        {
            case Entity::Int:
                value.setInt32(rawValue);
                break;

            case Entity::String:
            case Entity::Memo:
            case Entity::Composite:
                value.setString(rawValue);
                break;

            case Entity::Date:
            case Entity::Time:
            case Entity::DateTime:
                value.setUint64(rawValue);
                break;

            default:
                break;
        }
    }
}


namespace LiquidDb {

class Entity::Implementation
{
public:
    Implementation(Id id, Type type, const ::EFC::String &name, const ::EFC::String &title) :
        m_id(id),
        m_type(type),
        m_name(name),
        m_title(title)
    {}

    virtual ~Implementation()
    {}

    Id id() const { return m_id; }
    Type type() const { return m_type; }
    const ::EFC::String &name() const { return m_name; }
    const EntityTitle &title() const { return m_title; }
    const Parents &parents() const { return m_parents; }
    const Properties &properties() const { return m_properties; }

public:
    void addParent(const Entity &parent) { m_parents.insert(Parents::value_type(parent.id(), parent)); }
    void removeParent(const Entity &parent) { m_parents.erase(parent.id()); }

    void add(const Entity &property, const ::EFC::String &name)
    {
        m_properties.insert(Properties::value_type(property.id(), Property(property, name)));
    }
    void add(const Entity &property, ::EFC::String &&name)
    {
        m_properties.insert(Properties::value_type(property.id(), Property(property, std::move(name))));
    }
    ::EFC::String rename(const Entity &property, const ::EFC::String &name)
    {
        Entity::Property &prop = m_properties[property.id()];
        ::EFC::String tmp(std::move(prop.name));
        prop.name = name;

        return tmp;
    }
    void rename(const Entity &property, ::EFC::String &&name)
    {
        Entity::Property &prop = m_properties[property.id()];
        prop.name = std::move(name);
    }
    ::EFC::String remove(const Entity &property)
    {
        Properties::iterator prop = m_properties.find(property.id());
        ::EFC::String tmp(std::move((*prop).second.name));
        m_properties.erase(prop);

        return tmp;
    }

private:
    Id m_id;
    Type m_type;
    ::EFC::String m_name;
    Parents m_parents;
    EntityTitle m_title;
    Properties m_properties;
};

}

#endif /* LDB_ENTITY_P_H_ */
