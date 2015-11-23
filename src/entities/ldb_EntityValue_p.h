/** @file ldb_EntityValue_p.h
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

#ifndef LDB_ENTITYVALUE_P_H_
#define LDB_ENTITYVALUE_P_H_

#include "ldb_EntityValue.h"
#include "ldb_EntityTitle.h"

#include <platform/utils.h>


namespace LiquidDb {

class EntityValue::Implementation
{
public:
    Implementation(const Entity &entity, Id id, const ::EFC::Variant &value) :
        m_value(value),
        m_entity(entity),
        m_id(id)
    {}

    virtual ~Implementation()
    {}

    const Entity &entity() const { return m_entity; }

    Id id() const { return m_id; }
    virtual const ::EFC::Variant &value() const { return m_value; }
    ::EFC::Variant setValue(const ::EFC::Variant &value) { ::EFC::Variant tmp(m_value); m_value = value; return tmp; }

protected:
    Implementation(const Entity &entity, Id id) :
        m_entity(entity),
        m_id(id)
    {}

protected:
    mutable ::EFC::Variant m_value;

private:
    Entity m_entity;
    Id m_id;
};


class CompositeEntityValue::Implementation : public EntityValue::Implementation
{
public:
    Implementation(const Entity &entity, Id id) :
        EntityValue::Implementation(entity, id)
    {}

    Implementation(const Entity &entity, Id id, const ::EFC::Variant &value) :
        EntityValue::Implementation(entity, id, value)
    {}

    virtual ~Implementation()
    {}

    virtual const ::EFC::Variant &value() const
    {
        enum { BufferSize = 1024 };

        if (m_value.isValid())
            return m_value;
        else
        {
            bool ok;
            ::EFC::String val;

            for (auto i : entity().title())
                switch (i.type())
                {
                    case EntityTitle::Token::Text:
                    {
                        val += i.string();
                        break;
                    }

                    case EntityTitle::Token::Property:
                    {
                        ok = false;

                        for (auto q : entity().properties())
                            if (q.second.name == i.string())
                            {
                                Map::const_iterator values = m_items.find(q.second.entity);

                                if (values == m_items.end() || (*values).second.empty())
                                    val += nullString();
                                else
                                {
                                    char buffer[BufferSize];
                                    int res = 0;

                                    ::EFC::Variant value = (*(*values).second.begin()).second.value();

                                    switch (value.type())
                                    {
                                        case ::EFC::Variant::UINT8:
                                        case ::EFC::Variant::INT8:
                                        case ::EFC::Variant::UINT16:
                                        case ::EFC::Variant::INT16:
                                        case ::EFC::Variant::UINT32:
                                        case ::EFC::Variant::INT32:
                                        case ::EFC::Variant::UINT64:
                                        case ::EFC::Variant::INT64:
                                        case ::EFC::Variant::BOOL:
                                        case ::EFC::Variant::CHAR:
                                            res = snprintf(buffer, BufferSize, "%" PRId64, value.asInt64());
                                            break;

                                        case ::EFC::Variant::FLOAT:
                                            res = snprintf(buffer, BufferSize, "%f", value.asFloat());
                                            break;

                                        case ::EFC::Variant::DOUBLE:
                                            res = snprintf(buffer, BufferSize, "%f", value.asFloat());
                                            break;

                                        case ::EFC::Variant::STRING:
                                        case ::EFC::Variant::BINARY:
                                            res = snprintf(buffer, BufferSize, "%s", value.asString());
                                            break;
                                    }

                                    if (LIKELY(res > 0))
                                        val += buffer;
                                    else
                                        val += nullString();
                                }

                                ok = true;
                                break;
                            }

                        if (!ok)
                            val += nullString();

                        break;
                    }
                }

            return m_value = val.c_str();
        }
    }

    void resetValue()
    {
        m_value = ::EFC::Variant();
    }

    void add(const EntityValue &value)
    {
        m_items[value.entity()][value.id()] = value;
        m_value = ::EFC::Variant();
    }

    void add(const List &values)
    {
        for (List::const_iterator i = values.begin(), end = values.end(); i != end; ++i)
            m_items[i->entity()][i->id()] = (*i);

        m_value = ::EFC::Variant();
    }

    void remove(const EntityValue &value)
    {
        m_items[value.entity()].erase(value.id());
        m_value = ::EFC::Variant();
    }

    void remove(const List &values)
    {
        for (List::const_iterator i = values.begin(), end = values.end(); i != end; ++i)
            m_items[i->entity()].erase(i->id());

        m_value = ::EFC::Variant();
    }

    EntityValue value(const Entity &property, Id id)
    {
        Values &map = m_items[property];
        Values::iterator index = map.find(id);

        if (index == map.end())
            return EntityValue();
        else
            return (*index).second;
    }

    const Values &values(const Entity &property) const
    {
        Map::const_iterator i = m_items.find(property);

        if (i == m_items.end())
            return m_empty;
        else
            return (*i).second;
    }

private:
    typedef ::EFC::Map<Entity, Values> Map;
    Map m_items;
    Values m_empty;
};

}

#endif /* LDB_ENTITYVALUE_P_H_ */
