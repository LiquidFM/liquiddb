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
    typedef Entity::Id                  Id;
    typedef ::EFC::List<EntityValue>    List;
    typedef ::EFC::Map<Id, EntityValue> Values;
    enum { InvalidId = Entity::InvalidId };

public:
    EntityValue();
    ~EntityValue();

    bool operator<(const EntityValue &other) const { return m_implementation < other.m_implementation; }
    bool operator==(const EntityValue &other) const { return m_implementation == other.m_implementation; }
    bool operator!=(const EntityValue &other) const { return m_implementation != other.m_implementation; }

    bool isValid() const { return m_implementation != NULL; }
    const Entity &entity() const;

    Id id() const;
    ::EFC::Variant value() const;

    static void addValue(const EntityValue &value, const EntityValue &property);
    static void takeValue(const EntityValue &value, const EntityValue &property);
    static ::EFC::Variant updateValue(const EntityValue &value, const ::EFC::Variant &newValue);
    static void removeValue(const EntityValue &value, const EntityValue &property);
    static void removeValue(const EntityValue &value, const Values &values);
    static EntityValue createValue(const Entity &entity, EntityValue::Id id);
    static EntityValue createValue(const Entity &entity, EntityValue::Id id, const ::EFC::Variant &value);

protected:
    class Implementation;
    typedef ::EFC::SharedPointer<Implementation> Holder;

protected:
    EntityValue(Holder &holder);
    EntityValue(Implementation *implementation);

protected:
    friend class EntityValueReader;
    Holder m_implementation;
};


class CompositeEntityValue : public EntityValue
{
public:
    CompositeEntityValue();
    CompositeEntityValue(const EntityValue &value);

    const Values &values(const Entity &property) const;
    void resetValue();

private:
    friend class EntityValue;
    friend class EntityValueReader;
    class Implementation;
};

}

#endif /* LDB_ENTITYVALUE_H_ */
