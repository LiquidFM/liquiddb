/** @file ldb_EntityConstraint.cpp
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

#include "ldb_EntityConstraint.h"
#include "ldb_Entity_p.h"
#include "../ldb_conversions_p.h"

#include "../structure/ldb_EntitiesTable.h"
#include "../structure/ldb_PropertiesTable.h"
#include "../structure/ldb_EntityTable.h"
#include "../structure/ldb_PropertyTable.h"

#include <brolly/assert.h>


namespace LiquidDb {

EntityConstraint::EntityConstraint(const Entity &property, Operator op, const EntityValue &value) :
    m_property(property),
    m_op(op),
    m_value(value)
{
    ASSERT(value.value().isValid());
    ASSERT(property == value.entity());
    ASSERT(value.id() != EntityValue::InvalidId || value.entity().type() != Entity::Composite);
}

int EntityConstraint::build(char *buffer, size_t size) const
{
    if (m_value.id() == EntityValue::InvalidId)
    {
        EntityTable entityTable(m_property);
        Field field(entityTable, EntityTable::Value);

        Value val;
        ::EFC::Variant tmpVal(m_value.value());

        if (set(val, m_property, tmpVal))
        {
            ConstConstraint constraint(field, m_op, val);
            return constraint.build(buffer, size);
        }

        return -1;
    }
    else
    {
        Value id = m_value.id();
        EntityTable entityTable(m_property);
        Field field(entityTable, EntityTable::Id);
        ConstConstraint constraint(field, m_op, id);

        return constraint.build(buffer, size);
    }
}

}
