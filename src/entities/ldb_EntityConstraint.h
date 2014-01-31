/** @file ldb_EntityConstraint.h
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

#ifndef LDB_ENTITYCONSTRAINT_H_
#define LDB_ENTITYCONSTRAINT_H_

#include <liquiddb/Constraint>
#include <liquiddb/EntityValue>


namespace LiquidDb {

class EntityConstraint : public Constraint
{
public:
    EntityConstraint(const Entity &property, Operator op, const EntityValue &value);

    const Entity &property() const { return m_property; }
    Operator op() const { return m_op; }
    const EntityValue &value() const { return m_value; }

    virtual int build(char *buffer, size_t size) const;

private:
    Entity m_property;
    Operator m_op;
    EntityValue m_value;
};

}

#endif /* LDB_ENTITYCONSTRAINT_H_ */
