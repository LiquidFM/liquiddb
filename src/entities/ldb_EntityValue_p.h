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
    virtual ::EFC::Variant value() const { return m_value; }
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

    virtual ~Implementation()
    {}

    virtual ::EFC::Variant value() const
    {
//        if (m_value.isValid())
            return m_value;
//        else
//        {
//            QString val;
//            const EntityTitle &format = entity()->shortFormat();
//
//            for (EntityTitle::size_type i = 0, size = format.size(); i < size; ++i)
//                switch (format.at(i).type())
//                {
//                    case EntityTitle::Token::Text:
//                    {
//                        val.append(format.at(i).string());
//                        break;
//                    }
//                    case EntityTitle::Token::Property:
//                    {
//                        Entity::size_type index = entity()->indexOf(format.at(i).string());
//
//                        if (index == Entity::InvalidIndex)
//                            val.append(tr("Property \"%1\" does not exists").arg(format.at(i).string()));
//                        else
//                        {
//                            const InternalList values = m_items.value(entity()->at(index).entity);
//
//                            if (values.isEmpty())
//                                val.append(tr("Property \"%1\" is empty").arg(format.at(i).string()));
//                            else
//                                val.append(values.at(0)->value().toString());
//                        }
//
//                        break;
//                    }
//                }
//
//            return m_value = val;
//        }
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
