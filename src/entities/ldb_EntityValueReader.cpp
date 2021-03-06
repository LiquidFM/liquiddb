/** @file ldb_EntityValueReader.cpp
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

#include "ldb_EntityValueReader_p.h"


namespace LiquidDb {

EntityValueReader::EntityValueReader()
{}

EntityValueReader::~EntityValueReader()
{}

bool EntityValueReader::isValid() const
{
    return m_implementation != NULL && m_implementation->isValid();
}

const Entity &EntityValueReader::entity() const
{
    return m_implementation->entity();
}

EntityValue EntityValueReader::next()
{
    return m_implementation->next();
}

bool EntityValueReader::eof() const
{
    return m_implementation->eof();
}

bool EntityValueReader::bof() const
{
    return m_implementation->bof();
}

void EntityValueReader::refresh()
{
    return m_implementation->refresh();
}

void EntityValueReader::close()
{
    return m_implementation->close();
}

EntityValueReader::EntityValueReader(Holder &holder) :
    m_implementation(std::move(holder))
{}

EntityValueReader::EntityValueReader(Implementation *implementation) :
    m_implementation(implementation)
{}

}
